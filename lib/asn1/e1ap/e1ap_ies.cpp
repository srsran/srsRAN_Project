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

#include "srsran/asn1/e1ap/e1ap_ies.h"
using namespace asn1;
using namespace asn1::e1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// DRB-Activity ::= ENUMERATED
const char* drb_activity_opts::to_string() const
{
  static const char* names[] = {"active", "not-active"};
  return convert_enum_idx(names, 2, value, "drb_activity_e");
}

// PDU-Session-Resource-Activity ::= ENUMERATED
const char* pdu_session_res_activity_opts::to_string() const
{
  static const char* names[] = {"active", "not-active"};
  return convert_enum_idx(names, 2, value, "pdu_session_res_activity_e");
}

// DRB-Activity-Item ::= SEQUENCE
SRSASN_CODE drb_activity_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(drb_activity.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_activity_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(drb_activity.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_activity_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_str("dRB-Activity", drb_activity.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-Activity-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_activity_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pdu_session_res_activity.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_activity_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pdu_session_res_activity.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_activity_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.write_str("pDU-Session-Resource-Activity", pdu_session_res_activity.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// UE-Activity ::= ENUMERATED
const char* ue_activity_opts::to_string() const
{
  static const char* names[] = {"active", "not-active"};
  return convert_enum_idx(names, 2, value, "ue_activity_e");
}

// ActivityInformation ::= CHOICE
void activity_info_c::destroy_()
{
  switch (type_) {
    case types::drb_activity_list:
      c.destroy<drb_activity_list_l>();
      break;
    case types::pdu_session_res_activity_list:
      c.destroy<pdu_session_res_activity_list_l>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<activity_info_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void activity_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::drb_activity_list:
      c.init<drb_activity_list_l>();
      break;
    case types::pdu_session_res_activity_list:
      c.init<pdu_session_res_activity_list_l>();
      break;
    case types::ue_activity:
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<activity_info_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "activity_info_c");
  }
}
activity_info_c::activity_info_c(const activity_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::drb_activity_list:
      c.init(other.c.get<drb_activity_list_l>());
      break;
    case types::pdu_session_res_activity_list:
      c.init(other.c.get<pdu_session_res_activity_list_l>());
      break;
    case types::ue_activity:
      c.init(other.c.get<ue_activity_e>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "activity_info_c");
  }
}
activity_info_c& activity_info_c::operator=(const activity_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::drb_activity_list:
      c.set(other.c.get<drb_activity_list_l>());
      break;
    case types::pdu_session_res_activity_list:
      c.set(other.c.get<pdu_session_res_activity_list_l>());
      break;
    case types::ue_activity:
      c.set(other.c.get<ue_activity_e>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "activity_info_c");
  }

  return *this;
}
drb_activity_list_l& activity_info_c::set_drb_activity_list()
{
  set(types::drb_activity_list);
  return c.get<drb_activity_list_l>();
}
pdu_session_res_activity_list_l& activity_info_c::set_pdu_session_res_activity_list()
{
  set(types::pdu_session_res_activity_list);
  return c.get<pdu_session_res_activity_list_l>();
}
ue_activity_e& activity_info_c::set_ue_activity()
{
  set(types::ue_activity);
  return c.get<ue_activity_e>();
}
protocol_ie_single_container_s<activity_info_ext_ies_o>& activity_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>();
}
void activity_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drb_activity_list:
      j.start_array("dRB-Activity-List");
      for (const auto& e1 : c.get<drb_activity_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pdu_session_res_activity_list:
      j.start_array("pDU-Session-Resource-Activity-List");
      for (const auto& e1 : c.get<pdu_session_res_activity_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ue_activity:
      j.write_str("uE-Activity", c.get<ue_activity_e>().to_string());
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "activity_info_c");
  }
  j.end_obj();
}
SRSASN_CODE activity_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::drb_activity_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_activity_list_l>(), 1, 32, true));
      break;
    case types::pdu_session_res_activity_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_activity_list_l>(), 1, 256, true));
      break;
    case types::ue_activity:
      HANDLE_CODE(c.get<ue_activity_e>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "activity_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE activity_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::drb_activity_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_activity_list_l>(), bref, 1, 32, true));
      break;
    case types::pdu_session_res_activity_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_activity_list_l>(), bref, 1, 256, true));
      break;
    case types::ue_activity:
      HANDLE_CODE(c.get<ue_activity_e>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "activity_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* activity_info_c::types_opts::to_string() const
{
  static const char* names[] = {
      "dRB-Activity-List", "pDU-Session-Resource-Activity-List", "uE-Activity", "choice-extension"};
  return convert_enum_idx(names, 4, value, "activity_info_c::types");
}

// PacketErrorRate ::= SEQUENCE
SRSASN_CODE packet_error_rate_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, per_scalar, (uint8_t)0u, (uint8_t)9u, true, true));
  HANDLE_CODE(pack_integer(bref, per_exponent, (uint8_t)0u, (uint8_t)9u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE packet_error_rate_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(per_scalar, bref, (uint8_t)0u, (uint8_t)9u, true, true));
  HANDLE_CODE(unpack_integer(per_exponent, bref, (uint8_t)0u, (uint8_t)9u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void packet_error_rate_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pER-Scalar", per_scalar);
  j.write_int("pER-Exponent", per_exponent);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// AlternativeQoSParaSetItem ::= SEQUENCE
SRSASN_CODE alt_qos_para_set_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(guaranteed_flow_bit_rate_dl_present, 1));
  HANDLE_CODE(bref.pack(guaranteed_flow_bit_rate_ul_present, 1));
  HANDLE_CODE(bref.pack(packet_delay_budget_present, 1));
  HANDLE_CODE(bref.pack(packet_error_rate_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, alt_qos_param_idx, (uint8_t)1u, (uint8_t)8u, true, true));
  if (guaranteed_flow_bit_rate_dl_present) {
    HANDLE_CODE(pack_integer(bref, guaranteed_flow_bit_rate_dl, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (guaranteed_flow_bit_rate_ul_present) {
    HANDLE_CODE(pack_integer(bref, guaranteed_flow_bit_rate_ul, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (packet_delay_budget_present) {
    HANDLE_CODE(pack_integer(bref, packet_delay_budget, (uint16_t)0u, (uint16_t)1023u, true, true));
  }
  if (packet_error_rate_present) {
    HANDLE_CODE(packet_error_rate.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE alt_qos_para_set_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(guaranteed_flow_bit_rate_dl_present, 1));
  HANDLE_CODE(bref.unpack(guaranteed_flow_bit_rate_ul_present, 1));
  HANDLE_CODE(bref.unpack(packet_delay_budget_present, 1));
  HANDLE_CODE(bref.unpack(packet_error_rate_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(alt_qos_param_idx, bref, (uint8_t)1u, (uint8_t)8u, true, true));
  if (guaranteed_flow_bit_rate_dl_present) {
    HANDLE_CODE(unpack_integer(guaranteed_flow_bit_rate_dl, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (guaranteed_flow_bit_rate_ul_present) {
    HANDLE_CODE(unpack_integer(guaranteed_flow_bit_rate_ul, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (packet_delay_budget_present) {
    HANDLE_CODE(unpack_integer(packet_delay_budget, bref, (uint16_t)0u, (uint16_t)1023u, true, true));
  }
  if (packet_error_rate_present) {
    HANDLE_CODE(packet_error_rate.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void alt_qos_para_set_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("alternativeQoSParameterIndex", alt_qos_param_idx);
  if (guaranteed_flow_bit_rate_dl_present) {
    j.write_int("guaranteedFlowBitRateDL", guaranteed_flow_bit_rate_dl);
  }
  if (guaranteed_flow_bit_rate_ul_present) {
    j.write_int("guaranteedFlowBitRateUL", guaranteed_flow_bit_rate_ul);
  }
  if (packet_delay_budget_present) {
    j.write_int("packetDelayBudget", packet_delay_budget);
  }
  if (packet_error_rate_present) {
    j.write_fieldname("packetErrorRate");
    packet_error_rate.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Cell-Group-Information-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t cell_group_info_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {127};
  return map_enum_number(names, 1, idx, "id");
}
bool cell_group_info_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 127 == id;
}
crit_e cell_group_info_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 127) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cell_group_info_item_ext_ies_o::ext_c cell_group_info_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 127) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cell_group_info_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 127) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void cell_group_info_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("INTEGER (1..4,...)", c);
  j.end_obj();
}
SRSASN_CODE cell_group_info_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_integer(bref, c, (uint8_t)1u, (uint8_t)4u, true, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_group_info_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_integer(c, bref, (uint8_t)1u, (uint8_t)4u, true, true));
  return SRSASN_SUCCESS;
}

const char* cell_group_info_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..4,...)"};
  return convert_enum_idx(names, 1, value, "cell_group_info_item_ext_ies_o::ext_c::types");
}
uint8_t cell_group_info_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cell_group_info_item_ext_ies_o::ext_c::types");
}

// DL-TX-Stop ::= ENUMERATED
const char* dl_tx_stop_opts::to_string() const
{
  static const char* names[] = {"stop", "resume"};
  return convert_enum_idx(names, 2, value, "dl_tx_stop_e");
}

// RAT-Type ::= ENUMERATED
const char* rat_type_opts::to_string() const
{
  static const char* names[] = {"e-UTRA", "nR"};
  return convert_enum_idx(names, 2, value, "rat_type_e");
}

// UL-Configuration ::= ENUMERATED
const char* ul_cfg_opts::to_string() const
{
  static const char* names[] = {"no-data", "shared", "only"};
  return convert_enum_idx(names, 3, value, "ul_cfg_e");
}

// Cell-Group-Information-Item ::= SEQUENCE
SRSASN_CODE cell_group_info_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_cfg_present, 1));
  HANDLE_CODE(bref.pack(dl_tx_stop_present, 1));
  HANDLE_CODE(bref.pack(rat_type_present, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, cell_group_id, (uint8_t)0u, (uint8_t)3u, true, true));
  if (ul_cfg_present) {
    HANDLE_CODE(ul_cfg.pack(bref));
  }
  if (dl_tx_stop_present) {
    HANDLE_CODE(dl_tx_stop.pack(bref));
  }
  if (rat_type_present) {
    HANDLE_CODE(rat_type.pack(bref));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_group_info_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_cfg_present, 1));
  HANDLE_CODE(bref.unpack(dl_tx_stop_present, 1));
  HANDLE_CODE(bref.unpack(rat_type_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(cell_group_id, bref, (uint8_t)0u, (uint8_t)3u, true, true));
  if (ul_cfg_present) {
    HANDLE_CODE(ul_cfg.unpack(bref));
  }
  if (dl_tx_stop_present) {
    HANDLE_CODE(dl_tx_stop.unpack(bref));
  }
  if (rat_type_present) {
    HANDLE_CODE(rat_type.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void cell_group_info_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cell-Group-ID", cell_group_id);
  if (ul_cfg_present) {
    j.write_str("uL-Configuration", ul_cfg.to_string());
  }
  if (dl_tx_stop_present) {
    j.write_str("dL-TX-Stop", dl_tx_stop.to_string());
  }
  if (rat_type_present) {
    j.write_str("rAT-Type", rat_type.to_string());
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// DRB-Confirm-Modified-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_confirm_modified_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_group_info.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (cell_group_info.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_info, 1, 4, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_confirm_modified_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool cell_group_info_present;
  HANDLE_CODE(bref.unpack(cell_group_info_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (cell_group_info_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_group_info, bref, 1, 4, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_confirm_modified_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (cell_group_info.size() > 0) {
    j.start_array("cell-Group-Information");
    for (const auto& e1 : cell_group_info) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Confirm-Modified-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_confirm_modified_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_group_info.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (cell_group_info.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_info, 1, 4, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_confirm_modified_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool cell_group_info_present;
  HANDLE_CODE(bref.unpack(cell_group_info_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (cell_group_info_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_group_info, bref, 1, 4, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_confirm_modified_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (cell_group_info.size() > 0) {
    j.start_array("cell-Group-Information");
    for (const auto& e1 : cell_group_info) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-Confirm-Modified-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_confirm_modified_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_confirm_modified_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  if (drb_confirm_modified_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_confirm_modified_list_ng_ran, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_confirm_modified_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool drb_confirm_modified_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_confirm_modified_list_ng_ran_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (drb_confirm_modified_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_confirm_modified_list_ng_ran, bref, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_confirm_modified_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  if (drb_confirm_modified_list_ng_ran.size() > 0) {
    j.start_array("dRB-Confirm-Modified-List-NG-RAN");
    for (const auto& e1 : drb_confirm_modified_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// TypeOfError ::= ENUMERATED
const char* type_of_error_opts::to_string() const
{
  static const char* names[] = {"not-understood", "missing"};
  return convert_enum_idx(names, 2, value, "type_of_error_e");
}

// CauseMisc ::= ENUMERATED
const char* cause_misc_opts::to_string() const
{
  static const char* names[] = {"control-processing-overload",
                                "not-enough-user-plane-processing-resources",
                                "hardware-failure",
                                "om-intervention",
                                "unspecified"};
  return convert_enum_idx(names, 5, value, "cause_misc_e");
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

// CauseRadioNetwork ::= ENUMERATED
const char* cause_radio_network_opts::to_string() const
{
  static const char* names[] = {"unspecified",
                                "unknown-or-already-allocated-gnb-cu-cp-ue-e1ap-id",
                                "unknown-or-already-allocated-gnb-cu-up-ue-e1ap-id",
                                "unknown-or-inconsistent-pair-of-ue-e1ap-id",
                                "interaction-with-other-procedure",
                                "pPDCP-Count-wrap-around",
                                "not-supported-QCI-value",
                                "not-supported-5QI-value",
                                "encryption-algorithms-not-supported",
                                "integrity-protection-algorithms-not-supported",
                                "uP-integrity-protection-not-possible",
                                "uP-confidentiality-protection-not-possible",
                                "multiple-PDU-Session-ID-Instances",
                                "unknown-PDU-Session-ID",
                                "multiple-QoS-Flow-ID-Instances",
                                "unknown-QoS-Flow-ID",
                                "multiple-DRB-ID-Instances",
                                "unknown-DRB-ID",
                                "invalid-QoS-combination",
                                "procedure-cancelled",
                                "normal-release",
                                "no-radio-resources-available",
                                "action-desirable-for-radio-reasons",
                                "resources-not-available-for-the-slice",
                                "pDCP-configuration-not-supported",
                                "ue-dl-max-IP-data-rate-reason",
                                "uP-integrity-protection-failure",
                                "release-due-to-pre-emption",
                                "rsn-not-available-for-the-up",
                                "nPN-not-supported",
                                "report-characteristic-empty",
                                "existing-measurement-ID",
                                "measurement-temporarily-not-available",
                                "measurement-not-supported-for-the-object"};
  return convert_enum_idx(names, 34, value, "cause_radio_network_e");
}

// CauseTransport ::= ENUMERATED
const char* cause_transport_opts::to_string() const
{
  static const char* names[] = {"unspecified", "transport-resource-unavailable", "unknown-TNL-address-for-IAB"};
  return convert_enum_idx(names, 3, value, "cause_transport_e");
}

SRSASN_CODE crit_diagnostics_ie_list_item_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(ie_crit.pack(bref));
  HANDLE_CODE(pack_integer(bref, ie_id, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(type_of_error.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE crit_diagnostics_ie_list_item_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(ie_crit.unpack(bref));
  HANDLE_CODE(unpack_integer(ie_id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(type_of_error.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void crit_diagnostics_ie_list_item_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("iECriticality", ie_crit.to_string());
  j.write_int("iE-ID", ie_id);
  j.write_str("typeOfError", type_of_error.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// TriggeringMessage ::= ENUMERATED
const char* trigger_msg_opts::to_string() const
{
  static const char* names[] = {"initiating-message", "successful-outcome", "unsuccessful-outcome"};
  return convert_enum_idx(names, 3, value, "trigger_msg_e");
}

// Cause ::= CHOICE
void cause_c::destroy_()
{
  switch (type_) {
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<cause_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void cause_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::radio_network:
      break;
    case types::transport:
      break;
    case types::protocol:
      break;
    case types::misc:
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<cause_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
  }
}
cause_c::cause_c(const cause_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::radio_network:
      c.init(other.c.get<cause_radio_network_e>());
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
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<cause_ext_ies_o>>());
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
    case types::radio_network:
      c.set(other.c.get<cause_radio_network_e>());
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
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<cause_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
  }

  return *this;
}
cause_radio_network_e& cause_c::set_radio_network()
{
  set(types::radio_network);
  return c.get<cause_radio_network_e>();
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
protocol_ie_single_container_s<cause_ext_ies_o>& cause_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<cause_ext_ies_o>>();
}
void cause_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::radio_network:
      j.write_str("radioNetwork", c.get<cause_radio_network_e>().to_string());
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
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<cause_ext_ies_o>>().to_json(j);
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
    case types::radio_network:
      HANDLE_CODE(c.get<cause_radio_network_e>().pack(bref));
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
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<cause_ext_ies_o>>().pack(bref));
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
    case types::radio_network:
      HANDLE_CODE(c.get<cause_radio_network_e>().unpack(bref));
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
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<cause_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cause_c::types_opts::to_string() const
{
  static const char* names[] = {"radioNetwork", "transport", "protocol", "misc", "choice-extension"};
  return convert_enum_idx(names, 5, value, "cause_c::types");
}
uint8_t cause_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "cause_c::types");
}

// CriticalityDiagnostics ::= SEQUENCE
SRSASN_CODE crit_diagnostics_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(proc_code_present, 1));
  HANDLE_CODE(bref.pack(trigger_msg_present, 1));
  HANDLE_CODE(bref.pack(proc_crit_present, 1));
  HANDLE_CODE(bref.pack(transaction_id_present, 1));
  HANDLE_CODE(bref.pack(ies_crit_diagnostics.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (proc_code_present) {
    HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (trigger_msg_present) {
    HANDLE_CODE(trigger_msg.pack(bref));
  }
  if (proc_crit_present) {
    HANDLE_CODE(proc_crit.pack(bref));
  }
  if (transaction_id_present) {
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ies_crit_diagnostics.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ies_crit_diagnostics, 1, 256, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE crit_diagnostics_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(proc_code_present, 1));
  HANDLE_CODE(bref.unpack(trigger_msg_present, 1));
  HANDLE_CODE(bref.unpack(proc_crit_present, 1));
  HANDLE_CODE(bref.unpack(transaction_id_present, 1));
  bool ies_crit_diagnostics_present;
  HANDLE_CODE(bref.unpack(ies_crit_diagnostics_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (proc_code_present) {
    HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (trigger_msg_present) {
    HANDLE_CODE(trigger_msg.unpack(bref));
  }
  if (proc_crit_present) {
    HANDLE_CODE(proc_crit.unpack(bref));
  }
  if (transaction_id_present) {
    HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ies_crit_diagnostics_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ies_crit_diagnostics, bref, 1, 256, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (transaction_id_present) {
    j.write_int("transactionID", transaction_id);
  }
  if (ies_crit_diagnostics.size() > 0) {
    j.start_array("iEsCriticalityDiagnostics");
    for (const auto& e1 : ies_crit_diagnostics) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t dyn_5qi_descriptor_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {103, 101, 102};
  return map_enum_number(names, 3, idx, "id");
}
bool dyn_5qi_descriptor_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {103, 101, 102};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e dyn_5qi_descriptor_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 103:
      return crit_e::ignore;
    case 101:
      return crit_e::ignore;
    case 102:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
dyn_5qi_descriptor_ext_ies_o::ext_c dyn_5qi_descriptor_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 103:
      ret.set(ext_c::types::extended_packet_delay_budget);
      break;
    case 101:
      ret.set(ext_c::types::cn_packet_delay_budget_dl);
      break;
    case 102:
      ret.set(ext_c::types::cn_packet_delay_budget_ul);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e dyn_5qi_descriptor_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 103:
      return presence_e::optional;
    case 101:
      return presence_e::optional;
    case 102:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void dyn_5qi_descriptor_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::extended_packet_delay_budget:
      c = uint32_t{};
      break;
    case types::cn_packet_delay_budget_dl:
      c = uint32_t{};
      break;
    case types::cn_packet_delay_budget_ul:
      c = uint32_t{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dyn_5qi_descriptor_ext_ies_o::ext_c");
  }
}
uint32_t& dyn_5qi_descriptor_ext_ies_o::ext_c::extended_packet_delay_budget()
{
  assert_choice_type(types::extended_packet_delay_budget, type_, "Extension");
  return c.get<uint32_t>();
}
uint32_t& dyn_5qi_descriptor_ext_ies_o::ext_c::cn_packet_delay_budget_dl()
{
  assert_choice_type(types::cn_packet_delay_budget_dl, type_, "Extension");
  return c.get<uint32_t>();
}
uint32_t& dyn_5qi_descriptor_ext_ies_o::ext_c::cn_packet_delay_budget_ul()
{
  assert_choice_type(types::cn_packet_delay_budget_ul, type_, "Extension");
  return c.get<uint32_t>();
}
const uint32_t& dyn_5qi_descriptor_ext_ies_o::ext_c::extended_packet_delay_budget() const
{
  assert_choice_type(types::extended_packet_delay_budget, type_, "Extension");
  return c.get<uint32_t>();
}
const uint32_t& dyn_5qi_descriptor_ext_ies_o::ext_c::cn_packet_delay_budget_dl() const
{
  assert_choice_type(types::cn_packet_delay_budget_dl, type_, "Extension");
  return c.get<uint32_t>();
}
const uint32_t& dyn_5qi_descriptor_ext_ies_o::ext_c::cn_packet_delay_budget_ul() const
{
  assert_choice_type(types::cn_packet_delay_budget_ul, type_, "Extension");
  return c.get<uint32_t>();
}
void dyn_5qi_descriptor_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::extended_packet_delay_budget:
      j.write_int("INTEGER (1..65535,...)", c.get<uint32_t>());
      break;
    case types::cn_packet_delay_budget_dl:
      j.write_int("INTEGER (1..65535,...)", c.get<uint32_t>());
      break;
    case types::cn_packet_delay_budget_ul:
      j.write_int("INTEGER (1..65535,...)", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "dyn_5qi_descriptor_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE dyn_5qi_descriptor_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::extended_packet_delay_budget:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    case types::cn_packet_delay_budget_dl:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    case types::cn_packet_delay_budget_ul:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "dyn_5qi_descriptor_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dyn_5qi_descriptor_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::extended_packet_delay_budget:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    case types::cn_packet_delay_budget_dl:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    case types::cn_packet_delay_budget_ul:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "dyn_5qi_descriptor_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dyn_5qi_descriptor_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..65535,...)", "INTEGER (1..65535,...)", "INTEGER (1..65535,...)"};
  return convert_enum_idx(names, 3, value, "dyn_5qi_descriptor_ext_ies_o::ext_c::types");
}

// GBR-QosFlowInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t gbr_qos_flow_info_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {124};
  return map_enum_number(names, 1, idx, "id");
}
bool gbr_qos_flow_info_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 124 == id;
}
crit_e gbr_qos_flow_info_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 124) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gbr_qos_flow_info_ext_ies_o::ext_c gbr_qos_flow_info_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 124) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gbr_qos_flow_info_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 124) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void gbr_qos_flow_info_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("AlternativeQoSParaSetList");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE gbr_qos_flow_info_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 8, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gbr_qos_flow_info_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 8, true));
  return SRSASN_SUCCESS;
}

const char* gbr_qos_flow_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AlternativeQoSParaSetList"};
  return convert_enum_idx(names, 1, value, "gbr_qos_flow_info_ext_ies_o::ext_c::types");
}

// Non-Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t non_dyn_5qi_descriptor_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {101, 102};
  return map_enum_number(names, 2, idx, "id");
}
bool non_dyn_5qi_descriptor_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {101, 102};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e non_dyn_5qi_descriptor_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 101:
      return crit_e::ignore;
    case 102:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
non_dyn_5qi_descriptor_ext_ies_o::ext_c non_dyn_5qi_descriptor_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 101:
      ret.set(ext_c::types::cn_packet_delay_budget_dl);
      break;
    case 102:
      ret.set(ext_c::types::cn_packet_delay_budget_ul);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e non_dyn_5qi_descriptor_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 101:
      return presence_e::optional;
    case 102:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void non_dyn_5qi_descriptor_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::cn_packet_delay_budget_dl:
      c = uint32_t{};
      break;
    case types::cn_packet_delay_budget_ul:
      c = uint32_t{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "non_dyn_5qi_descriptor_ext_ies_o::ext_c");
  }
}
uint32_t& non_dyn_5qi_descriptor_ext_ies_o::ext_c::cn_packet_delay_budget_dl()
{
  assert_choice_type(types::cn_packet_delay_budget_dl, type_, "Extension");
  return c.get<uint32_t>();
}
uint32_t& non_dyn_5qi_descriptor_ext_ies_o::ext_c::cn_packet_delay_budget_ul()
{
  assert_choice_type(types::cn_packet_delay_budget_ul, type_, "Extension");
  return c.get<uint32_t>();
}
const uint32_t& non_dyn_5qi_descriptor_ext_ies_o::ext_c::cn_packet_delay_budget_dl() const
{
  assert_choice_type(types::cn_packet_delay_budget_dl, type_, "Extension");
  return c.get<uint32_t>();
}
const uint32_t& non_dyn_5qi_descriptor_ext_ies_o::ext_c::cn_packet_delay_budget_ul() const
{
  assert_choice_type(types::cn_packet_delay_budget_ul, type_, "Extension");
  return c.get<uint32_t>();
}
void non_dyn_5qi_descriptor_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cn_packet_delay_budget_dl:
      j.write_int("INTEGER (1..65535,...)", c.get<uint32_t>());
      break;
    case types::cn_packet_delay_budget_ul:
      j.write_int("INTEGER (1..65535,...)", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "non_dyn_5qi_descriptor_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE non_dyn_5qi_descriptor_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::cn_packet_delay_budget_dl:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    case types::cn_packet_delay_budget_ul:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "non_dyn_5qi_descriptor_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE non_dyn_5qi_descriptor_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::cn_packet_delay_budget_dl:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    case types::cn_packet_delay_budget_ul:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)1u, (uint32_t)65535u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "non_dyn_5qi_descriptor_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* non_dyn_5qi_descriptor_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..65535,...)", "INTEGER (1..65535,...)"};
  return convert_enum_idx(names, 2, value, "non_dyn_5qi_descriptor_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<dyn_5qi_descriptor_ext_ies_o>;

SRSASN_CODE dyn_5qi_descriptor_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += extended_packet_delay_budget_present ? 1 : 0;
  nof_ies += cn_packet_delay_budget_dl_present ? 1 : 0;
  nof_ies += cn_packet_delay_budget_ul_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (extended_packet_delay_budget_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)103, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, extended_packet_delay_budget, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (cn_packet_delay_budget_dl_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)101, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, cn_packet_delay_budget_dl, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (cn_packet_delay_budget_ul_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)102, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, cn_packet_delay_budget_ul, (uint32_t)1u, (uint32_t)65535u, true, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dyn_5qi_descriptor_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 103: {
        extended_packet_delay_budget_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(extended_packet_delay_budget, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
        break;
      }
      case 101: {
        cn_packet_delay_budget_dl_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(cn_packet_delay_budget_dl, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
        break;
      }
      case 102: {
        cn_packet_delay_budget_ul_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(cn_packet_delay_budget_ul, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void dyn_5qi_descriptor_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (extended_packet_delay_budget_present) {
    j.write_int("id", 103);
    j.write_str("criticality", "ignore");
    j.write_int("Extension", extended_packet_delay_budget);
  }
  if (cn_packet_delay_budget_dl_present) {
    j.write_int("id", 101);
    j.write_str("criticality", "ignore");
    j.write_int("Extension", cn_packet_delay_budget_dl);
  }
  if (cn_packet_delay_budget_ul_present) {
    j.write_int("id", 102);
    j.write_str("criticality", "ignore");
    j.write_int("Extension", cn_packet_delay_budget_ul);
  }
  j.end_obj();
}

// Dynamic5QIDescriptor ::= SEQUENCE
SRSASN_CODE dyn_5qi_descriptor_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(five_qi_present, 1));
  HANDLE_CODE(bref.pack(delay_crit_present, 1));
  HANDLE_CODE(bref.pack(averaging_win_present, 1));
  HANDLE_CODE(bref.pack(max_data_burst_volume_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_prio_level, (uint8_t)0u, (uint8_t)127u, true, true));
  HANDLE_CODE(pack_integer(bref, packet_delay_budget, (uint16_t)0u, (uint16_t)1023u, true, true));
  HANDLE_CODE(packet_error_rate.pack(bref));
  if (five_qi_present) {
    HANDLE_CODE(pack_integer(bref, five_qi, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (delay_crit_present) {
    HANDLE_CODE(delay_crit.pack(bref));
  }
  if (averaging_win_present) {
    HANDLE_CODE(pack_integer(bref, averaging_win, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (max_data_burst_volume_present) {
    HANDLE_CODE(pack_integer(bref, max_data_burst_volume, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dyn_5qi_descriptor_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(five_qi_present, 1));
  HANDLE_CODE(bref.unpack(delay_crit_present, 1));
  HANDLE_CODE(bref.unpack(averaging_win_present, 1));
  HANDLE_CODE(bref.unpack(max_data_burst_volume_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_prio_level, bref, (uint8_t)0u, (uint8_t)127u, true, true));
  HANDLE_CODE(unpack_integer(packet_delay_budget, bref, (uint16_t)0u, (uint16_t)1023u, true, true));
  HANDLE_CODE(packet_error_rate.unpack(bref));
  if (five_qi_present) {
    HANDLE_CODE(unpack_integer(five_qi, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (delay_crit_present) {
    HANDLE_CODE(delay_crit.unpack(bref));
  }
  if (averaging_win_present) {
    HANDLE_CODE(unpack_integer(averaging_win, bref, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (max_data_burst_volume_present) {
    HANDLE_CODE(unpack_integer(max_data_burst_volume, bref, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dyn_5qi_descriptor_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoSPriorityLevel", qos_prio_level);
  j.write_int("packetDelayBudget", packet_delay_budget);
  j.write_fieldname("packetErrorRate");
  packet_error_rate.to_json(j);
  if (five_qi_present) {
    j.write_int("fiveQI", five_qi);
  }
  if (delay_crit_present) {
    j.write_str("delayCritical", delay_crit.to_string());
  }
  if (averaging_win_present) {
    j.write_int("averagingWindow", averaging_win);
  }
  if (max_data_burst_volume_present) {
    j.write_int("maxDataBurstVolume", max_data_burst_volume);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* dyn_5qi_descriptor_s::delay_crit_opts::to_string() const
{
  static const char* names[] = {"delay-critical", "non-delay-critical"};
  return convert_enum_idx(names, 2, value, "dyn_5qi_descriptor_s::delay_crit_e_");
}

// EHC-Downlink-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t ehc_dl_params_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {137};
  return map_enum_number(names, 1, idx, "id");
}
bool ehc_dl_params_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 137 == id;
}
crit_e ehc_dl_params_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 137) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ehc_dl_params_ext_ies_o::ext_c ehc_dl_params_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 137) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ehc_dl_params_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 137) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void ehc_dl_params_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("INTEGER (1..32767,...)", c);
  j.end_obj();
}
SRSASN_CODE ehc_dl_params_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_integer(bref, c, (uint16_t)1u, (uint16_t)32767u, true, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ehc_dl_params_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_integer(c, bref, (uint16_t)1u, (uint16_t)32767u, true, true));
  return SRSASN_SUCCESS;
}

const char* ehc_dl_params_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..32767,...)"};
  return convert_enum_idx(names, 1, value, "ehc_dl_params_ext_ies_o::ext_c::types");
}
uint8_t ehc_dl_params_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ehc_dl_params_ext_ies_o::ext_c::types");
}

// GBR-QoSFlowInformation ::= SEQUENCE
SRSASN_CODE gbr_qos_flow_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(max_packet_loss_rate_dl_present, 1));
  HANDLE_CODE(bref.pack(max_packet_loss_rate_ul_present, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, max_flow_bit_rate_dl, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(pack_integer(bref, max_flow_bit_rate_ul, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(pack_integer(bref, guaranteed_flow_bit_rate_dl, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(pack_integer(bref, guaranteed_flow_bit_rate_ul, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  if (max_packet_loss_rate_dl_present) {
    HANDLE_CODE(pack_integer(bref, max_packet_loss_rate_dl, (uint16_t)0u, (uint16_t)1000u, true, true));
  }
  if (max_packet_loss_rate_ul_present) {
    HANDLE_CODE(pack_integer(bref, max_packet_loss_rate_ul, (uint16_t)0u, (uint16_t)1000u, true, true));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gbr_qos_flow_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(max_packet_loss_rate_dl_present, 1));
  HANDLE_CODE(bref.unpack(max_packet_loss_rate_ul_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(max_flow_bit_rate_dl, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(unpack_integer(max_flow_bit_rate_ul, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(unpack_integer(guaranteed_flow_bit_rate_dl, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(unpack_integer(guaranteed_flow_bit_rate_ul, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  if (max_packet_loss_rate_dl_present) {
    HANDLE_CODE(unpack_integer(max_packet_loss_rate_dl, bref, (uint16_t)0u, (uint16_t)1000u, true, true));
  }
  if (max_packet_loss_rate_ul_present) {
    HANDLE_CODE(unpack_integer(max_packet_loss_rate_ul, bref, (uint16_t)0u, (uint16_t)1000u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void gbr_qos_flow_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxFlowBitRateDownlink", max_flow_bit_rate_dl);
  j.write_int("maxFlowBitRateUplink", max_flow_bit_rate_ul);
  j.write_int("guaranteedFlowBitRateDownlink", guaranteed_flow_bit_rate_dl);
  j.write_int("guaranteedFlowBitRateUplink", guaranteed_flow_bit_rate_ul);
  if (max_packet_loss_rate_dl_present) {
    j.write_int("maxPacketLossRateDownlink", max_packet_loss_rate_dl);
  }
  if (max_packet_loss_rate_ul_present) {
    j.write_int("maxPacketLossRateUplink", max_packet_loss_rate_ul);
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<non_dyn_5qi_descriptor_ext_ies_o>;

SRSASN_CODE non_dyn_5qi_descriptor_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += cn_packet_delay_budget_dl_present ? 1 : 0;
  nof_ies += cn_packet_delay_budget_ul_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (cn_packet_delay_budget_dl_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)101, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, cn_packet_delay_budget_dl, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (cn_packet_delay_budget_ul_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)102, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, cn_packet_delay_budget_ul, (uint32_t)1u, (uint32_t)65535u, true, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE non_dyn_5qi_descriptor_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 101: {
        cn_packet_delay_budget_dl_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(cn_packet_delay_budget_dl, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
        break;
      }
      case 102: {
        cn_packet_delay_budget_ul_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(cn_packet_delay_budget_ul, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void non_dyn_5qi_descriptor_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (cn_packet_delay_budget_dl_present) {
    j.write_int("id", 101);
    j.write_str("criticality", "ignore");
    j.write_int("Extension", cn_packet_delay_budget_dl);
  }
  if (cn_packet_delay_budget_ul_present) {
    j.write_int("id", 102);
    j.write_str("criticality", "ignore");
    j.write_int("Extension", cn_packet_delay_budget_ul);
  }
  j.end_obj();
}

// Non-Dynamic5QIDescriptor ::= SEQUENCE
SRSASN_CODE non_dyn_5qi_descriptor_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(qos_prio_level_present, 1));
  HANDLE_CODE(bref.pack(averaging_win_present, 1));
  HANDLE_CODE(bref.pack(max_data_burst_volume_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, five_qi, (uint16_t)0u, (uint16_t)255u, true, true));
  if (qos_prio_level_present) {
    HANDLE_CODE(pack_integer(bref, qos_prio_level, (uint8_t)0u, (uint8_t)127u, true, true));
  }
  if (averaging_win_present) {
    HANDLE_CODE(pack_integer(bref, averaging_win, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (max_data_burst_volume_present) {
    HANDLE_CODE(pack_integer(bref, max_data_burst_volume, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE non_dyn_5qi_descriptor_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(qos_prio_level_present, 1));
  HANDLE_CODE(bref.unpack(averaging_win_present, 1));
  HANDLE_CODE(bref.unpack(max_data_burst_volume_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(five_qi, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  if (qos_prio_level_present) {
    HANDLE_CODE(unpack_integer(qos_prio_level, bref, (uint8_t)0u, (uint8_t)127u, true, true));
  }
  if (averaging_win_present) {
    HANDLE_CODE(unpack_integer(averaging_win, bref, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (max_data_burst_volume_present) {
    HANDLE_CODE(unpack_integer(max_data_burst_volume, bref, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void non_dyn_5qi_descriptor_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("fiveQI", five_qi);
  if (qos_prio_level_present) {
    j.write_int("qoSPriorityLevel", qos_prio_level);
  }
  if (averaging_win_present) {
    j.write_int("averagingWindow", averaging_win);
  }
  if (max_data_burst_volume_present) {
    j.write_int("maxDataBurstVolume", max_data_burst_volume);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Pre-emptionCapability ::= ENUMERATED
const char* pre_emption_cap_opts::to_string() const
{
  static const char* names[] = {"shall-not-trigger-pre-emption", "may-trigger-pre-emption"};
  return convert_enum_idx(names, 2, value, "pre_emption_cap_e");
}

// Pre-emptionVulnerability ::= ENUMERATED
const char* pre_emption_vulnerability_opts::to_string() const
{
  static const char* names[] = {"not-pre-emptable", "pre-emptable"};
  return convert_enum_idx(names, 2, value, "pre_emption_vulnerability_e");
}

// QoS-Flow-Mapping-Indication ::= ENUMERATED
const char* qos_flow_map_ind_opts::to_string() const
{
  static const char* names[] = {"ul", "dl"};
  return convert_enum_idx(names, 2, value, "qos_flow_map_ind_e");
}

// QosMonitoringDisabled ::= ENUMERATED
const char* qos_monitoring_disabled_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "qos_monitoring_disabled_e");
}

// QosMonitoringRequest ::= ENUMERATED
const char* qos_monitoring_request_opts::to_string() const
{
  static const char* names[] = {"ul", "dl", "both"};
  return convert_enum_idx(names, 3, value, "qos_monitoring_request_e");
}

// TSCTrafficInformation ::= SEQUENCE
SRSASN_CODE tsc_traffic_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(burst_arrival_time.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, periodicity, (uint32_t)1u, (uint32_t)640000u, true, true));
  if (burst_arrival_time.size() > 0) {
    HANDLE_CODE(burst_arrival_time.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tsc_traffic_info_s::unpack(cbit_ref& bref)
{
  bool burst_arrival_time_present;
  HANDLE_CODE(bref.unpack(burst_arrival_time_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(periodicity, bref, (uint32_t)1u, (uint32_t)640000u, true, true));
  if (burst_arrival_time_present) {
    HANDLE_CODE(burst_arrival_time.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void tsc_traffic_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("periodicity", periodicity);
  if (burst_arrival_time.size() > 0) {
    j.write_str("burstArrivalTime", burst_arrival_time.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DataForwardingtoNG-RANQoSFlowInformationList-Item ::= SEQUENCE
SRSASN_CODE data_forwardingto_ng_ran_qos_flow_info_list_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_forwardingto_ng_ran_qos_flow_info_list_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void data_forwardingto_ng_ran_qos_flow_info_list_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qos_flow_id);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// EHC-Common-Parameters ::= SEQUENCE
SRSASN_CODE ehc_common_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(ehc_c_id_len.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ehc_common_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(ehc_c_id_len.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ehc_common_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ehc-CID-Length", ehc_c_id_len.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* ehc_common_params_s::ehc_c_id_len_opts::to_string() const
{
  static const char* names[] = {"bits7", "bits15"};
  return convert_enum_idx(names, 2, value, "ehc_common_params_s::ehc_c_id_len_e_");
}
uint8_t ehc_common_params_s::ehc_c_id_len_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 15};
  return map_enum_number(numbers, 2, value, "ehc_common_params_s::ehc_c_id_len_e_");
}

// EHC-Downlink-Parameters ::= SEQUENCE
SRSASN_CODE ehc_dl_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(drb_continue_ehc_dl.pack(bref));
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ehc_dl_params_s::unpack(cbit_ref& bref)
{
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(drb_continue_ehc_dl.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void ehc_dl_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("drb-ContinueEHC-DL", drb_continue_ehc_dl.to_string());
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

const char* ehc_dl_params_s::drb_continue_ehc_dl_opts::to_string() const
{
  static const char* names[] = {"true", "false"};
  return convert_enum_idx(names, 2, value, "ehc_dl_params_s::drb_continue_ehc_dl_e_");
}

// EHC-Uplink-Parameters ::= SEQUENCE
SRSASN_CODE ehc_ul_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(drb_continue_ehc_ul.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ehc_ul_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(drb_continue_ehc_ul.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ehc_ul_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("drb-ContinueEHC-UL", drb_continue_ehc_ul.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* ehc_ul_params_s::drb_continue_ehc_ul_opts::to_string() const
{
  static const char* names[] = {"true", "false"};
  return convert_enum_idx(names, 2, value, "ehc_ul_params_s::drb_continue_ehc_ul_e_");
}

// GTPTunnel ::= SEQUENCE
SRSASN_CODE gtp_tunnel_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(transport_layer_address.pack(bref));
  HANDLE_CODE(gtp_teid.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gtp_tunnel_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(transport_layer_address.unpack(bref));
  HANDLE_CODE(gtp_teid.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gtp_tunnel_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("transportLayerAddress", transport_layer_address.to_string());
  j.write_str("gTP-TEID", gtp_teid.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// NGRANAllocationAndRetentionPriority ::= SEQUENCE
SRSASN_CODE ngran_alloc_and_retention_prio_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, prio_level, (uint8_t)0u, (uint8_t)15u, false, true));
  HANDLE_CODE(pre_emption_cap.pack(bref));
  HANDLE_CODE(pre_emption_vulnerability.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ngran_alloc_and_retention_prio_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(prio_level, bref, (uint8_t)0u, (uint8_t)15u, false, true));
  HANDLE_CODE(pre_emption_cap.unpack(bref));
  HANDLE_CODE(pre_emption_vulnerability.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ngran_alloc_and_retention_prio_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("priorityLevel", prio_level);
  j.write_str("pre-emptionCapability", pre_emption_cap.to_string());
  j.write_str("pre-emptionVulnerability", pre_emption_vulnerability.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDCP-Count ::= SEQUENCE
SRSASN_CODE pdcp_count_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdcp_sn, (uint32_t)0u, (uint32_t)262143u, false, true));
  HANDLE_CODE(pack_integer(bref, hfn, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_count_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdcp_sn, bref, (uint32_t)0u, (uint32_t)262143u, false, true));
  HANDLE_CODE(unpack_integer(hfn, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcp_count_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDCP-SN", pdcp_sn);
  j.write_int("hFN", hfn);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// QoS-Characteristics ::= CHOICE
void qos_characteristics_c::destroy_()
{
  switch (type_) {
    case types::non_dyn_5qi:
      c.destroy<non_dyn_5qi_descriptor_s>();
      break;
    case types::dyn_5qi:
      c.destroy<dyn_5qi_descriptor_s>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void qos_characteristics_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::non_dyn_5qi:
      c.init<non_dyn_5qi_descriptor_s>();
      break;
    case types::dyn_5qi:
      c.init<dyn_5qi_descriptor_s>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qos_characteristics_c");
  }
}
qos_characteristics_c::qos_characteristics_c(const qos_characteristics_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::non_dyn_5qi:
      c.init(other.c.get<non_dyn_5qi_descriptor_s>());
      break;
    case types::dyn_5qi:
      c.init(other.c.get<dyn_5qi_descriptor_s>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qos_characteristics_c");
  }
}
qos_characteristics_c& qos_characteristics_c::operator=(const qos_characteristics_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::non_dyn_5qi:
      c.set(other.c.get<non_dyn_5qi_descriptor_s>());
      break;
    case types::dyn_5qi:
      c.set(other.c.get<dyn_5qi_descriptor_s>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qos_characteristics_c");
  }

  return *this;
}
non_dyn_5qi_descriptor_s& qos_characteristics_c::set_non_dyn_5qi()
{
  set(types::non_dyn_5qi);
  return c.get<non_dyn_5qi_descriptor_s>();
}
dyn_5qi_descriptor_s& qos_characteristics_c::set_dyn_5qi()
{
  set(types::dyn_5qi);
  return c.get<dyn_5qi_descriptor_s>();
}
protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& qos_characteristics_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
}
void qos_characteristics_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::non_dyn_5qi:
      j.write_fieldname("non-Dynamic-5QI");
      c.get<non_dyn_5qi_descriptor_s>().to_json(j);
      break;
    case types::dyn_5qi:
      j.write_fieldname("dynamic-5QI");
      c.get<dyn_5qi_descriptor_s>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "qos_characteristics_c");
  }
  j.end_obj();
}
SRSASN_CODE qos_characteristics_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::non_dyn_5qi:
      HANDLE_CODE(c.get<non_dyn_5qi_descriptor_s>().pack(bref));
      break;
    case types::dyn_5qi:
      HANDLE_CODE(c.get<dyn_5qi_descriptor_s>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "qos_characteristics_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_characteristics_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::non_dyn_5qi:
      HANDLE_CODE(c.get<non_dyn_5qi_descriptor_s>().unpack(bref));
      break;
    case types::dyn_5qi:
      HANDLE_CODE(c.get<dyn_5qi_descriptor_s>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "qos_characteristics_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* qos_characteristics_c::types_opts::to_string() const
{
  static const char* names[] = {"non-Dynamic-5QI", "dynamic-5QI", "choice-extension"};
  return convert_enum_idx(names, 3, value, "qos_characteristics_c::types");
}

// QoS-Flow-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t qos_flow_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {80, 142};
  return map_enum_number(names, 2, idx, "id");
}
bool qos_flow_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {80, 142};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e qos_flow_item_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 80:
      return crit_e::ignore;
    case 142:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
qos_flow_item_ext_ies_o::ext_c qos_flow_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 80:
      ret.set(ext_c::types::qos_flow_map_ind);
      break;
    case 142:
      ret.set(ext_c::types::data_forwarding_source_ip_address);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e qos_flow_item_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 80:
      return presence_e::optional;
    case 142:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void qos_flow_item_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::qos_flow_map_ind:
      c = qos_flow_map_ind_e{};
      break;
    case types::data_forwarding_source_ip_address:
      c = bounded_bitstring<1, 160, true, true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_item_ext_ies_o::ext_c");
  }
}
qos_flow_map_ind_e& qos_flow_item_ext_ies_o::ext_c::qos_flow_map_ind()
{
  assert_choice_type(types::qos_flow_map_ind, type_, "Extension");
  return c.get<qos_flow_map_ind_e>();
}
bounded_bitstring<1, 160, true, true>& qos_flow_item_ext_ies_o::ext_c::data_forwarding_source_ip_address()
{
  assert_choice_type(types::data_forwarding_source_ip_address, type_, "Extension");
  return c.get<bounded_bitstring<1, 160, true, true>>();
}
const qos_flow_map_ind_e& qos_flow_item_ext_ies_o::ext_c::qos_flow_map_ind() const
{
  assert_choice_type(types::qos_flow_map_ind, type_, "Extension");
  return c.get<qos_flow_map_ind_e>();
}
const bounded_bitstring<1, 160, true, true>& qos_flow_item_ext_ies_o::ext_c::data_forwarding_source_ip_address() const
{
  assert_choice_type(types::data_forwarding_source_ip_address, type_, "Extension");
  return c.get<bounded_bitstring<1, 160, true, true>>();
}
void qos_flow_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::qos_flow_map_ind:
      j.write_str("QoS-Flow-Mapping-Indication", c.get<qos_flow_map_ind_e>().to_string());
      break;
    case types::data_forwarding_source_ip_address:
      j.write_str("BIT STRING", c.get<bounded_bitstring<1, 160, true, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_item_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE qos_flow_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::qos_flow_map_ind:
      HANDLE_CODE(c.get<qos_flow_map_ind_e>().pack(bref));
      break;
    case types::data_forwarding_source_ip_address:
      HANDLE_CODE((c.get<bounded_bitstring<1, 160, true, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::qos_flow_map_ind:
      HANDLE_CODE(c.get<qos_flow_map_ind_e>().unpack(bref));
      break;
    case types::data_forwarding_source_ip_address:
      HANDLE_CODE((c.get<bounded_bitstring<1, 160, true, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* qos_flow_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QoS-Flow-Mapping-Indication", "BIT STRING"};
  return convert_enum_idx(names, 2, value, "qos_flow_item_ext_ies_o::ext_c::types");
}

// QoS-Mapping-Information ::= SEQUENCE
SRSASN_CODE qos_map_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dscp_present, 1));
  HANDLE_CODE(bref.pack(flow_label_present, 1));

  if (dscp_present) {
    HANDLE_CODE(dscp.pack(bref));
  }
  if (flow_label_present) {
    HANDLE_CODE(flow_label.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_map_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dscp_present, 1));
  HANDLE_CODE(bref.unpack(flow_label_present, 1));

  if (dscp_present) {
    HANDLE_CODE(dscp.unpack(bref));
  }
  if (flow_label_present) {
    HANDLE_CODE(flow_label.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_map_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dscp_present) {
    j.write_str("dscp", dscp.to_string());
  }
  if (flow_label_present) {
    j.write_str("flow-label", flow_label.to_string());
  }
  j.end_obj();
}

// QoSFlowLevelQoSParameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t qos_flow_level_qos_params_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {87, 126, 132, 133, 142};
  return map_enum_number(names, 5, idx, "id");
}
bool qos_flow_level_qos_params_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {87, 126, 132, 133, 142};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e qos_flow_level_qos_params_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 87:
      return crit_e::ignore;
    case 126:
      return crit_e::ignore;
    case 132:
      return crit_e::ignore;
    case 133:
      return crit_e::ignore;
    case 142:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
qos_flow_level_qos_params_ext_ies_o::ext_c qos_flow_level_qos_params_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 87:
      ret.set(ext_c::types::qos_monitoring_request);
      break;
    case 126:
      ret.set(ext_c::types::mcg_offered_gbr_qos_flow_info);
      break;
    case 132:
      ret.set(ext_c::types::qos_monitoring_report_freq);
      break;
    case 133:
      ret.set(ext_c::types::qos_monitoring_disabled);
      break;
    case 142:
      ret.set(ext_c::types::data_forwarding_source_ip_address);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e qos_flow_level_qos_params_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 87:
      return presence_e::optional;
    case 126:
      return presence_e::optional;
    case 132:
      return presence_e::optional;
    case 133:
      return presence_e::optional;
    case 142:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void qos_flow_level_qos_params_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::qos_monitoring_request:
      c = qos_monitoring_request_e{};
      break;
    case types::mcg_offered_gbr_qos_flow_info:
      c = gbr_qos_flow_info_s{};
      break;
    case types::qos_monitoring_report_freq:
      c = uint16_t{};
      break;
    case types::qos_monitoring_disabled:
      c = qos_monitoring_disabled_e{};
      break;
    case types::data_forwarding_source_ip_address:
      c = bounded_bitstring<1, 160, true, true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_level_qos_params_ext_ies_o::ext_c");
  }
}
qos_monitoring_request_e& qos_flow_level_qos_params_ext_ies_o::ext_c::qos_monitoring_request()
{
  assert_choice_type(types::qos_monitoring_request, type_, "Extension");
  return c.get<qos_monitoring_request_e>();
}
gbr_qos_flow_info_s& qos_flow_level_qos_params_ext_ies_o::ext_c::mcg_offered_gbr_qos_flow_info()
{
  assert_choice_type(types::mcg_offered_gbr_qos_flow_info, type_, "Extension");
  return c.get<gbr_qos_flow_info_s>();
}
uint16_t& qos_flow_level_qos_params_ext_ies_o::ext_c::qos_monitoring_report_freq()
{
  assert_choice_type(types::qos_monitoring_report_freq, type_, "Extension");
  return c.get<uint16_t>();
}
qos_monitoring_disabled_e& qos_flow_level_qos_params_ext_ies_o::ext_c::qos_monitoring_disabled()
{
  assert_choice_type(types::qos_monitoring_disabled, type_, "Extension");
  return c.get<qos_monitoring_disabled_e>();
}
bounded_bitstring<1, 160, true, true>& qos_flow_level_qos_params_ext_ies_o::ext_c::data_forwarding_source_ip_address()
{
  assert_choice_type(types::data_forwarding_source_ip_address, type_, "Extension");
  return c.get<bounded_bitstring<1, 160, true, true>>();
}
const qos_monitoring_request_e& qos_flow_level_qos_params_ext_ies_o::ext_c::qos_monitoring_request() const
{
  assert_choice_type(types::qos_monitoring_request, type_, "Extension");
  return c.get<qos_monitoring_request_e>();
}
const gbr_qos_flow_info_s& qos_flow_level_qos_params_ext_ies_o::ext_c::mcg_offered_gbr_qos_flow_info() const
{
  assert_choice_type(types::mcg_offered_gbr_qos_flow_info, type_, "Extension");
  return c.get<gbr_qos_flow_info_s>();
}
const uint16_t& qos_flow_level_qos_params_ext_ies_o::ext_c::qos_monitoring_report_freq() const
{
  assert_choice_type(types::qos_monitoring_report_freq, type_, "Extension");
  return c.get<uint16_t>();
}
const qos_monitoring_disabled_e& qos_flow_level_qos_params_ext_ies_o::ext_c::qos_monitoring_disabled() const
{
  assert_choice_type(types::qos_monitoring_disabled, type_, "Extension");
  return c.get<qos_monitoring_disabled_e>();
}
const bounded_bitstring<1, 160, true, true>&
qos_flow_level_qos_params_ext_ies_o::ext_c::data_forwarding_source_ip_address() const
{
  assert_choice_type(types::data_forwarding_source_ip_address, type_, "Extension");
  return c.get<bounded_bitstring<1, 160, true, true>>();
}
void qos_flow_level_qos_params_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::qos_monitoring_request:
      j.write_str("QosMonitoringRequest", c.get<qos_monitoring_request_e>().to_string());
      break;
    case types::mcg_offered_gbr_qos_flow_info:
      j.write_fieldname("GBR-QoSFlowInformation");
      c.get<gbr_qos_flow_info_s>().to_json(j);
      break;
    case types::qos_monitoring_report_freq:
      j.write_int("INTEGER (1..1800,...)", c.get<uint16_t>());
      break;
    case types::qos_monitoring_disabled:
      j.write_str("QosMonitoringDisabled", "true");
      break;
    case types::data_forwarding_source_ip_address:
      j.write_str("BIT STRING", c.get<bounded_bitstring<1, 160, true, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_level_qos_params_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE qos_flow_level_qos_params_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::qos_monitoring_request:
      HANDLE_CODE(c.get<qos_monitoring_request_e>().pack(bref));
      break;
    case types::mcg_offered_gbr_qos_flow_info:
      HANDLE_CODE(c.get<gbr_qos_flow_info_s>().pack(bref));
      break;
    case types::qos_monitoring_report_freq:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)1800u, true, true));
      break;
    case types::qos_monitoring_disabled:
      HANDLE_CODE(c.get<qos_monitoring_disabled_e>().pack(bref));
      break;
    case types::data_forwarding_source_ip_address:
      HANDLE_CODE((c.get<bounded_bitstring<1, 160, true, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_level_qos_params_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_level_qos_params_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::qos_monitoring_request:
      HANDLE_CODE(c.get<qos_monitoring_request_e>().unpack(bref));
      break;
    case types::mcg_offered_gbr_qos_flow_info:
      HANDLE_CODE(c.get<gbr_qos_flow_info_s>().unpack(bref));
      break;
    case types::qos_monitoring_report_freq:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)1800u, true, true));
      break;
    case types::qos_monitoring_disabled:
      HANDLE_CODE(c.get<qos_monitoring_disabled_e>().unpack(bref));
      break;
    case types::data_forwarding_source_ip_address:
      HANDLE_CODE((c.get<bounded_bitstring<1, 160, true, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_level_qos_params_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* qos_flow_level_qos_params_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {
      "QosMonitoringRequest", "GBR-QoSFlowInformation", "INTEGER (1..1800,...)", "QosMonitoringDisabled", "BIT STRING"};
  return convert_enum_idx(names, 5, value, "qos_flow_level_qos_params_ext_ies_o::ext_c::types");
}
uint8_t qos_flow_level_qos_params_ext_ies_o::ext_c::types_opts::to_number() const
{
  if (value == qos_monitoring_report_freq) {
    return 1;
  }
  invalid_enum_number(value, "qos_flow_level_qos_params_ext_ies_o::ext_c::types");
  return 0;
}

// RedundantQoSFlowIndicator ::= ENUMERATED
const char* redundant_qos_flow_ind_opts::to_string() const
{
  static const char* names[] = {"true", "false"};
  return convert_enum_idx(names, 2, value, "redundant_qos_flow_ind_e");
}

// TSCTrafficCharacteristics ::= SEQUENCE
SRSASN_CODE tsc_traffic_characteristics_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tsc_traffic_characteristics_ul_present, 1));
  HANDLE_CODE(bref.pack(tsc_traffic_characteristics_dl_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (tsc_traffic_characteristics_ul_present) {
    HANDLE_CODE(tsc_traffic_characteristics_ul.pack(bref));
  }
  if (tsc_traffic_characteristics_dl_present) {
    HANDLE_CODE(tsc_traffic_characteristics_dl.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tsc_traffic_characteristics_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tsc_traffic_characteristics_ul_present, 1));
  HANDLE_CODE(bref.unpack(tsc_traffic_characteristics_dl_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (tsc_traffic_characteristics_ul_present) {
    HANDLE_CODE(tsc_traffic_characteristics_ul.unpack(bref));
  }
  if (tsc_traffic_characteristics_dl_present) {
    HANDLE_CODE(tsc_traffic_characteristics_dl.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void tsc_traffic_characteristics_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tsc_traffic_characteristics_ul_present) {
    j.write_fieldname("tSCTrafficCharacteristicsUL");
    tsc_traffic_characteristics_ul.to_json(j);
  }
  if (tsc_traffic_characteristics_dl_present) {
    j.write_fieldname("tSCTrafficCharacteristicsDL");
    tsc_traffic_characteristics_dl.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// AdditionalPDCPduplicationInformation ::= ENUMERATED
const char* add_pdcp_dupl_info_opts::to_string() const
{
  static const char* names[] = {"three", "four"};
  return convert_enum_idx(names, 2, value, "add_pdcp_dupl_info_e");
}
uint8_t add_pdcp_dupl_info_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4};
  return map_enum_number(numbers, 2, value, "add_pdcp_dupl_info_e");
}

// DLDiscarding ::= SEQUENCE
SRSASN_CODE dl_discarding_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(dl_discarding_count_val.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_discarding_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(dl_discarding_count_val.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_discarding_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("dLDiscardingCountVal");
  dl_discarding_count_val.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DiscardTimerExtended ::= ENUMERATED
const char* discard_timer_extended_opts::to_string() const
{
  static const char* names[] = {"ms0dot5", "ms1", "ms2", "ms4", "ms6", "ms8"};
  return convert_enum_idx(names, 6, value, "discard_timer_extended_e");
}
float discard_timer_extended_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 4.0, 6.0, 8.0};
  return map_enum_number(numbers, 6, value, "discard_timer_extended_e");
}
const char* discard_timer_extended_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "4", "6", "8"};
  return convert_enum_idx(number_strs, 6, value, "discard_timer_extended_e");
}

// EHC-Parameters ::= SEQUENCE
SRSASN_CODE ehc_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ehc_dl_present, 1));
  HANDLE_CODE(bref.pack(ehc_ul_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(ehc_common.pack(bref));
  if (ehc_dl_present) {
    HANDLE_CODE(ehc_dl.pack(bref));
  }
  if (ehc_ul_present) {
    HANDLE_CODE(ehc_ul.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ehc_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ehc_dl_present, 1));
  HANDLE_CODE(bref.unpack(ehc_ul_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(ehc_common.unpack(bref));
  if (ehc_dl_present) {
    HANDLE_CODE(ehc_dl.unpack(bref));
  }
  if (ehc_ul_present) {
    HANDLE_CODE(ehc_ul.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ehc_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ehc-Common");
  ehc_common.to_json(j);
  if (ehc_dl_present) {
    j.write_fieldname("ehc-Downlink");
    ehc_dl.to_json(j);
  }
  if (ehc_ul_present) {
    j.write_fieldname("ehc-Uplink");
    ehc_ul.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// FirstDLCount ::= SEQUENCE
SRSASN_CODE first_dl_count_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(first_dl_count_val.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE first_dl_count_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(first_dl_count_val.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void first_dl_count_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("firstDLCountVal");
  first_dl_count_val.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDCP-StatusReportIndication ::= ENUMERATED
const char* pdcp_status_report_ind_opts::to_string() const
{
  static const char* names[] = {"downlink", "uplink", "both"};
  return convert_enum_idx(names, 3, value, "pdcp_status_report_ind_e");
}

template struct asn1::protocol_ext_field_s<qos_flow_item_ext_ies_o>;

SRSASN_CODE qos_flow_item_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += qos_flow_map_ind_present ? 1 : 0;
  nof_ies += data_forwarding_source_ip_address_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (qos_flow_map_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)80, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(qos_flow_map_ind.pack(bref));
  }
  if (data_forwarding_source_ip_address_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)142, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(data_forwarding_source_ip_address.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_item_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 80: {
        qos_flow_map_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(qos_flow_map_ind.unpack(bref));
        break;
      }
      case 142: {
        data_forwarding_source_ip_address_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(data_forwarding_source_ip_address.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void qos_flow_item_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (qos_flow_map_ind_present) {
    j.write_int("id", 80);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", qos_flow_map_ind.to_string());
  }
  if (data_forwarding_source_ip_address_present) {
    j.write_int("id", 142);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", data_forwarding_source_ip_address.to_string());
  }
  j.end_obj();
}

// QoS-Flow-Item ::= SEQUENCE
SRSASN_CODE qos_flow_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_flow_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qos_flow_id);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// QoS-Flow-Mapping-Item ::= SEQUENCE
SRSASN_CODE qos_flow_map_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qos_flow_map_ind_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  if (qos_flow_map_ind_present) {
    HANDLE_CODE(qos_flow_map_ind.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_map_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(qos_flow_map_ind_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  if (qos_flow_map_ind_present) {
    HANDLE_CODE(qos_flow_map_ind.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_flow_map_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qos_flow_id);
  if (qos_flow_map_ind_present) {
    j.write_str("qoSFlowMappingIndication", qos_flow_map_ind.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// QoS-Flow-QoS-Parameter-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t qos_flow_qos_param_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {99, 100};
  return map_enum_number(names, 2, idx, "id");
}
bool qos_flow_qos_param_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {99, 100};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e qos_flow_qos_param_item_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 99:
      return crit_e::ignore;
    case 100:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
qos_flow_qos_param_item_ext_ies_o::ext_c qos_flow_qos_param_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 99:
      ret.set(ext_c::types::redundant_qos_flow_ind);
      break;
    case 100:
      ret.set(ext_c::types::tsc_traffic_characteristics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e qos_flow_qos_param_item_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 99:
      return presence_e::optional;
    case 100:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void qos_flow_qos_param_item_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::redundant_qos_flow_ind:
      c = redundant_qos_flow_ind_e{};
      break;
    case types::tsc_traffic_characteristics:
      c = tsc_traffic_characteristics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_qos_param_item_ext_ies_o::ext_c");
  }
}
redundant_qos_flow_ind_e& qos_flow_qos_param_item_ext_ies_o::ext_c::redundant_qos_flow_ind()
{
  assert_choice_type(types::redundant_qos_flow_ind, type_, "Extension");
  return c.get<redundant_qos_flow_ind_e>();
}
tsc_traffic_characteristics_s& qos_flow_qos_param_item_ext_ies_o::ext_c::tsc_traffic_characteristics()
{
  assert_choice_type(types::tsc_traffic_characteristics, type_, "Extension");
  return c.get<tsc_traffic_characteristics_s>();
}
const redundant_qos_flow_ind_e& qos_flow_qos_param_item_ext_ies_o::ext_c::redundant_qos_flow_ind() const
{
  assert_choice_type(types::redundant_qos_flow_ind, type_, "Extension");
  return c.get<redundant_qos_flow_ind_e>();
}
const tsc_traffic_characteristics_s& qos_flow_qos_param_item_ext_ies_o::ext_c::tsc_traffic_characteristics() const
{
  assert_choice_type(types::tsc_traffic_characteristics, type_, "Extension");
  return c.get<tsc_traffic_characteristics_s>();
}
void qos_flow_qos_param_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::redundant_qos_flow_ind:
      j.write_str("RedundantQoSFlowIndicator", c.get<redundant_qos_flow_ind_e>().to_string());
      break;
    case types::tsc_traffic_characteristics:
      j.write_fieldname("TSCTrafficCharacteristics");
      c.get<tsc_traffic_characteristics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_qos_param_item_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE qos_flow_qos_param_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::redundant_qos_flow_ind:
      HANDLE_CODE(c.get<redundant_qos_flow_ind_e>().pack(bref));
      break;
    case types::tsc_traffic_characteristics:
      HANDLE_CODE(c.get<tsc_traffic_characteristics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_qos_param_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_qos_param_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::redundant_qos_flow_ind:
      HANDLE_CODE(c.get<redundant_qos_flow_ind_e>().unpack(bref));
      break;
    case types::tsc_traffic_characteristics:
      HANDLE_CODE(c.get<tsc_traffic_characteristics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "qos_flow_qos_param_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* qos_flow_qos_param_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"RedundantQoSFlowIndicator", "TSCTrafficCharacteristics"};
  return convert_enum_idx(names, 2, value, "qos_flow_qos_param_item_ext_ies_o::ext_c::types");
}

// QoS-Flows-to-be-forwarded-Item ::= SEQUENCE
SRSASN_CODE qos_flows_to_be_forwarded_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flows_to_be_forwarded_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_flows_to_be_forwarded_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qos_flow_id);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<qos_flow_level_qos_params_ext_ies_o>;

SRSASN_CODE qos_flow_level_qos_params_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += qos_monitoring_request_present ? 1 : 0;
  nof_ies += mcg_offered_gbr_qos_flow_info_present ? 1 : 0;
  nof_ies += qos_monitoring_report_freq_present ? 1 : 0;
  nof_ies += qos_monitoring_disabled_present ? 1 : 0;
  nof_ies += data_forwarding_source_ip_address_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (qos_monitoring_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)87, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(qos_monitoring_request.pack(bref));
  }
  if (mcg_offered_gbr_qos_flow_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)126, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(mcg_offered_gbr_qos_flow_info.pack(bref));
  }
  if (qos_monitoring_report_freq_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)132, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, qos_monitoring_report_freq, (uint16_t)1u, (uint16_t)1800u, true, true));
  }
  if (qos_monitoring_disabled_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)133, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(qos_monitoring_disabled.pack(bref));
  }
  if (data_forwarding_source_ip_address_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)142, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(data_forwarding_source_ip_address.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_level_qos_params_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 87: {
        qos_monitoring_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(qos_monitoring_request.unpack(bref));
        break;
      }
      case 126: {
        mcg_offered_gbr_qos_flow_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(mcg_offered_gbr_qos_flow_info.unpack(bref));
        break;
      }
      case 132: {
        qos_monitoring_report_freq_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(qos_monitoring_report_freq, bref, (uint16_t)1u, (uint16_t)1800u, true, true));
        break;
      }
      case 133: {
        qos_monitoring_disabled_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(qos_monitoring_disabled.unpack(bref));
        break;
      }
      case 142: {
        data_forwarding_source_ip_address_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(data_forwarding_source_ip_address.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void qos_flow_level_qos_params_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (qos_monitoring_request_present) {
    j.write_int("id", 87);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", qos_monitoring_request.to_string());
  }
  if (mcg_offered_gbr_qos_flow_info_present) {
    j.write_int("id", 126);
    j.write_str("criticality", "ignore");
    mcg_offered_gbr_qos_flow_info.to_json(j);
  }
  if (qos_monitoring_report_freq_present) {
    j.write_int("id", 132);
    j.write_str("criticality", "ignore");
    j.write_int("Extension", qos_monitoring_report_freq);
  }
  if (qos_monitoring_disabled_present) {
    j.write_int("id", 133);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", "true");
  }
  if (data_forwarding_source_ip_address_present) {
    j.write_int("id", 142);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", data_forwarding_source_ip_address.to_string());
  }
  j.end_obj();
}

// QoSFlowLevelQoSParameters ::= SEQUENCE
SRSASN_CODE qos_flow_level_qos_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gbr_qos_flow_info_present, 1));
  HANDLE_CODE(bref.pack(reflective_qos_attribute_present, 1));
  HANDLE_CODE(bref.pack(add_qos_info_present, 1));
  HANDLE_CODE(bref.pack(paging_policy_ind_present, 1));
  HANDLE_CODE(bref.pack(reflective_qos_ind_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(qos_characteristics.pack(bref));
  HANDLE_CODE(ngra_nalloc_retention_prio.pack(bref));
  if (gbr_qos_flow_info_present) {
    HANDLE_CODE(gbr_qos_flow_info.pack(bref));
  }
  if (reflective_qos_attribute_present) {
    HANDLE_CODE(reflective_qos_attribute.pack(bref));
  }
  if (add_qos_info_present) {
    HANDLE_CODE(add_qos_info.pack(bref));
  }
  if (paging_policy_ind_present) {
    HANDLE_CODE(pack_integer(bref, paging_policy_ind, (uint8_t)1u, (uint8_t)8u, true, true));
  }
  if (reflective_qos_ind_present) {
    HANDLE_CODE(reflective_qos_ind.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_level_qos_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gbr_qos_flow_info_present, 1));
  HANDLE_CODE(bref.unpack(reflective_qos_attribute_present, 1));
  HANDLE_CODE(bref.unpack(add_qos_info_present, 1));
  HANDLE_CODE(bref.unpack(paging_policy_ind_present, 1));
  HANDLE_CODE(bref.unpack(reflective_qos_ind_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(qos_characteristics.unpack(bref));
  HANDLE_CODE(ngra_nalloc_retention_prio.unpack(bref));
  if (gbr_qos_flow_info_present) {
    HANDLE_CODE(gbr_qos_flow_info.unpack(bref));
  }
  if (reflective_qos_attribute_present) {
    HANDLE_CODE(reflective_qos_attribute.unpack(bref));
  }
  if (add_qos_info_present) {
    HANDLE_CODE(add_qos_info.unpack(bref));
  }
  if (paging_policy_ind_present) {
    HANDLE_CODE(unpack_integer(paging_policy_ind, bref, (uint8_t)1u, (uint8_t)8u, true, true));
  }
  if (reflective_qos_ind_present) {
    HANDLE_CODE(reflective_qos_ind.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_flow_level_qos_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("qoS-Characteristics");
  qos_characteristics.to_json(j);
  j.write_fieldname("nGRANallocationRetentionPriority");
  ngra_nalloc_retention_prio.to_json(j);
  if (gbr_qos_flow_info_present) {
    j.write_fieldname("gBR-QoS-Flow-Information");
    gbr_qos_flow_info.to_json(j);
  }
  if (reflective_qos_attribute_present) {
    j.write_str("reflective-QoS-Attribute", "subject-to");
  }
  if (add_qos_info_present) {
    j.write_str("additional-QoS-Information", "more-likely");
  }
  if (paging_policy_ind_present) {
    j.write_int("paging-Policy-Indicator", paging_policy_ind);
  }
  if (reflective_qos_ind_present) {
    j.write_str("reflective-QoS-Indicator", "enabled");
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* qos_flow_level_qos_params_s::reflective_qos_attribute_opts::to_string() const
{
  static const char* names[] = {"subject-to"};
  return convert_enum_idx(names, 1, value, "qos_flow_level_qos_params_s::reflective_qos_attribute_e_");
}

const char* qos_flow_level_qos_params_s::add_qos_info_opts::to_string() const
{
  static const char* names[] = {"more-likely"};
  return convert_enum_idx(names, 1, value, "qos_flow_level_qos_params_s::add_qos_info_e_");
}

const char* qos_flow_level_qos_params_s::reflective_qos_ind_opts::to_string() const
{
  static const char* names[] = {"enabled"};
  return convert_enum_idx(names, 1, value, "qos_flow_level_qos_params_s::reflective_qos_ind_e_");
}

// ROHC ::= SEQUENCE
SRSASN_CODE rohc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(continue_rohc_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, max_c_id, (uint16_t)0u, (uint16_t)16383u, true, true));
  HANDLE_CODE(pack_integer(bref, rohc_profiles, (uint16_t)0u, (uint16_t)511u, true, true));
  if (continue_rohc_present) {
    HANDLE_CODE(continue_rohc.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rohc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(continue_rohc_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(max_c_id, bref, (uint16_t)0u, (uint16_t)16383u, true, true));
  HANDLE_CODE(unpack_integer(rohc_profiles, bref, (uint16_t)0u, (uint16_t)511u, true, true));
  if (continue_rohc_present) {
    HANDLE_CODE(continue_rohc.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rohc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxCID", max_c_id);
  j.write_int("rOHC-Profiles", rohc_profiles);
  if (continue_rohc_present) {
    j.write_str("continueROHC", "true");
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* rohc_s::continue_rohc_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "rohc_s::continue_rohc_e_");
}

// T-Reordering ::= ENUMERATED
const char* t_reordering_opts::to_string() const
{
  static const char* names[] = {"ms0",    "ms1",    "ms2",    "ms4",    "ms5",    "ms8",    "ms10",   "ms15",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms120",
                                "ms140",  "ms160",  "ms180",  "ms200",  "ms220",  "ms240",  "ms260",  "ms280",
                                "ms300",  "ms500",  "ms750",  "ms1000", "ms1250", "ms1500", "ms1750", "ms2000",
                                "ms2250", "ms2500", "ms2750", "ms3000"};
  return convert_enum_idx(names, 36, value, "t_reordering_e");
}
uint16_t t_reordering_opts::to_number() const
{
  static const uint16_t numbers[] = {0,   1,   2,   4,    5,    8,    10,   15,   20,   30,   40,   50,
                                     60,  80,  100, 120,  140,  160,  180,  200,  220,  240,  260,  280,
                                     300, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000};
  return map_enum_number(numbers, 36, value, "t_reordering_e");
}

// UP-Parameters-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t up_params_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {107};
  return map_enum_number(names, 1, idx, "id");
}
bool up_params_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 107 == id;
}
crit_e up_params_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 107) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
up_params_item_ext_ies_o::ext_c up_params_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 107) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e up_params_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 107) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void up_params_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("QoS-Mapping-Information");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE up_params_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE up_params_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* up_params_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QoS-Mapping-Information"};
  return convert_enum_idx(names, 1, value, "up_params_item_ext_ies_o::ext_c::types");
}

// UP-TNL-Information ::= CHOICE
void up_tnl_info_c::destroy_()
{
  switch (type_) {
    case types::gtp_tunnel:
      c.destroy<gtp_tunnel_s>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void up_tnl_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gtp_tunnel:
      c.init<gtp_tunnel_s>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "up_tnl_info_c");
  }
}
up_tnl_info_c::up_tnl_info_c(const up_tnl_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gtp_tunnel:
      c.init(other.c.get<gtp_tunnel_s>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "up_tnl_info_c");
  }
}
up_tnl_info_c& up_tnl_info_c::operator=(const up_tnl_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gtp_tunnel:
      c.set(other.c.get<gtp_tunnel_s>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "up_tnl_info_c");
  }

  return *this;
}
gtp_tunnel_s& up_tnl_info_c::set_gtp_tunnel()
{
  set(types::gtp_tunnel);
  return c.get<gtp_tunnel_s>();
}
protocol_ie_single_container_s<up_tnl_info_ext_ies_o>& up_tnl_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>();
}
void up_tnl_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gtp_tunnel:
      j.write_fieldname("gTPTunnel");
      c.get<gtp_tunnel_s>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "up_tnl_info_c");
  }
  j.end_obj();
}
SRSASN_CODE up_tnl_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gtp_tunnel:
      HANDLE_CODE(c.get<gtp_tunnel_s>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "up_tnl_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE up_tnl_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gtp_tunnel:
      HANDLE_CODE(c.get<gtp_tunnel_s>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "up_tnl_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* up_tnl_info_c::types_opts::to_string() const
{
  static const char* names[] = {"gTPTunnel", "choice-extension"};
  return convert_enum_idx(names, 2, value, "up_tnl_info_c::types");
}

// UplinkOnlyROHC ::= SEQUENCE
SRSASN_CODE ul_only_rohc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(continue_rohc_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, max_c_id, (uint16_t)0u, (uint16_t)16383u, true, true));
  HANDLE_CODE(pack_integer(bref, rohc_profiles, (uint16_t)0u, (uint16_t)511u, true, true));
  if (continue_rohc_present) {
    HANDLE_CODE(continue_rohc.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_only_rohc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(continue_rohc_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(max_c_id, bref, (uint16_t)0u, (uint16_t)16383u, true, true));
  HANDLE_CODE(unpack_integer(rohc_profiles, bref, (uint16_t)0u, (uint16_t)511u, true, true));
  if (continue_rohc_present) {
    HANDLE_CODE(continue_rohc.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_only_rohc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxCID", max_c_id);
  j.write_int("rOHC-Profiles", rohc_profiles);
  if (continue_rohc_present) {
    j.write_str("continueROHC", "true");
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* ul_only_rohc_s::continue_rohc_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "ul_only_rohc_s::continue_rohc_e_");
}

// DAPSRequestInfo ::= SEQUENCE
SRSASN_CODE daps_request_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(daps_ind.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE daps_request_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(daps_ind.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void daps_request_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dapsIndicator", "daps-HO-required");
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* daps_request_info_s::daps_ind_opts::to_string() const
{
  static const char* names[] = {"daps-HO-required"};
  return convert_enum_idx(names, 1, value, "daps_request_info_s::daps_ind_e_");
}

// DRBBStatusTransfer ::= SEQUENCE
SRSASN_CODE drb_b_status_transfer_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(receive_statusof_pdcp_sdu_present, 1));
  HANDLE_CODE(bref.pack(ie_ext_present, 1));

  if (receive_statusof_pdcp_sdu_present) {
    HANDLE_CODE(receive_statusof_pdcp_sdu.pack(bref));
  }
  HANDLE_CODE(count_value.pack(bref));
  if (ie_ext_present) {
    HANDLE_CODE(ie_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_b_status_transfer_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(receive_statusof_pdcp_sdu_present, 1));
  HANDLE_CODE(bref.unpack(ie_ext_present, 1));

  if (receive_statusof_pdcp_sdu_present) {
    HANDLE_CODE(receive_statusof_pdcp_sdu.unpack(bref));
  }
  HANDLE_CODE(count_value.unpack(bref));
  if (ie_ext_present) {
    HANDLE_CODE(ie_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_b_status_transfer_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (receive_statusof_pdcp_sdu_present) {
    j.write_str("receiveStatusofPDCPSDU", receive_statusof_pdcp_sdu.to_string());
  }
  j.write_fieldname("countValue");
  count_value.to_json(j);
  if (ie_ext_present) {
    j.write_fieldname("iE-Extension");
    ie_ext.to_json(j);
  }
  j.end_obj();
}

// Data-Forwarding-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t data_forwarding_info_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {136};
  return map_enum_number(names, 1, idx, "id");
}
bool data_forwarding_info_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 136 == id;
}
crit_e data_forwarding_info_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 136) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
data_forwarding_info_ext_ies_o::ext_c data_forwarding_info_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 136) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e data_forwarding_info_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 136) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void data_forwarding_info_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("DataForwardingtoNG-RANQoSFlowInformationList");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE data_forwarding_info_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 64, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE data_forwarding_info_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 64, true));
  return SRSASN_SUCCESS;
}

const char* data_forwarding_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"DataForwardingtoNG-RANQoSFlowInformationList"};
  return convert_enum_idx(names, 1, value, "data_forwarding_info_ext_ies_o::ext_c::types");
}

// Data-Forwarding-Request ::= ENUMERATED
const char* data_forwarding_request_opts::to_string() const
{
  static const char* names[] = {"uL", "dL", "both"};
  return convert_enum_idx(names, 3, value, "data_forwarding_request_e");
}

// DefaultDRB ::= ENUMERATED
const char* default_drb_opts::to_string() const
{
  static const char* names[] = {"true", "false"};
  return convert_enum_idx(names, 2, value, "default_drb_e");
}

// DiscardTimer ::= ENUMERATED
const char* discard_timer_opts::to_string() const
{
  static const char* names[] = {"ms10",
                                "ms20",
                                "ms30",
                                "ms40",
                                "ms50",
                                "ms60",
                                "ms75",
                                "ms100",
                                "ms150",
                                "ms200",
                                "ms250",
                                "ms300",
                                "ms500",
                                "ms750",
                                "ms1500",
                                "infinity"};
  return convert_enum_idx(names, 16, value, "discard_timer_e");
}
int16_t discard_timer_opts::to_number() const
{
  static const int16_t numbers[] = {10, 20, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(numbers, 16, value, "discard_timer_e");
}

// Duplication-Activation ::= ENUMERATED
const char* dupl_activation_opts::to_string() const
{
  static const char* names[] = {"active", "inactive"};
  return convert_enum_idx(names, 2, value, "dupl_activation_e");
}

// EarlyDataForwardingIndicator ::= ENUMERATED
const char* early_data_forwarding_ind_opts::to_string() const
{
  static const char* names[] = {"stop"};
  return convert_enum_idx(names, 1, value, "early_data_forwarding_ind_e");
}

// EarlyForwardingCOUNTInfo ::= CHOICE
void early_forwarding_count_info_c::destroy_()
{
  switch (type_) {
    case types::first_dl_count:
      c.destroy<first_dl_count_s>();
      break;
    case types::dl_discarding_count:
      c.destroy<dl_discarding_s>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void early_forwarding_count_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::first_dl_count:
      c.init<first_dl_count_s>();
      break;
    case types::dl_discarding_count:
      c.init<dl_discarding_s>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_count_info_c");
  }
}
early_forwarding_count_info_c::early_forwarding_count_info_c(const early_forwarding_count_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::first_dl_count:
      c.init(other.c.get<first_dl_count_s>());
      break;
    case types::dl_discarding_count:
      c.init(other.c.get<dl_discarding_s>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_count_info_c");
  }
}
early_forwarding_count_info_c& early_forwarding_count_info_c::operator=(const early_forwarding_count_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::first_dl_count:
      c.set(other.c.get<first_dl_count_s>());
      break;
    case types::dl_discarding_count:
      c.set(other.c.get<dl_discarding_s>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_count_info_c");
  }

  return *this;
}
first_dl_count_s& early_forwarding_count_info_c::set_first_dl_count()
{
  set(types::first_dl_count);
  return c.get<first_dl_count_s>();
}
dl_discarding_s& early_forwarding_count_info_c::set_dl_discarding_count()
{
  set(types::dl_discarding_count);
  return c.get<dl_discarding_s>();
}
protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>& early_forwarding_count_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>();
}
void early_forwarding_count_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::first_dl_count:
      j.write_fieldname("firstDLCount");
      c.get<first_dl_count_s>().to_json(j);
      break;
    case types::dl_discarding_count:
      j.write_fieldname("dLDiscardingCount");
      c.get<dl_discarding_s>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-Extension");
      c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_count_info_c");
  }
  j.end_obj();
}
SRSASN_CODE early_forwarding_count_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::first_dl_count:
      HANDLE_CODE(c.get<first_dl_count_s>().pack(bref));
      break;
    case types::dl_discarding_count:
      HANDLE_CODE(c.get<dl_discarding_s>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_count_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE early_forwarding_count_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::first_dl_count:
      HANDLE_CODE(c.get<first_dl_count_s>().unpack(bref));
      break;
    case types::dl_discarding_count:
      HANDLE_CODE(c.get<dl_discarding_s>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_count_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* early_forwarding_count_info_c::types_opts::to_string() const
{
  static const char* names[] = {"firstDLCount", "dLDiscardingCount", "choice-Extension"};
  return convert_enum_idx(names, 3, value, "early_forwarding_count_info_c::types");
}

// EarlyForwardingCOUNTReq ::= ENUMERATED
const char* early_forwarding_count_req_opts::to_string() const
{
  static const char* names[] = {"first-dl-count", "dl-discarding"};
  return convert_enum_idx(names, 2, value, "early_forwarding_count_req_e");
}

// IgnoreMappingRuleIndication ::= ENUMERATED
const char* ignore_map_rule_ind_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "ignore_map_rule_ind_e");
}

// MaxIPrate ::= ENUMERATED
const char* max_ip_rate_opts::to_string() const
{
  static const char* names[] = {"bitrate64kbs", "max-UErate"};
  return convert_enum_idx(names, 2, value, "max_ip_rate_e");
}
uint8_t max_ip_rate_opts::to_number() const
{
  static const uint8_t numbers[] = {64};
  return map_enum_number(numbers, 1, value, "max_ip_rate_e");
}

// OutOfOrderDelivery ::= ENUMERATED
const char* out_of_order_delivery_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "out_of_order_delivery_e");
}

// PDCP-Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdcp_cfg_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {88, 104, 118, 177};
  return map_enum_number(names, 4, idx, "id");
}
bool pdcp_cfg_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {88, 104, 118, 177};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e pdcp_cfg_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 88:
      return crit_e::ignore;
    case 104:
      return crit_e::ignore;
    case 118:
      return crit_e::ignore;
    case 177:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
pdcp_cfg_ext_ies_o::ext_c pdcp_cfg_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 88:
      ret.set(ext_c::types::pdcp_status_report_ind);
      break;
    case 104:
      ret.set(ext_c::types::add_pdcp_dupl_info);
      break;
    case 118:
      ret.set(ext_c::types::ehc_params);
      break;
    case 177:
      ret.set(ext_c::types::discard_timer_extended);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdcp_cfg_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 88:
      return presence_e::optional;
    case 104:
      return presence_e::optional;
    case 118:
      return presence_e::optional;
    case 177:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void pdcp_cfg_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::pdcp_status_report_ind:
      c = pdcp_status_report_ind_e{};
      break;
    case types::add_pdcp_dupl_info:
      c = add_pdcp_dupl_info_e{};
      break;
    case types::ehc_params:
      c = ehc_params_s{};
      break;
    case types::discard_timer_extended:
      c = discard_timer_extended_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_ext_ies_o::ext_c");
  }
}
pdcp_status_report_ind_e& pdcp_cfg_ext_ies_o::ext_c::pdcp_status_report_ind()
{
  assert_choice_type(types::pdcp_status_report_ind, type_, "Extension");
  return c.get<pdcp_status_report_ind_e>();
}
add_pdcp_dupl_info_e& pdcp_cfg_ext_ies_o::ext_c::add_pdcp_dupl_info()
{
  assert_choice_type(types::add_pdcp_dupl_info, type_, "Extension");
  return c.get<add_pdcp_dupl_info_e>();
}
ehc_params_s& pdcp_cfg_ext_ies_o::ext_c::ehc_params()
{
  assert_choice_type(types::ehc_params, type_, "Extension");
  return c.get<ehc_params_s>();
}
discard_timer_extended_e& pdcp_cfg_ext_ies_o::ext_c::discard_timer_extended()
{
  assert_choice_type(types::discard_timer_extended, type_, "Extension");
  return c.get<discard_timer_extended_e>();
}
const pdcp_status_report_ind_e& pdcp_cfg_ext_ies_o::ext_c::pdcp_status_report_ind() const
{
  assert_choice_type(types::pdcp_status_report_ind, type_, "Extension");
  return c.get<pdcp_status_report_ind_e>();
}
const add_pdcp_dupl_info_e& pdcp_cfg_ext_ies_o::ext_c::add_pdcp_dupl_info() const
{
  assert_choice_type(types::add_pdcp_dupl_info, type_, "Extension");
  return c.get<add_pdcp_dupl_info_e>();
}
const ehc_params_s& pdcp_cfg_ext_ies_o::ext_c::ehc_params() const
{
  assert_choice_type(types::ehc_params, type_, "Extension");
  return c.get<ehc_params_s>();
}
const discard_timer_extended_e& pdcp_cfg_ext_ies_o::ext_c::discard_timer_extended() const
{
  assert_choice_type(types::discard_timer_extended, type_, "Extension");
  return c.get<discard_timer_extended_e>();
}
void pdcp_cfg_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pdcp_status_report_ind:
      j.write_str("PDCP-StatusReportIndication", c.get<pdcp_status_report_ind_e>().to_string());
      break;
    case types::add_pdcp_dupl_info:
      j.write_str("AdditionalPDCPduplicationInformation", c.get<add_pdcp_dupl_info_e>().to_string());
      break;
    case types::ehc_params:
      j.write_fieldname("EHC-Parameters");
      c.get<ehc_params_s>().to_json(j);
      break;
    case types::discard_timer_extended:
      j.write_str("DiscardTimerExtended", c.get<discard_timer_extended_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdcp_status_report_ind:
      HANDLE_CODE(c.get<pdcp_status_report_ind_e>().pack(bref));
      break;
    case types::add_pdcp_dupl_info:
      HANDLE_CODE(c.get<add_pdcp_dupl_info_e>().pack(bref));
      break;
    case types::ehc_params:
      HANDLE_CODE(c.get<ehc_params_s>().pack(bref));
      break;
    case types::discard_timer_extended:
      HANDLE_CODE(c.get<discard_timer_extended_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdcp_status_report_ind:
      HANDLE_CODE(c.get<pdcp_status_report_ind_e>().unpack(bref));
      break;
    case types::add_pdcp_dupl_info:
      HANDLE_CODE(c.get<add_pdcp_dupl_info_e>().unpack(bref));
      break;
    case types::ehc_params:
      HANDLE_CODE(c.get<ehc_params_s>().unpack(bref));
      break;
    case types::discard_timer_extended:
      HANDLE_CODE(c.get<discard_timer_extended_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdcp_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {
      "PDCP-StatusReportIndication", "AdditionalPDCPduplicationInformation", "EHC-Parameters", "DiscardTimerExtended"};
  return convert_enum_idx(names, 4, value, "pdcp_cfg_ext_ies_o::ext_c::types");
}

// PDCP-DataRecovery ::= ENUMERATED
const char* pdcp_data_recovery_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "pdcp_data_recovery_e");
}

// PDCP-Duplication ::= ENUMERATED
const char* pdcp_dupl_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "pdcp_dupl_e");
}

// PDCP-Reestablishment ::= ENUMERATED
const char* pdcp_reest_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "pdcp_reest_e");
}

// PDCP-SN-Size ::= ENUMERATED
const char* pdcp_sn_size_opts::to_string() const
{
  static const char* names[] = {"s-12", "s-18"};
  return convert_enum_idx(names, 2, value, "pdcp_sn_size_e");
}
int8_t pdcp_sn_size_opts::to_number() const
{
  static const int8_t numbers[] = {-12, -18};
  return map_enum_number(numbers, 2, value, "pdcp_sn_size_e");
}

template struct asn1::protocol_ext_field_s<qos_flow_qos_param_item_ext_ies_o>;

SRSASN_CODE qos_flow_qos_param_item_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += redundant_qos_flow_ind_present ? 1 : 0;
  nof_ies += tsc_traffic_characteristics_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (redundant_qos_flow_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)99, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_qos_flow_ind.pack(bref));
  }
  if (tsc_traffic_characteristics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)100, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(tsc_traffic_characteristics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_qos_param_item_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 99: {
        redundant_qos_flow_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_qos_flow_ind.unpack(bref));
        break;
      }
      case 100: {
        tsc_traffic_characteristics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(tsc_traffic_characteristics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void qos_flow_qos_param_item_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (redundant_qos_flow_ind_present) {
    j.write_int("id", 99);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", redundant_qos_flow_ind.to_string());
  }
  if (tsc_traffic_characteristics_present) {
    j.write_int("id", 100);
    j.write_str("criticality", "ignore");
    tsc_traffic_characteristics.to_json(j);
  }
  j.end_obj();
}

// QoS-Flow-QoS-Parameter-Item ::= SEQUENCE
SRSASN_CODE qos_flow_qos_param_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qos_flow_map_ind_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(qos_flow_level_qos_params.pack(bref));
  if (qos_flow_map_ind_present) {
    HANDLE_CODE(qos_flow_map_ind.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_qos_param_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(qos_flow_map_ind_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(qos_flow_level_qos_params.unpack(bref));
  if (qos_flow_map_ind_present) {
    HANDLE_CODE(qos_flow_map_ind.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_flow_qos_param_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qos_flow_id);
  j.write_fieldname("qoSFlowLevelQoSParameters");
  qos_flow_level_qos_params.to_json(j);
  if (qos_flow_map_ind_present) {
    j.write_str("qoSFlowMappingIndication", qos_flow_map_ind.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// QoS-Flows-DRB-Remapping ::= ENUMERATED
const char* qos_flows_drb_remap_opts::to_string() const
{
  static const char* names[] = {"update", "source-configuration"};
  return convert_enum_idx(names, 2, value, "qos_flows_drb_remap_e");
}

// RLC-Mode ::= ENUMERATED
const char* rlc_mode_opts::to_string() const
{
  static const char* names[] = {
      "rlc-tm", "rlc-am", "rlc-um-bidirectional", "rlc-um-unidirectional-ul", "rlc-um-unidirectional-dl"};
  return convert_enum_idx(names, 5, value, "rlc_mode_e");
}

// ROHC-Parameters ::= CHOICE
void rohc_params_c::destroy_()
{
  switch (type_) {
    case types::rohc:
      c.destroy<rohc_s>();
      break;
    case types::ul_only_rohc:
      c.destroy<ul_only_rohc_s>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<rohc_params_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void rohc_params_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rohc:
      c.init<rohc_s>();
      break;
    case types::ul_only_rohc:
      c.init<ul_only_rohc_s>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<rohc_params_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rohc_params_c");
  }
}
rohc_params_c::rohc_params_c(const rohc_params_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rohc:
      c.init(other.c.get<rohc_s>());
      break;
    case types::ul_only_rohc:
      c.init(other.c.get<ul_only_rohc_s>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rohc_params_c");
  }
}
rohc_params_c& rohc_params_c::operator=(const rohc_params_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rohc:
      c.set(other.c.get<rohc_s>());
      break;
    case types::ul_only_rohc:
      c.set(other.c.get<ul_only_rohc_s>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rohc_params_c");
  }

  return *this;
}
rohc_s& rohc_params_c::set_rohc()
{
  set(types::rohc);
  return c.get<rohc_s>();
}
ul_only_rohc_s& rohc_params_c::set_ul_only_rohc()
{
  set(types::ul_only_rohc);
  return c.get<ul_only_rohc_s>();
}
protocol_ie_single_container_s<rohc_params_ext_ies_o>& rohc_params_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>();
}
void rohc_params_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rohc:
      j.write_fieldname("rOHC");
      c.get<rohc_s>().to_json(j);
      break;
    case types::ul_only_rohc:
      j.write_fieldname("uPlinkOnlyROHC");
      c.get<ul_only_rohc_s>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-Extension");
      c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rohc_params_c");
  }
  j.end_obj();
}
SRSASN_CODE rohc_params_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rohc:
      HANDLE_CODE(c.get<rohc_s>().pack(bref));
      break;
    case types::ul_only_rohc:
      HANDLE_CODE(c.get<ul_only_rohc_s>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rohc_params_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rohc_params_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rohc:
      HANDLE_CODE(c.get<rohc_s>().unpack(bref));
      break;
    case types::ul_only_rohc:
      HANDLE_CODE(c.get<ul_only_rohc_s>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rohc_params_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rohc_params_c::types_opts::to_string() const
{
  static const char* names[] = {"rOHC", "uPlinkOnlyROHC", "choice-Extension"};
  return convert_enum_idx(names, 3, value, "rohc_params_c::types");
}

// SDAP-Header-DL ::= ENUMERATED
const char* sdap_hdr_dl_opts::to_string() const
{
  static const char* names[] = {"present", "absent"};
  return convert_enum_idx(names, 2, value, "sdap_hdr_dl_e");
}

// SDAP-Header-UL ::= ENUMERATED
const char* sdap_hdr_ul_opts::to_string() const
{
  static const char* names[] = {"present", "absent"};
  return convert_enum_idx(names, 2, value, "sdap_hdr_ul_e");
}

// T-ReorderingTimer ::= SEQUENCE
SRSASN_CODE t_reordering_timer_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(t_reordering.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE t_reordering_timer_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(t_reordering.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void t_reordering_timer_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-Reordering", t_reordering.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// ULDataSplitThreshold ::= ENUMERATED
const char* ul_data_split_thres_opts::to_string() const
{
  static const char* names[] = {"b0",       "b100",     "b200",     "b400",     "b800",     "b1600",
                                "b3200",    "b6400",    "b12800",   "b25600",   "b51200",   "b102400",
                                "b204800",  "b409600",  "b819200",  "b1228800", "b1638400", "b2457600",
                                "b3276800", "b4096000", "b4915200", "b5734400", "b6553600", "infinity"};
  return convert_enum_idx(names, 24, value, "ul_data_split_thres_e");
}
int32_t ul_data_split_thres_opts::to_number() const
{
  static const int32_t numbers[] = {0,       100,     200,     400,     800,     1600,    3200,    6400,
                                    12800,   25600,   51200,   102400,  204800,  409600,  819200,  1228800,
                                    1638400, 2457600, 3276800, 4096000, 4915200, 5734400, 6553600, -1};
  return map_enum_number(numbers, 24, value, "ul_data_split_thres_e");
}

// UP-Parameters-Item ::= SEQUENCE
SRSASN_CODE up_params_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(up_tnl_info.pack(bref));
  HANDLE_CODE(pack_integer(bref, cell_group_id, (uint8_t)0u, (uint8_t)3u, true, true));
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE up_params_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(up_tnl_info.unpack(bref));
  HANDLE_CODE(unpack_integer(cell_group_id, bref, (uint8_t)0u, (uint8_t)3u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void up_params_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("uP-TNL-Information");
  up_tnl_info.to_json(j);
  j.write_int("cell-Group-ID", cell_group_id);
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// ConfidentialityProtectionIndication ::= ENUMERATED
const char* confidentiality_protection_ind_opts::to_string() const
{
  static const char* names[] = {"required", "preferred", "not-needed"};
  return convert_enum_idx(names, 3, value, "confidentiality_protection_ind_e");
}

// DRB-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_to_modify_item_ng_ran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {71, 72, 122, 123, 120, 140};
  return map_enum_number(names, 6, idx, "id");
}
bool drb_to_modify_item_ng_ran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {71, 72, 122, 123, 120, 140};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e drb_to_modify_item_ng_ran_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 71:
      return crit_e::reject;
    case 72:
      return crit_e::ignore;
    case 122:
      return crit_e::reject;
    case 123:
      return crit_e::reject;
    case 120:
      return crit_e::ignore;
    case 140:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
drb_to_modify_item_ng_ran_ext_ies_o::ext_c drb_to_modify_item_ng_ran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 71:
      ret.set(ext_c::types::old_qos_flow_map_ul_endmarkerexpected);
      break;
    case 72:
      ret.set(ext_c::types::drb_qos);
      break;
    case 122:
      ret.set(ext_c::types::early_forwarding_count_req);
      break;
    case 123:
      ret.set(ext_c::types::early_forwarding_count_info);
      break;
    case 120:
      ret.set(ext_c::types::daps_request_info);
      break;
    case 140:
      ret.set(ext_c::types::early_data_forwarding_ind);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_to_modify_item_ng_ran_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 71:
      return presence_e::optional;
    case 72:
      return presence_e::optional;
    case 122:
      return presence_e::optional;
    case 123:
      return presence_e::optional;
    case 120:
      return presence_e::optional;
    case 140:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void drb_to_modify_item_ng_ran_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::old_qos_flow_map_ul_endmarkerexpected:
      c = qos_flow_list_l{};
      break;
    case types::drb_qos:
      c = qos_flow_level_qos_params_s{};
      break;
    case types::early_forwarding_count_req:
      c = early_forwarding_count_req_e{};
      break;
    case types::early_forwarding_count_info:
      c = early_forwarding_count_info_c{};
      break;
    case types::daps_request_info:
      c = daps_request_info_s{};
      break;
    case types::early_data_forwarding_ind:
      c = early_data_forwarding_ind_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c");
  }
}
qos_flow_list_l& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::old_qos_flow_map_ul_endmarkerexpected()
{
  assert_choice_type(types::old_qos_flow_map_ul_endmarkerexpected, type_, "Extension");
  return c.get<qos_flow_list_l>();
}
qos_flow_level_qos_params_s& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::drb_qos()
{
  assert_choice_type(types::drb_qos, type_, "Extension");
  return c.get<qos_flow_level_qos_params_s>();
}
early_forwarding_count_req_e& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::early_forwarding_count_req()
{
  assert_choice_type(types::early_forwarding_count_req, type_, "Extension");
  return c.get<early_forwarding_count_req_e>();
}
early_forwarding_count_info_c& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::early_forwarding_count_info()
{
  assert_choice_type(types::early_forwarding_count_info, type_, "Extension");
  return c.get<early_forwarding_count_info_c>();
}
daps_request_info_s& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::daps_request_info()
{
  assert_choice_type(types::daps_request_info, type_, "Extension");
  return c.get<daps_request_info_s>();
}
early_data_forwarding_ind_e& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::early_data_forwarding_ind()
{
  assert_choice_type(types::early_data_forwarding_ind, type_, "Extension");
  return c.get<early_data_forwarding_ind_e>();
}
const qos_flow_list_l& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::old_qos_flow_map_ul_endmarkerexpected() const
{
  assert_choice_type(types::old_qos_flow_map_ul_endmarkerexpected, type_, "Extension");
  return c.get<qos_flow_list_l>();
}
const qos_flow_level_qos_params_s& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::drb_qos() const
{
  assert_choice_type(types::drb_qos, type_, "Extension");
  return c.get<qos_flow_level_qos_params_s>();
}
const early_forwarding_count_req_e& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::early_forwarding_count_req() const
{
  assert_choice_type(types::early_forwarding_count_req, type_, "Extension");
  return c.get<early_forwarding_count_req_e>();
}
const early_forwarding_count_info_c& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::early_forwarding_count_info() const
{
  assert_choice_type(types::early_forwarding_count_info, type_, "Extension");
  return c.get<early_forwarding_count_info_c>();
}
const daps_request_info_s& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::daps_request_info() const
{
  assert_choice_type(types::daps_request_info, type_, "Extension");
  return c.get<daps_request_info_s>();
}
const early_data_forwarding_ind_e& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::early_data_forwarding_ind() const
{
  assert_choice_type(types::early_data_forwarding_ind, type_, "Extension");
  return c.get<early_data_forwarding_ind_e>();
}
void drb_to_modify_item_ng_ran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::old_qos_flow_map_ul_endmarkerexpected:
      j.start_array("QoS-Flow-List");
      for (const auto& e1 : c.get<qos_flow_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_qos:
      j.write_fieldname("QoSFlowLevelQoSParameters");
      c.get<qos_flow_level_qos_params_s>().to_json(j);
      break;
    case types::early_forwarding_count_req:
      j.write_str("EarlyForwardingCOUNTReq", c.get<early_forwarding_count_req_e>().to_string());
      break;
    case types::early_forwarding_count_info:
      j.write_fieldname("EarlyForwardingCOUNTInfo");
      c.get<early_forwarding_count_info_c>().to_json(j);
      break;
    case types::daps_request_info:
      j.write_fieldname("DAPSRequestInfo");
      c.get<daps_request_info_s>().to_json(j);
      break;
    case types::early_data_forwarding_ind:
      j.write_str("EarlyDataForwardingIndicator", "stop");
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE drb_to_modify_item_ng_ran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::old_qos_flow_map_ul_endmarkerexpected:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<qos_flow_list_l>(), 1, 64, true));
      break;
    case types::drb_qos:
      HANDLE_CODE(c.get<qos_flow_level_qos_params_s>().pack(bref));
      break;
    case types::early_forwarding_count_req:
      HANDLE_CODE(c.get<early_forwarding_count_req_e>().pack(bref));
      break;
    case types::early_forwarding_count_info:
      HANDLE_CODE(c.get<early_forwarding_count_info_c>().pack(bref));
      break;
    case types::daps_request_info:
      HANDLE_CODE(c.get<daps_request_info_s>().pack(bref));
      break;
    case types::early_data_forwarding_ind:
      HANDLE_CODE(c.get<early_data_forwarding_ind_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_modify_item_ng_ran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::old_qos_flow_map_ul_endmarkerexpected:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<qos_flow_list_l>(), bref, 1, 64, true));
      break;
    case types::drb_qos:
      HANDLE_CODE(c.get<qos_flow_level_qos_params_s>().unpack(bref));
      break;
    case types::early_forwarding_count_req:
      HANDLE_CODE(c.get<early_forwarding_count_req_e>().unpack(bref));
      break;
    case types::early_forwarding_count_info:
      HANDLE_CODE(c.get<early_forwarding_count_info_c>().unpack(bref));
      break;
    case types::daps_request_info:
      HANDLE_CODE(c.get<daps_request_info_s>().unpack(bref));
      break;
    case types::early_data_forwarding_ind:
      HANDLE_CODE(c.get<early_data_forwarding_ind_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drb_to_modify_item_ng_ran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QoS-Flow-List",
                                "QoSFlowLevelQoSParameters",
                                "EarlyForwardingCOUNTReq",
                                "EarlyForwardingCOUNTInfo",
                                "DAPSRequestInfo",
                                "EarlyDataForwardingIndicator"};
  return convert_enum_idx(names, 6, value, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c::types");
}

// DRB-To-Setup-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_to_setup_item_ng_ran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {72, 120, 138, 141};
  return map_enum_number(names, 4, idx, "id");
}
bool drb_to_setup_item_ng_ran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {72, 120, 138, 141};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e drb_to_setup_item_ng_ran_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 72:
      return crit_e::ignore;
    case 120:
      return crit_e::ignore;
    case 138:
      return crit_e::reject;
    case 141:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
drb_to_setup_item_ng_ran_ext_ies_o::ext_c drb_to_setup_item_ng_ran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 72:
      ret.set(ext_c::types::drb_qos);
      break;
    case 120:
      ret.set(ext_c::types::daps_request_info);
      break;
    case 138:
      ret.set(ext_c::types::ignore_map_rule_ind);
      break;
    case 141:
      ret.set(ext_c::types::qos_flows_drb_remap);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_to_setup_item_ng_ran_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 72:
      return presence_e::optional;
    case 120:
      return presence_e::optional;
    case 138:
      return presence_e::optional;
    case 141:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void drb_to_setup_item_ng_ran_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::drb_qos:
      c = qos_flow_level_qos_params_s{};
      break;
    case types::daps_request_info:
      c = daps_request_info_s{};
      break;
    case types::ignore_map_rule_ind:
      c = ignore_map_rule_ind_e{};
      break;
    case types::qos_flows_drb_remap:
      c = qos_flows_drb_remap_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_setup_item_ng_ran_ext_ies_o::ext_c");
  }
}
qos_flow_level_qos_params_s& drb_to_setup_item_ng_ran_ext_ies_o::ext_c::drb_qos()
{
  assert_choice_type(types::drb_qos, type_, "Extension");
  return c.get<qos_flow_level_qos_params_s>();
}
daps_request_info_s& drb_to_setup_item_ng_ran_ext_ies_o::ext_c::daps_request_info()
{
  assert_choice_type(types::daps_request_info, type_, "Extension");
  return c.get<daps_request_info_s>();
}
ignore_map_rule_ind_e& drb_to_setup_item_ng_ran_ext_ies_o::ext_c::ignore_map_rule_ind()
{
  assert_choice_type(types::ignore_map_rule_ind, type_, "Extension");
  return c.get<ignore_map_rule_ind_e>();
}
qos_flows_drb_remap_e& drb_to_setup_item_ng_ran_ext_ies_o::ext_c::qos_flows_drb_remap()
{
  assert_choice_type(types::qos_flows_drb_remap, type_, "Extension");
  return c.get<qos_flows_drb_remap_e>();
}
const qos_flow_level_qos_params_s& drb_to_setup_item_ng_ran_ext_ies_o::ext_c::drb_qos() const
{
  assert_choice_type(types::drb_qos, type_, "Extension");
  return c.get<qos_flow_level_qos_params_s>();
}
const daps_request_info_s& drb_to_setup_item_ng_ran_ext_ies_o::ext_c::daps_request_info() const
{
  assert_choice_type(types::daps_request_info, type_, "Extension");
  return c.get<daps_request_info_s>();
}
const ignore_map_rule_ind_e& drb_to_setup_item_ng_ran_ext_ies_o::ext_c::ignore_map_rule_ind() const
{
  assert_choice_type(types::ignore_map_rule_ind, type_, "Extension");
  return c.get<ignore_map_rule_ind_e>();
}
const qos_flows_drb_remap_e& drb_to_setup_item_ng_ran_ext_ies_o::ext_c::qos_flows_drb_remap() const
{
  assert_choice_type(types::qos_flows_drb_remap, type_, "Extension");
  return c.get<qos_flows_drb_remap_e>();
}
void drb_to_setup_item_ng_ran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drb_qos:
      j.write_fieldname("QoSFlowLevelQoSParameters");
      c.get<qos_flow_level_qos_params_s>().to_json(j);
      break;
    case types::daps_request_info:
      j.write_fieldname("DAPSRequestInfo");
      c.get<daps_request_info_s>().to_json(j);
      break;
    case types::ignore_map_rule_ind:
      j.write_str("IgnoreMappingRuleIndication", "true");
      break;
    case types::qos_flows_drb_remap:
      j.write_str("QoS-Flows-DRB-Remapping", c.get<qos_flows_drb_remap_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_setup_item_ng_ran_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE drb_to_setup_item_ng_ran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_qos:
      HANDLE_CODE(c.get<qos_flow_level_qos_params_s>().pack(bref));
      break;
    case types::daps_request_info:
      HANDLE_CODE(c.get<daps_request_info_s>().pack(bref));
      break;
    case types::ignore_map_rule_ind:
      HANDLE_CODE(c.get<ignore_map_rule_ind_e>().pack(bref));
      break;
    case types::qos_flows_drb_remap:
      HANDLE_CODE(c.get<qos_flows_drb_remap_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_setup_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_item_ng_ran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_qos:
      HANDLE_CODE(c.get<qos_flow_level_qos_params_s>().unpack(bref));
      break;
    case types::daps_request_info:
      HANDLE_CODE(c.get<daps_request_info_s>().unpack(bref));
      break;
    case types::ignore_map_rule_ind:
      HANDLE_CODE(c.get<ignore_map_rule_ind_e>().unpack(bref));
      break;
    case types::qos_flows_drb_remap:
      HANDLE_CODE(c.get<qos_flows_drb_remap_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_setup_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drb_to_setup_item_ng_ran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {
      "QoSFlowLevelQoSParameters", "DAPSRequestInfo", "IgnoreMappingRuleIndication", "QoS-Flows-DRB-Remapping"};
  return convert_enum_idx(names, 4, value, "drb_to_setup_item_ng_ran_ext_ies_o::ext_c::types");
}

// DRB-To-Setup-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_to_setup_mod_item_ng_ran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {72, 138, 120};
  return map_enum_number(names, 3, idx, "id");
}
bool drb_to_setup_mod_item_ng_ran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {72, 138, 120};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e drb_to_setup_mod_item_ng_ran_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 72:
      return crit_e::ignore;
    case 138:
      return crit_e::reject;
    case 120:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c drb_to_setup_mod_item_ng_ran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 72:
      ret.set(ext_c::types::drb_qos);
      break;
    case 138:
      ret.set(ext_c::types::ignore_map_rule_ind);
      break;
    case 120:
      ret.set(ext_c::types::daps_request_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_to_setup_mod_item_ng_ran_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 72:
      return presence_e::optional;
    case 138:
      return presence_e::optional;
    case 120:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::drb_qos:
      c = qos_flow_level_qos_params_s{};
      break;
    case types::ignore_map_rule_ind:
      c = ignore_map_rule_ind_e{};
      break;
    case types::daps_request_info:
      c = daps_request_info_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c");
  }
}
qos_flow_level_qos_params_s& drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::drb_qos()
{
  assert_choice_type(types::drb_qos, type_, "Extension");
  return c.get<qos_flow_level_qos_params_s>();
}
ignore_map_rule_ind_e& drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::ignore_map_rule_ind()
{
  assert_choice_type(types::ignore_map_rule_ind, type_, "Extension");
  return c.get<ignore_map_rule_ind_e>();
}
daps_request_info_s& drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::daps_request_info()
{
  assert_choice_type(types::daps_request_info, type_, "Extension");
  return c.get<daps_request_info_s>();
}
const qos_flow_level_qos_params_s& drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::drb_qos() const
{
  assert_choice_type(types::drb_qos, type_, "Extension");
  return c.get<qos_flow_level_qos_params_s>();
}
const ignore_map_rule_ind_e& drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::ignore_map_rule_ind() const
{
  assert_choice_type(types::ignore_map_rule_ind, type_, "Extension");
  return c.get<ignore_map_rule_ind_e>();
}
const daps_request_info_s& drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::daps_request_info() const
{
  assert_choice_type(types::daps_request_info, type_, "Extension");
  return c.get<daps_request_info_s>();
}
void drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drb_qos:
      j.write_fieldname("QoSFlowLevelQoSParameters");
      c.get<qos_flow_level_qos_params_s>().to_json(j);
      break;
    case types::ignore_map_rule_ind:
      j.write_str("IgnoreMappingRuleIndication", "true");
      break;
    case types::daps_request_info:
      j.write_fieldname("DAPSRequestInfo");
      c.get<daps_request_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_qos:
      HANDLE_CODE(c.get<qos_flow_level_qos_params_s>().pack(bref));
      break;
    case types::ignore_map_rule_ind:
      HANDLE_CODE(c.get<ignore_map_rule_ind_e>().pack(bref));
      break;
    case types::daps_request_info:
      HANDLE_CODE(c.get<daps_request_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_qos:
      HANDLE_CODE(c.get<qos_flow_level_qos_params_s>().unpack(bref));
      break;
    case types::ignore_map_rule_ind:
      HANDLE_CODE(c.get<ignore_map_rule_ind_e>().unpack(bref));
      break;
    case types::daps_request_info:
      HANDLE_CODE(c.get<daps_request_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QoSFlowLevelQoSParameters", "IgnoreMappingRuleIndication", "DAPSRequestInfo"};
  return convert_enum_idx(names, 3, value, "drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::types");
}

// Data-Forwarding-Information ::= SEQUENCE
SRSASN_CODE data_forwarding_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_data_forwarding_present, 1));
  HANDLE_CODE(bref.pack(dl_data_forwarding_present, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  if (ul_data_forwarding_present) {
    HANDLE_CODE(ul_data_forwarding.pack(bref));
  }
  if (dl_data_forwarding_present) {
    HANDLE_CODE(dl_data_forwarding.pack(bref));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_forwarding_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_data_forwarding_present, 1));
  HANDLE_CODE(bref.unpack(dl_data_forwarding_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (ul_data_forwarding_present) {
    HANDLE_CODE(ul_data_forwarding.unpack(bref));
  }
  if (dl_data_forwarding_present) {
    HANDLE_CODE(dl_data_forwarding.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void data_forwarding_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_data_forwarding_present) {
    j.write_fieldname("uL-Data-Forwarding");
    ul_data_forwarding.to_json(j);
  }
  if (dl_data_forwarding_present) {
    j.write_fieldname("dL-Data-Forwarding");
    dl_data_forwarding.to_json(j);
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// Data-Forwarding-Information-Request ::= SEQUENCE
SRSASN_CODE data_forwarding_info_request_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qos_flows_forwarded_on_fwd_tunnels.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(data_forwarding_request.pack(bref));
  if (qos_flows_forwarded_on_fwd_tunnels.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, qos_flows_forwarded_on_fwd_tunnels, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_forwarding_info_request_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool qos_flows_forwarded_on_fwd_tunnels_present;
  HANDLE_CODE(bref.unpack(qos_flows_forwarded_on_fwd_tunnels_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(data_forwarding_request.unpack(bref));
  if (qos_flows_forwarded_on_fwd_tunnels_present) {
    HANDLE_CODE(unpack_dyn_seq_of(qos_flows_forwarded_on_fwd_tunnels, bref, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void data_forwarding_info_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("data-Forwarding-Request", data_forwarding_request.to_string());
  if (qos_flows_forwarded_on_fwd_tunnels.size() > 0) {
    j.start_array("qoS-Flows-Forwarded-On-Fwd-Tunnels");
    for (const auto& e1 : qos_flows_forwarded_on_fwd_tunnels) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DataForwardingtoE-UTRANInformationListItem ::= SEQUENCE
SRSASN_CODE data_forwardingto_e_utran_info_list_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(data_forwarding_tunnel_info.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, qos_flows_to_be_forwarded_list, 1, 64, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_forwardingto_e_utran_info_list_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(data_forwarding_tunnel_info.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(qos_flows_to_be_forwarded_list, bref, 1, 64, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void data_forwardingto_e_utran_info_list_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("data-forwarding-tunnel-information");
  data_forwarding_tunnel_info.to_json(j);
  j.start_array("qoS-Flows-to-be-forwarded-List");
  for (const auto& e1 : qos_flows_to_be_forwarded_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// IntegrityProtectionIndication ::= ENUMERATED
const char* integrity_protection_ind_opts::to_string() const
{
  static const char* names[] = {"required", "preferred", "not-needed"};
  return convert_enum_idx(names, 3, value, "integrity_protection_ind_e");
}

// MaximumIPdatarate ::= SEQUENCE
SRSASN_CODE max_ip_datarate_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(max_ip_rate.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE max_ip_datarate_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(max_ip_rate.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void max_ip_datarate_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxIPrate", max_ip_rate.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<pdcp_cfg_ext_ies_o>;

SRSASN_CODE pdcp_cfg_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += pdcp_status_report_ind_present ? 1 : 0;
  nof_ies += add_pdcp_dupl_info_present ? 1 : 0;
  nof_ies += ehc_params_present ? 1 : 0;
  nof_ies += discard_timer_extended_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (pdcp_status_report_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)88, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pdcp_status_report_ind.pack(bref));
  }
  if (add_pdcp_dupl_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)104, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(add_pdcp_dupl_info.pack(bref));
  }
  if (ehc_params_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)118, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ehc_params.pack(bref));
  }
  if (discard_timer_extended_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)177, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(discard_timer_extended.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 88: {
        pdcp_status_report_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdcp_status_report_ind.unpack(bref));
        break;
      }
      case 104: {
        add_pdcp_dupl_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_pdcp_dupl_info.unpack(bref));
        break;
      }
      case 118: {
        ehc_params_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ehc_params.unpack(bref));
        break;
      }
      case 177: {
        discard_timer_extended_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(discard_timer_extended.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void pdcp_cfg_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_status_report_ind_present) {
    j.write_int("id", 88);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", pdcp_status_report_ind.to_string());
  }
  if (add_pdcp_dupl_info_present) {
    j.write_int("id", 104);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", add_pdcp_dupl_info.to_string());
  }
  if (ehc_params_present) {
    j.write_int("id", 118);
    j.write_str("criticality", "ignore");
    ehc_params.to_json(j);
  }
  if (discard_timer_extended_present) {
    j.write_int("id", 177);
    j.write_str("criticality", "reject");
    j.write_str("Extension", discard_timer_extended.to_string());
  }
  j.end_obj();
}

// PDCP-Configuration ::= SEQUENCE
SRSASN_CODE pdcp_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rohc_params_present, 1));
  HANDLE_CODE(bref.pack(t_reordering_timer_present, 1));
  HANDLE_CODE(bref.pack(discard_timer_present, 1));
  HANDLE_CODE(bref.pack(ul_data_split_thres_present, 1));
  HANDLE_CODE(bref.pack(pdcp_dupl_present, 1));
  HANDLE_CODE(bref.pack(pdcp_reest_present, 1));
  HANDLE_CODE(bref.pack(pdcp_data_recovery_present, 1));
  HANDLE_CODE(bref.pack(dupl_activation_present, 1));
  HANDLE_CODE(bref.pack(out_of_order_delivery_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pdcp_sn_size_ul.pack(bref));
  HANDLE_CODE(pdcp_sn_size_dl.pack(bref));
  HANDLE_CODE(rlc_mode.pack(bref));
  if (rohc_params_present) {
    HANDLE_CODE(rohc_params.pack(bref));
  }
  if (t_reordering_timer_present) {
    HANDLE_CODE(t_reordering_timer.pack(bref));
  }
  if (discard_timer_present) {
    HANDLE_CODE(discard_timer.pack(bref));
  }
  if (ul_data_split_thres_present) {
    HANDLE_CODE(ul_data_split_thres.pack(bref));
  }
  if (pdcp_dupl_present) {
    HANDLE_CODE(pdcp_dupl.pack(bref));
  }
  if (pdcp_reest_present) {
    HANDLE_CODE(pdcp_reest.pack(bref));
  }
  if (pdcp_data_recovery_present) {
    HANDLE_CODE(pdcp_data_recovery.pack(bref));
  }
  if (dupl_activation_present) {
    HANDLE_CODE(dupl_activation.pack(bref));
  }
  if (out_of_order_delivery_present) {
    HANDLE_CODE(out_of_order_delivery.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rohc_params_present, 1));
  HANDLE_CODE(bref.unpack(t_reordering_timer_present, 1));
  HANDLE_CODE(bref.unpack(discard_timer_present, 1));
  HANDLE_CODE(bref.unpack(ul_data_split_thres_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_dupl_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_reest_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_data_recovery_present, 1));
  HANDLE_CODE(bref.unpack(dupl_activation_present, 1));
  HANDLE_CODE(bref.unpack(out_of_order_delivery_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(pdcp_sn_size_ul.unpack(bref));
  HANDLE_CODE(pdcp_sn_size_dl.unpack(bref));
  HANDLE_CODE(rlc_mode.unpack(bref));
  if (rohc_params_present) {
    HANDLE_CODE(rohc_params.unpack(bref));
  }
  if (t_reordering_timer_present) {
    HANDLE_CODE(t_reordering_timer.unpack(bref));
  }
  if (discard_timer_present) {
    HANDLE_CODE(discard_timer.unpack(bref));
  }
  if (ul_data_split_thres_present) {
    HANDLE_CODE(ul_data_split_thres.unpack(bref));
  }
  if (pdcp_dupl_present) {
    HANDLE_CODE(pdcp_dupl.unpack(bref));
  }
  if (pdcp_reest_present) {
    HANDLE_CODE(pdcp_reest.unpack(bref));
  }
  if (pdcp_data_recovery_present) {
    HANDLE_CODE(pdcp_data_recovery.unpack(bref));
  }
  if (dupl_activation_present) {
    HANDLE_CODE(dupl_activation.unpack(bref));
  }
  if (out_of_order_delivery_present) {
    HANDLE_CODE(out_of_order_delivery.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcp_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pDCP-SN-Size-UL", pdcp_sn_size_ul.to_string());
  j.write_str("pDCP-SN-Size-DL", pdcp_sn_size_dl.to_string());
  j.write_str("rLC-Mode", rlc_mode.to_string());
  if (rohc_params_present) {
    j.write_fieldname("rOHC-Parameters");
    rohc_params.to_json(j);
  }
  if (t_reordering_timer_present) {
    j.write_fieldname("t-ReorderingTimer");
    t_reordering_timer.to_json(j);
  }
  if (discard_timer_present) {
    j.write_str("discardTimer", discard_timer.to_string());
  }
  if (ul_data_split_thres_present) {
    j.write_str("uLDataSplitThreshold", ul_data_split_thres.to_string());
  }
  if (pdcp_dupl_present) {
    j.write_str("pDCP-Duplication", "true");
  }
  if (pdcp_reest_present) {
    j.write_str("pDCP-Reestablishment", "true");
  }
  if (pdcp_data_recovery_present) {
    j.write_str("pDCP-DataRecovery", "true");
  }
  if (dupl_activation_present) {
    j.write_str("duplication-Activation", dupl_activation.to_string());
  }
  if (out_of_order_delivery_present) {
    j.write_str("outOfOrderDelivery", "true");
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDCP-SN-Status-Information ::= SEQUENCE
SRSASN_CODE pdcp_sn_status_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_ext_present, 1));

  HANDLE_CODE(pdcp_status_transfer_ul.pack(bref));
  HANDLE_CODE(pdcp_status_transfer_dl.pack(bref));
  if (ie_ext_present) {
    HANDLE_CODE(ie_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_sn_status_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_ext_present, 1));

  HANDLE_CODE(pdcp_status_transfer_ul.unpack(bref));
  HANDLE_CODE(pdcp_status_transfer_dl.unpack(bref));
  if (ie_ext_present) {
    HANDLE_CODE(ie_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcp_sn_status_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pdcpStatusTransfer-UL");
  pdcp_status_transfer_ul.to_json(j);
  j.write_fieldname("pdcpStatusTransfer-DL");
  pdcp_status_transfer_dl.to_json(j);
  if (ie_ext_present) {
    j.write_fieldname("iE-Extension");
    ie_ext.to_json(j);
  }
  j.end_obj();
}

// PDCP-SN-Status-Request ::= ENUMERATED
const char* pdcp_sn_status_request_opts::to_string() const
{
  static const char* names[] = {"requested"};
  return convert_enum_idx(names, 1, value, "pdcp_sn_status_request_e");
}

// SDAP-Configuration ::= SEQUENCE
SRSASN_CODE sdap_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(default_drb.pack(bref));
  HANDLE_CODE(sdap_hdr_ul.pack(bref));
  HANDLE_CODE(sdap_hdr_dl.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdap_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(default_drb.unpack(bref));
  HANDLE_CODE(sdap_hdr_ul.unpack(bref));
  HANDLE_CODE(sdap_hdr_dl.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sdap_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("defaultDRB", default_drb.to_string());
  j.write_str("sDAP-Header-UL", sdap_hdr_ul.to_string());
  j.write_str("sDAP-Header-DL", sdap_hdr_dl.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<drb_to_modify_item_ng_ran_ext_ies_o>;

SRSASN_CODE drb_to_modify_item_ng_ran_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += old_qos_flow_map_ul_endmarkerexpected_present ? 1 : 0;
  nof_ies += drb_qos_present ? 1 : 0;
  nof_ies += early_forwarding_count_req_present ? 1 : 0;
  nof_ies += early_forwarding_count_info_present ? 1 : 0;
  nof_ies += daps_request_info_present ? 1 : 0;
  nof_ies += early_data_forwarding_ind_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (old_qos_flow_map_ul_endmarkerexpected_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)71, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, old_qos_flow_map_ul_endmarkerexpected, 1, 64, true));
  }
  if (drb_qos_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)72, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(drb_qos.pack(bref));
  }
  if (early_forwarding_count_req_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)122, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(early_forwarding_count_req.pack(bref));
  }
  if (early_forwarding_count_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)123, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(early_forwarding_count_info.pack(bref));
  }
  if (daps_request_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)120, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(daps_request_info.pack(bref));
  }
  if (early_data_forwarding_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)140, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(early_data_forwarding_ind.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_modify_item_ng_ran_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 71: {
        old_qos_flow_map_ul_endmarkerexpected_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(old_qos_flow_map_ul_endmarkerexpected, bref, 1, 64, true));
        break;
      }
      case 72: {
        drb_qos_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_qos.unpack(bref));
        break;
      }
      case 122: {
        early_forwarding_count_req_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(early_forwarding_count_req.unpack(bref));
        break;
      }
      case 123: {
        early_forwarding_count_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(early_forwarding_count_info.unpack(bref));
        break;
      }
      case 120: {
        daps_request_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(daps_request_info.unpack(bref));
        break;
      }
      case 140: {
        early_data_forwarding_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(early_data_forwarding_ind.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void drb_to_modify_item_ng_ran_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (old_qos_flow_map_ul_endmarkerexpected_present) {
    j.write_int("id", 71);
    j.write_str("criticality", "reject");
    j.start_array("Extension");
    for (const auto& e1 : old_qos_flow_map_ul_endmarkerexpected) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_qos_present) {
    j.write_int("id", 72);
    j.write_str("criticality", "ignore");
    drb_qos.to_json(j);
  }
  if (early_forwarding_count_req_present) {
    j.write_int("id", 122);
    j.write_str("criticality", "reject");
    j.write_str("Extension", early_forwarding_count_req.to_string());
  }
  if (early_forwarding_count_info_present) {
    j.write_int("id", 123);
    j.write_str("criticality", "reject");
    early_forwarding_count_info.to_json(j);
  }
  if (daps_request_info_present) {
    j.write_int("id", 120);
    j.write_str("criticality", "ignore");
    daps_request_info.to_json(j);
  }
  if (early_data_forwarding_ind_present) {
    j.write_int("id", 140);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", "stop");
  }
  j.end_obj();
}

// DRB-To-Modify-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_to_modify_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sdap_cfg_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_present, 1));
  HANDLE_CODE(bref.pack(drb_data_forwarding_info_present, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_request_present, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.pack(dl_up_params.size() > 0, 1));
  HANDLE_CODE(bref.pack(cell_group_to_add.size() > 0, 1));
  HANDLE_CODE(bref.pack(cell_group_to_modify.size() > 0, 1));
  HANDLE_CODE(bref.pack(cell_group_to_rem.size() > 0, 1));
  HANDLE_CODE(bref.pack(flow_map_info.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (sdap_cfg_present) {
    HANDLE_CODE(sdap_cfg.pack(bref));
  }
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.pack(bref));
  }
  if (drb_data_forwarding_info_present) {
    HANDLE_CODE(drb_data_forwarding_info.pack(bref));
  }
  if (pdcp_sn_status_request_present) {
    HANDLE_CODE(pdcp_sn_status_request.pack(bref));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.pack(bref));
  }
  if (dl_up_params.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_up_params, 1, 8, true));
  }
  if (cell_group_to_add.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_to_add, 1, 4, true));
  }
  if (cell_group_to_modify.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_to_modify, 1, 4, true));
  }
  if (cell_group_to_rem.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_to_rem, 1, 4, true));
  }
  if (flow_map_info.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, flow_map_info, 1, 64, true));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, drb_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_modify_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sdap_cfg_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_present, 1));
  HANDLE_CODE(bref.unpack(drb_data_forwarding_info_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_request_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_info_present, 1));
  bool dl_up_params_present;
  HANDLE_CODE(bref.unpack(dl_up_params_present, 1));
  bool cell_group_to_add_present;
  HANDLE_CODE(bref.unpack(cell_group_to_add_present, 1));
  bool cell_group_to_modify_present;
  HANDLE_CODE(bref.unpack(cell_group_to_modify_present, 1));
  bool cell_group_to_rem_present;
  HANDLE_CODE(bref.unpack(cell_group_to_rem_present, 1));
  bool flow_map_info_present;
  HANDLE_CODE(bref.unpack(flow_map_info_present, 1));
  HANDLE_CODE(bref.unpack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (sdap_cfg_present) {
    HANDLE_CODE(sdap_cfg.unpack(bref));
  }
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.unpack(bref));
  }
  if (drb_data_forwarding_info_present) {
    HANDLE_CODE(drb_data_forwarding_info.unpack(bref));
  }
  if (pdcp_sn_status_request_present) {
    HANDLE_CODE(pdcp_sn_status_request.unpack(bref));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.unpack(bref));
  }
  if (dl_up_params_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dl_up_params, bref, 1, 8, true));
  }
  if (cell_group_to_add_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_group_to_add, bref, 1, 4, true));
  }
  if (cell_group_to_modify_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_group_to_modify, bref, 1, 4, true));
  }
  if (cell_group_to_rem_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_group_to_rem, bref, 1, 4, true));
  }
  if (flow_map_info_present) {
    HANDLE_CODE(unpack_dyn_seq_of(flow_map_info, bref, 1, 64, true));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(drb_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_to_modify_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (sdap_cfg_present) {
    j.write_fieldname("sDAP-Configuration");
    sdap_cfg.to_json(j);
  }
  if (pdcp_cfg_present) {
    j.write_fieldname("pDCP-Configuration");
    pdcp_cfg.to_json(j);
  }
  if (drb_data_forwarding_info_present) {
    j.write_fieldname("dRB-Data-Forwarding-Information");
    drb_data_forwarding_info.to_json(j);
  }
  if (pdcp_sn_status_request_present) {
    j.write_str("pDCP-SN-Status-Request", "requested");
  }
  if (pdcp_sn_status_info_present) {
    j.write_fieldname("pdcp-SN-Status-Information");
    pdcp_sn_status_info.to_json(j);
  }
  if (dl_up_params.size() > 0) {
    j.start_array("dL-UP-Parameters");
    for (const auto& e1 : dl_up_params) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_group_to_add.size() > 0) {
    j.start_array("cell-Group-To-Add");
    for (const auto& e1 : cell_group_to_add) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_group_to_modify.size() > 0) {
    j.start_array("cell-Group-To-Modify");
    for (const auto& e1 : cell_group_to_modify) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_group_to_rem.size() > 0) {
    j.start_array("cell-Group-To-Remove");
    for (const auto& e1 : cell_group_to_rem) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (flow_map_info.size() > 0) {
    j.start_array("flow-Mapping-Information");
    for (const auto& e1 : flow_map_info) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_inactivity_timer_present) {
    j.write_int("dRB-Inactivity-Timer", drb_inactivity_timer);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-To-Remove-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_to_rem_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_rem_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_to_rem_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<drb_to_setup_item_ng_ran_ext_ies_o>;

SRSASN_CODE drb_to_setup_item_ng_ran_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += drb_qos_present ? 1 : 0;
  nof_ies += daps_request_info_present ? 1 : 0;
  nof_ies += ignore_map_rule_ind_present ? 1 : 0;
  nof_ies += qos_flows_drb_remap_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (drb_qos_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)72, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(drb_qos.pack(bref));
  }
  if (daps_request_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)120, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(daps_request_info.pack(bref));
  }
  if (ignore_map_rule_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)138, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ignore_map_rule_ind.pack(bref));
  }
  if (qos_flows_drb_remap_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)141, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(qos_flows_drb_remap.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_item_ng_ran_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 72: {
        drb_qos_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_qos.unpack(bref));
        break;
      }
      case 120: {
        daps_request_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(daps_request_info.unpack(bref));
        break;
      }
      case 138: {
        ignore_map_rule_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ignore_map_rule_ind.unpack(bref));
        break;
      }
      case 141: {
        qos_flows_drb_remap_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(qos_flows_drb_remap.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void drb_to_setup_item_ng_ran_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_qos_present) {
    j.write_int("id", 72);
    j.write_str("criticality", "ignore");
    drb_qos.to_json(j);
  }
  if (daps_request_info_present) {
    j.write_int("id", 120);
    j.write_str("criticality", "ignore");
    daps_request_info.to_json(j);
  }
  if (ignore_map_rule_ind_present) {
    j.write_int("id", 138);
    j.write_str("criticality", "reject");
    j.write_str("Extension", "true");
  }
  if (qos_flows_drb_remap_present) {
    j.write_int("id", 141);
    j.write_str("criticality", "reject");
    j.write_str("Extension", qos_flows_drb_remap.to_string());
  }
  j.end_obj();
}

// DRB-To-Setup-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_to_setup_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(sdap_cfg.pack(bref));
  HANDLE_CODE(pdcp_cfg.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_info, 1, 4, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, qos_flow_info_to_be_setup, 1, 64, true));
  if (drb_data_forwarding_info_request_present) {
    HANDLE_CODE(drb_data_forwarding_info_request.pack(bref));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, drb_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.unpack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(sdap_cfg.unpack(bref));
  HANDLE_CODE(pdcp_cfg.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(cell_group_info, bref, 1, 4, true));
  HANDLE_CODE(unpack_dyn_seq_of(qos_flow_info_to_be_setup, bref, 1, 64, true));
  if (drb_data_forwarding_info_request_present) {
    HANDLE_CODE(drb_data_forwarding_info_request.unpack(bref));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(drb_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_to_setup_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("sDAP-Configuration");
  sdap_cfg.to_json(j);
  j.write_fieldname("pDCP-Configuration");
  pdcp_cfg.to_json(j);
  j.start_array("cell-Group-Information");
  for (const auto& e1 : cell_group_info) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("qos-flow-Information-To-Be-Setup");
  for (const auto& e1 : qos_flow_info_to_be_setup) {
    e1.to_json(j);
  }
  j.end_array();
  if (drb_data_forwarding_info_request_present) {
    j.write_fieldname("dRB-Data-Forwarding-Information-Request");
    drb_data_forwarding_info_request.to_json(j);
  }
  if (drb_inactivity_timer_present) {
    j.write_int("dRB-Inactivity-Timer", drb_inactivity_timer);
  }
  if (pdcp_sn_status_info_present) {
    j.write_fieldname("pDCP-SN-Status-Information");
    pdcp_sn_status_info.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<drb_to_setup_mod_item_ng_ran_ext_ies_o>;

SRSASN_CODE drb_to_setup_mod_item_ng_ran_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += drb_qos_present ? 1 : 0;
  nof_ies += ignore_map_rule_ind_present ? 1 : 0;
  nof_ies += daps_request_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (drb_qos_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)72, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(drb_qos.pack(bref));
  }
  if (ignore_map_rule_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)138, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ignore_map_rule_ind.pack(bref));
  }
  if (daps_request_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)120, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(daps_request_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_mod_item_ng_ran_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 72: {
        drb_qos_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_qos.unpack(bref));
        break;
      }
      case 138: {
        ignore_map_rule_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ignore_map_rule_ind.unpack(bref));
        break;
      }
      case 120: {
        daps_request_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(daps_request_info.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void drb_to_setup_mod_item_ng_ran_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_qos_present) {
    j.write_int("id", 72);
    j.write_str("criticality", "ignore");
    drb_qos.to_json(j);
  }
  if (ignore_map_rule_ind_present) {
    j.write_int("id", 138);
    j.write_str("criticality", "reject");
    j.write_str("Extension", "true");
  }
  if (daps_request_info_present) {
    j.write_int("id", 120);
    j.write_str("criticality", "ignore");
    daps_request_info.to_json(j);
  }
  j.end_obj();
}

// DRB-To-Setup-Mod-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_to_setup_mod_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(sdap_cfg.pack(bref));
  HANDLE_CODE(pdcp_cfg.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_info, 1, 4, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, flow_map_info, 1, 64, true));
  if (drb_data_forwarding_info_request_present) {
    HANDLE_CODE(drb_data_forwarding_info_request.pack(bref));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, drb_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_mod_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.unpack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(sdap_cfg.unpack(bref));
  HANDLE_CODE(pdcp_cfg.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(cell_group_info, bref, 1, 4, true));
  HANDLE_CODE(unpack_dyn_seq_of(flow_map_info, bref, 1, 64, true));
  if (drb_data_forwarding_info_request_present) {
    HANDLE_CODE(drb_data_forwarding_info_request.unpack(bref));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(drb_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_to_setup_mod_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("sDAP-Configuration");
  sdap_cfg.to_json(j);
  j.write_fieldname("pDCP-Configuration");
  pdcp_cfg.to_json(j);
  j.start_array("cell-Group-Information");
  for (const auto& e1 : cell_group_info) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("flow-Mapping-Information");
  for (const auto& e1 : flow_map_info) {
    e1.to_json(j);
  }
  j.end_array();
  if (drb_data_forwarding_info_request_present) {
    j.write_fieldname("dRB-Data-Forwarding-Information-Request");
    drb_data_forwarding_info_request.to_json(j);
  }
  if (drb_inactivity_timer_present) {
    j.write_int("dRB-Inactivity-Timer", drb_inactivity_timer);
  }
  if (pdcp_sn_status_info_present) {
    j.write_fieldname("pDCP-SN-Status-Information");
    pdcp_sn_status_info.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// EUTRANAllocationAndRetentionPriority ::= SEQUENCE
SRSASN_CODE eutran_alloc_and_retention_prio_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, prio_level, (uint8_t)0u, (uint8_t)15u, false, true));
  HANDLE_CODE(pre_emption_cap.pack(bref));
  HANDLE_CODE(pre_emption_vulnerability.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_alloc_and_retention_prio_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(prio_level, bref, (uint8_t)0u, (uint8_t)15u, false, true));
  HANDLE_CODE(pre_emption_cap.unpack(bref));
  HANDLE_CODE(pre_emption_vulnerability.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void eutran_alloc_and_retention_prio_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("priorityLevel", prio_level);
  j.write_str("pre-emptionCapability", pre_emption_cap.to_string());
  j.write_str("pre-emptionVulnerability", pre_emption_vulnerability.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GBR-QosInformation ::= SEQUENCE
SRSASN_CODE gbr_qos_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, erab_max_bitrate_dl, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(pack_integer(bref, erab_max_bitrate_ul, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(pack_integer(bref, erab_guaranteed_bitrate_dl, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(pack_integer(bref, erab_guaranteed_bitrate_ul, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gbr_qos_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(erab_max_bitrate_dl, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(unpack_integer(erab_max_bitrate_ul, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(unpack_integer(erab_guaranteed_bitrate_dl, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(unpack_integer(erab_guaranteed_bitrate_ul, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gbr_qos_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("e-RAB-MaximumBitrateDL", erab_max_bitrate_dl);
  j.write_int("e-RAB-MaximumBitrateUL", erab_max_bitrate_ul);
  j.write_int("e-RAB-GuaranteedBitrateDL", erab_guaranteed_bitrate_dl);
  j.write_int("e-RAB-GuaranteedBitrateUL", erab_guaranteed_bitrate_ul);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// SNSSAI ::= SEQUENCE
SRSASN_CODE snssai_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sd_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(sst.pack(bref));
  if (sd_present) {
    HANDLE_CODE(sd.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE snssai_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sd_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(sst.unpack(bref));
  if (sd_present) {
    HANDLE_CODE(sd.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void snssai_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sST", sst.to_string());
  if (sd_present) {
    j.write_str("sD", sd.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// SecurityIndication ::= SEQUENCE
SRSASN_CODE security_ind_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(max_ip_datarate_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(integrity_protection_ind.pack(bref));
  HANDLE_CODE(confidentiality_protection_ind.pack(bref));
  if (max_ip_datarate_present) {
    HANDLE_CODE(max_ip_datarate.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_ind_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(max_ip_datarate_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(integrity_protection_ind.unpack(bref));
  HANDLE_CODE(confidentiality_protection_ind.unpack(bref));
  if (max_ip_datarate_present) {
    HANDLE_CODE(max_ip_datarate.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_ind_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("integrityProtectionIndication", integrity_protection_ind.to_string());
  j.write_str("confidentialityProtectionIndication", confidentiality_protection_ind.to_string());
  if (max_ip_datarate_present) {
    j.write_fieldname("maximumIPdatarate");
    max_ip_datarate.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-To-Setup-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_to_setup_mod_item_eutran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {142};
  return map_enum_number(names, 1, idx, "id");
}
bool drb_to_setup_mod_item_eutran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 142 == id;
}
crit_e drb_to_setup_mod_item_eutran_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 142) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drb_to_setup_mod_item_eutran_ext_ies_o::ext_c drb_to_setup_mod_item_eutran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 142) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_to_setup_mod_item_eutran_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 142) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void drb_to_setup_mod_item_eutran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("BIT STRING", c.to_string());
  j.end_obj();
}
SRSASN_CODE drb_to_setup_mod_item_eutran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_mod_item_eutran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* drb_to_setup_mod_item_eutran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING"};
  return convert_enum_idx(names, 1, value, "drb_to_setup_mod_item_eutran_ext_ies_o::ext_c::types");
}

// EUTRAN-QoS ::= SEQUENCE
SRSASN_CODE eutran_qos_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gbr_qos_info_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qci, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(eutran_alloc_and_retention_prio.pack(bref));
  if (gbr_qos_info_present) {
    HANDLE_CODE(gbr_qos_info.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_qos_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gbr_qos_info_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qci, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(eutran_alloc_and_retention_prio.unpack(bref));
  if (gbr_qos_info_present) {
    HANDLE_CODE(gbr_qos_info.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void eutran_qos_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qCI", qci);
  j.write_fieldname("eUTRANallocationAndRetentionPriority");
  eutran_alloc_and_retention_prio.to_json(j);
  if (gbr_qos_info_present) {
    j.write_fieldname("gbrQosInformation");
    gbr_qos_info.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_to_modify_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {69, 78, 97, 96, 131, 143};
  return map_enum_number(names, 6, idx, "id");
}
bool pdu_session_res_to_modify_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {69, 78, 97, 96, 131, 143};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e pdu_session_res_to_modify_item_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 69:
      return crit_e::reject;
    case 78:
      return crit_e::ignore;
    case 97:
      return crit_e::ignore;
    case 96:
      return crit_e::ignore;
    case 131:
      return crit_e::ignore;
    case 143:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
pdu_session_res_to_modify_item_ext_ies_o::ext_c pdu_session_res_to_modify_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 69:
      ret.set(ext_c::types::snssai);
      break;
    case 78:
      ret.set(ext_c::types::common_network_instance);
      break;
    case 97:
      ret.set(ext_c::types::redundant_n_g_ul_up_tnl_info);
      break;
    case 96:
      ret.set(ext_c::types::redundant_common_network_instance);
      break;
    case 131:
      ret.set(ext_c::types::data_forwardingto_e_utran_info_list);
      break;
    case 143:
      ret.set(ext_c::types::security_ind_modify);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_to_modify_item_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 69:
      return presence_e::optional;
    case 78:
      return presence_e::optional;
    case 97:
      return presence_e::optional;
    case 96:
      return presence_e::optional;
    case 131:
      return presence_e::optional;
    case 143:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_to_modify_item_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::snssai:
      c = snssai_s{};
      break;
    case types::common_network_instance:
      c = unbounded_octstring<true>{};
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      c = up_tnl_info_c{};
      break;
    case types::redundant_common_network_instance:
      c = unbounded_octstring<true>{};
      break;
    case types::data_forwardingto_e_utran_info_list:
      c = data_forwardingto_e_utran_info_list_l{};
      break;
    case types::security_ind_modify:
      c = security_ind_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_modify_item_ext_ies_o::ext_c");
  }
}
snssai_s& pdu_session_res_to_modify_item_ext_ies_o::ext_c::snssai()
{
  assert_choice_type(types::snssai, type_, "Extension");
  return c.get<snssai_s>();
}
unbounded_octstring<true>& pdu_session_res_to_modify_item_ext_ies_o::ext_c::common_network_instance()
{
  assert_choice_type(types::common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
up_tnl_info_c& pdu_session_res_to_modify_item_ext_ies_o::ext_c::redundant_n_g_ul_up_tnl_info()
{
  assert_choice_type(types::redundant_n_g_ul_up_tnl_info, type_, "Extension");
  return c.get<up_tnl_info_c>();
}
unbounded_octstring<true>& pdu_session_res_to_modify_item_ext_ies_o::ext_c::redundant_common_network_instance()
{
  assert_choice_type(types::redundant_common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
data_forwardingto_e_utran_info_list_l&
pdu_session_res_to_modify_item_ext_ies_o::ext_c::data_forwardingto_e_utran_info_list()
{
  assert_choice_type(types::data_forwardingto_e_utran_info_list, type_, "Extension");
  return c.get<data_forwardingto_e_utran_info_list_l>();
}
security_ind_s& pdu_session_res_to_modify_item_ext_ies_o::ext_c::security_ind_modify()
{
  assert_choice_type(types::security_ind_modify, type_, "Extension");
  return c.get<security_ind_s>();
}
const snssai_s& pdu_session_res_to_modify_item_ext_ies_o::ext_c::snssai() const
{
  assert_choice_type(types::snssai, type_, "Extension");
  return c.get<snssai_s>();
}
const unbounded_octstring<true>& pdu_session_res_to_modify_item_ext_ies_o::ext_c::common_network_instance() const
{
  assert_choice_type(types::common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
const up_tnl_info_c& pdu_session_res_to_modify_item_ext_ies_o::ext_c::redundant_n_g_ul_up_tnl_info() const
{
  assert_choice_type(types::redundant_n_g_ul_up_tnl_info, type_, "Extension");
  return c.get<up_tnl_info_c>();
}
const unbounded_octstring<true>&
pdu_session_res_to_modify_item_ext_ies_o::ext_c::redundant_common_network_instance() const
{
  assert_choice_type(types::redundant_common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
const data_forwardingto_e_utran_info_list_l&
pdu_session_res_to_modify_item_ext_ies_o::ext_c::data_forwardingto_e_utran_info_list() const
{
  assert_choice_type(types::data_forwardingto_e_utran_info_list, type_, "Extension");
  return c.get<data_forwardingto_e_utran_info_list_l>();
}
const security_ind_s& pdu_session_res_to_modify_item_ext_ies_o::ext_c::security_ind_modify() const
{
  assert_choice_type(types::security_ind_modify, type_, "Extension");
  return c.get<security_ind_s>();
}
void pdu_session_res_to_modify_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::snssai:
      j.write_fieldname("SNSSAI");
      c.get<snssai_s>().to_json(j);
      break;
    case types::common_network_instance:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      j.write_fieldname("UP-TNL-Information");
      c.get<up_tnl_info_c>().to_json(j);
      break;
    case types::redundant_common_network_instance:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::data_forwardingto_e_utran_info_list:
      j.start_array("DataForwardingtoE-UTRANInformationList");
      for (const auto& e1 : c.get<data_forwardingto_e_utran_info_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::security_ind_modify:
      j.write_fieldname("SecurityIndication");
      c.get<security_ind_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_modify_item_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE pdu_session_res_to_modify_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::snssai:
      HANDLE_CODE(c.get<snssai_s>().pack(bref));
      break;
    case types::common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      HANDLE_CODE(c.get<up_tnl_info_c>().pack(bref));
      break;
    case types::redundant_common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::data_forwardingto_e_utran_info_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<data_forwardingto_e_utran_info_list_l>(), 1, 256, true));
      break;
    case types::security_ind_modify:
      HANDLE_CODE(c.get<security_ind_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_modify_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_modify_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::snssai:
      HANDLE_CODE(c.get<snssai_s>().unpack(bref));
      break;
    case types::common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      HANDLE_CODE(c.get<up_tnl_info_c>().unpack(bref));
      break;
    case types::redundant_common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::data_forwardingto_e_utran_info_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<data_forwardingto_e_utran_info_list_l>(), bref, 1, 256, true));
      break;
    case types::security_ind_modify:
      HANDLE_CODE(c.get<security_ind_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_modify_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_to_modify_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SNSSAI",
                                "OCTET STRING",
                                "UP-TNL-Information",
                                "OCTET STRING",
                                "DataForwardingtoE-UTRANInformationList",
                                "SecurityIndication"};
  return convert_enum_idx(names, 6, value, "pdu_session_res_to_modify_item_ext_ies_o::ext_c::types");
}

// PDU-Session-Resource-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_to_rem_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {0};
  return map_enum_number(names, 1, idx, "id");
}
bool pdu_session_res_to_rem_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 0 == id;
}
crit_e pdu_session_res_to_rem_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 0) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
pdu_session_res_to_rem_item_ext_ies_o::ext_c pdu_session_res_to_rem_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 0) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_to_rem_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 0) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_to_rem_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cause");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE pdu_session_res_to_rem_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_rem_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_to_rem_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"Cause"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_to_rem_item_ext_ies_o::ext_c::types");
}

// PDU-Session-Resource-To-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_to_setup_mod_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {79, 78, 97, 96};
  return map_enum_number(names, 4, idx, "id");
}
bool pdu_session_res_to_setup_mod_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {79, 78, 97, 96};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e pdu_session_res_to_setup_mod_item_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 79:
      return crit_e::ignore;
    case 78:
      return crit_e::ignore;
    case 97:
      return crit_e::ignore;
    case 96:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c
pdu_session_res_to_setup_mod_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 79:
      ret.set(ext_c::types::network_instance);
      break;
    case 78:
      ret.set(ext_c::types::common_network_instance);
      break;
    case 97:
      ret.set(ext_c::types::redundant_n_g_ul_up_tnl_info);
      break;
    case 96:
      ret.set(ext_c::types::redundant_common_network_instance);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_to_setup_mod_item_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 79:
      return presence_e::optional;
    case 78:
      return presence_e::optional;
    case 97:
      return presence_e::optional;
    case 96:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::network_instance:
      c = uint16_t{};
      break;
    case types::common_network_instance:
      c = unbounded_octstring<true>{};
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      c = up_tnl_info_c{};
      break;
    case types::redundant_common_network_instance:
      c = unbounded_octstring<true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c");
  }
}
uint16_t& pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::network_instance()
{
  assert_choice_type(types::network_instance, type_, "Extension");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::common_network_instance()
{
  assert_choice_type(types::common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
up_tnl_info_c& pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::redundant_n_g_ul_up_tnl_info()
{
  assert_choice_type(types::redundant_n_g_ul_up_tnl_info, type_, "Extension");
  return c.get<up_tnl_info_c>();
}
unbounded_octstring<true>& pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::redundant_common_network_instance()
{
  assert_choice_type(types::redundant_common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
const uint16_t& pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::network_instance() const
{
  assert_choice_type(types::network_instance, type_, "Extension");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::common_network_instance() const
{
  assert_choice_type(types::common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
const up_tnl_info_c& pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::redundant_n_g_ul_up_tnl_info() const
{
  assert_choice_type(types::redundant_n_g_ul_up_tnl_info, type_, "Extension");
  return c.get<up_tnl_info_c>();
}
const unbounded_octstring<true>&
pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::redundant_common_network_instance() const
{
  assert_choice_type(types::redundant_common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
void pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::network_instance:
      j.write_int("INTEGER (1..256,...)", c.get<uint16_t>());
      break;
    case types::common_network_instance:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      j.write_fieldname("UP-TNL-Information");
      c.get<up_tnl_info_c>().to_json(j);
      break;
    case types::redundant_common_network_instance:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::network_instance:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)256u, true, true));
      break;
    case types::common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      HANDLE_CODE(c.get<up_tnl_info_c>().pack(bref));
      break;
    case types::redundant_common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::network_instance:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)256u, true, true));
      break;
    case types::common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      HANDLE_CODE(c.get<up_tnl_info_c>().unpack(bref));
      break;
    case types::redundant_common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..256,...)", "OCTET STRING", "UP-TNL-Information", "OCTET STRING"};
  return convert_enum_idx(names, 4, value, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::types");
}
uint8_t pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::types");
}

// PDU-Session-Type ::= ENUMERATED
const char* pdu_session_type_opts::to_string() const
{
  static const char* names[] = {"ipv4", "ipv6", "ipv4v6", "ethernet", "unstructured"};
  return convert_enum_idx(names, 5, value, "pdu_session_type_e");
}

// DRB-To-Modify-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_to_modify_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdcp_cfg_present, 1));
  HANDLE_CODE(bref.pack(eutran_qos_present, 1));
  HANDLE_CODE(bref.pack(s1_ul_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(data_forwarding_info_present, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_request_present, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.pack(dl_up_params.size() > 0, 1));
  HANDLE_CODE(bref.pack(cell_group_to_add.size() > 0, 1));
  HANDLE_CODE(bref.pack(cell_group_to_modify.size() > 0, 1));
  HANDLE_CODE(bref.pack(cell_group_to_rem.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.pack(bref));
  }
  if (eutran_qos_present) {
    HANDLE_CODE(eutran_qos.pack(bref));
  }
  if (s1_ul_up_tnl_info_present) {
    HANDLE_CODE(s1_ul_up_tnl_info.pack(bref));
  }
  if (data_forwarding_info_present) {
    HANDLE_CODE(data_forwarding_info.pack(bref));
  }
  if (pdcp_sn_status_request_present) {
    HANDLE_CODE(pdcp_sn_status_request.pack(bref));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.pack(bref));
  }
  if (dl_up_params.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_up_params, 1, 8, true));
  }
  if (cell_group_to_add.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_to_add, 1, 4, true));
  }
  if (cell_group_to_modify.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_to_modify, 1, 4, true));
  }
  if (cell_group_to_rem.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_to_rem, 1, 4, true));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, drb_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_modify_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdcp_cfg_present, 1));
  HANDLE_CODE(bref.unpack(eutran_qos_present, 1));
  HANDLE_CODE(bref.unpack(s1_ul_up_tnl_info_present, 1));
  HANDLE_CODE(bref.unpack(data_forwarding_info_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_request_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_info_present, 1));
  bool dl_up_params_present;
  HANDLE_CODE(bref.unpack(dl_up_params_present, 1));
  bool cell_group_to_add_present;
  HANDLE_CODE(bref.unpack(cell_group_to_add_present, 1));
  bool cell_group_to_modify_present;
  HANDLE_CODE(bref.unpack(cell_group_to_modify_present, 1));
  bool cell_group_to_rem_present;
  HANDLE_CODE(bref.unpack(cell_group_to_rem_present, 1));
  HANDLE_CODE(bref.unpack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.unpack(bref));
  }
  if (eutran_qos_present) {
    HANDLE_CODE(eutran_qos.unpack(bref));
  }
  if (s1_ul_up_tnl_info_present) {
    HANDLE_CODE(s1_ul_up_tnl_info.unpack(bref));
  }
  if (data_forwarding_info_present) {
    HANDLE_CODE(data_forwarding_info.unpack(bref));
  }
  if (pdcp_sn_status_request_present) {
    HANDLE_CODE(pdcp_sn_status_request.unpack(bref));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.unpack(bref));
  }
  if (dl_up_params_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dl_up_params, bref, 1, 8, true));
  }
  if (cell_group_to_add_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_group_to_add, bref, 1, 4, true));
  }
  if (cell_group_to_modify_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_group_to_modify, bref, 1, 4, true));
  }
  if (cell_group_to_rem_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_group_to_rem, bref, 1, 4, true));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(drb_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_to_modify_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (pdcp_cfg_present) {
    j.write_fieldname("pDCP-Configuration");
    pdcp_cfg.to_json(j);
  }
  if (eutran_qos_present) {
    j.write_fieldname("eUTRAN-QoS");
    eutran_qos.to_json(j);
  }
  if (s1_ul_up_tnl_info_present) {
    j.write_fieldname("s1-UL-UP-TNL-Information");
    s1_ul_up_tnl_info.to_json(j);
  }
  if (data_forwarding_info_present) {
    j.write_fieldname("data-Forwarding-Information");
    data_forwarding_info.to_json(j);
  }
  if (pdcp_sn_status_request_present) {
    j.write_str("pDCP-SN-Status-Request", "requested");
  }
  if (pdcp_sn_status_info_present) {
    j.write_fieldname("pDCP-SN-Status-Information");
    pdcp_sn_status_info.to_json(j);
  }
  if (dl_up_params.size() > 0) {
    j.start_array("dL-UP-Parameters");
    for (const auto& e1 : dl_up_params) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_group_to_add.size() > 0) {
    j.start_array("cell-Group-To-Add");
    for (const auto& e1 : cell_group_to_add) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_group_to_modify.size() > 0) {
    j.start_array("cell-Group-To-Modify");
    for (const auto& e1 : cell_group_to_modify) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_group_to_rem.size() > 0) {
    j.start_array("cell-Group-To-Remove");
    for (const auto& e1 : cell_group_to_rem) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_inactivity_timer_present) {
    j.write_int("dRB-Inactivity-Timer", drb_inactivity_timer);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-To-Remove-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_to_rem_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_rem_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_to_rem_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-To-Setup-Mod-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_to_setup_mod_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(dl_up_params.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(pdcp_cfg.pack(bref));
  HANDLE_CODE(eutran_qos.pack(bref));
  HANDLE_CODE(s1_ul_up_tnl_info.pack(bref));
  if (data_forwarding_info_request_present) {
    HANDLE_CODE(data_forwarding_info_request.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_info, 1, 4, true));
  if (dl_up_params.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_up_params, 1, 8, true));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, drb_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_mod_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_forwarding_info_request_present, 1));
  bool dl_up_params_present;
  HANDLE_CODE(bref.unpack(dl_up_params_present, 1));
  HANDLE_CODE(bref.unpack(drb_inactivity_timer_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(pdcp_cfg.unpack(bref));
  HANDLE_CODE(eutran_qos.unpack(bref));
  HANDLE_CODE(s1_ul_up_tnl_info.unpack(bref));
  if (data_forwarding_info_request_present) {
    HANDLE_CODE(data_forwarding_info_request.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(cell_group_info, bref, 1, 4, true));
  if (dl_up_params_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dl_up_params, bref, 1, 8, true));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(drb_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void drb_to_setup_mod_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("pDCP-Configuration");
  pdcp_cfg.to_json(j);
  j.write_fieldname("eUTRAN-QoS");
  eutran_qos.to_json(j);
  j.write_fieldname("s1-UL-UP-TNL-Information");
  s1_ul_up_tnl_info.to_json(j);
  if (data_forwarding_info_request_present) {
    j.write_fieldname("data-Forwarding-Information-Request");
    data_forwarding_info_request.to_json(j);
  }
  j.start_array("cell-Group-Information");
  for (const auto& e1 : cell_group_info) {
    e1.to_json(j);
  }
  j.end_array();
  if (dl_up_params.size() > 0) {
    j.start_array("dL-UP-Parameters");
    for (const auto& e1 : dl_up_params) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_inactivity_timer_present) {
    j.write_int("dRB-Inactivity-Timer", drb_inactivity_timer);
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<pdu_session_res_to_modify_item_ext_ies_o>;

SRSASN_CODE pdu_session_res_to_modify_item_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += snssai_present ? 1 : 0;
  nof_ies += common_network_instance_present ? 1 : 0;
  nof_ies += redundant_n_g_ul_up_tnl_info_present ? 1 : 0;
  nof_ies += redundant_common_network_instance_present ? 1 : 0;
  nof_ies += data_forwardingto_e_utran_info_list_present ? 1 : 0;
  nof_ies += security_ind_modify_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (snssai_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)69, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(snssai.pack(bref));
  }
  if (common_network_instance_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)78, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(common_network_instance.pack(bref));
  }
  if (redundant_n_g_ul_up_tnl_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)97, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_n_g_ul_up_tnl_info.pack(bref));
  }
  if (redundant_common_network_instance_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)96, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_common_network_instance.pack(bref));
  }
  if (data_forwardingto_e_utran_info_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)131, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, data_forwardingto_e_utran_info_list, 1, 256, true));
  }
  if (security_ind_modify_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)143, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(security_ind_modify.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_modify_item_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 69: {
        snssai_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(snssai.unpack(bref));
        break;
      }
      case 78: {
        common_network_instance_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(common_network_instance.unpack(bref));
        break;
      }
      case 97: {
        redundant_n_g_ul_up_tnl_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_n_g_ul_up_tnl_info.unpack(bref));
        break;
      }
      case 96: {
        redundant_common_network_instance_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_common_network_instance.unpack(bref));
        break;
      }
      case 131: {
        data_forwardingto_e_utran_info_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(data_forwardingto_e_utran_info_list, bref, 1, 256, true));
        break;
      }
      case 143: {
        security_ind_modify_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(security_ind_modify.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_to_modify_item_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (snssai_present) {
    j.write_int("id", 69);
    j.write_str("criticality", "reject");
    snssai.to_json(j);
  }
  if (common_network_instance_present) {
    j.write_int("id", 78);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", common_network_instance.to_string());
  }
  if (redundant_n_g_ul_up_tnl_info_present) {
    j.write_int("id", 97);
    j.write_str("criticality", "ignore");
    redundant_n_g_ul_up_tnl_info.to_json(j);
  }
  if (redundant_common_network_instance_present) {
    j.write_int("id", 96);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", redundant_common_network_instance.to_string());
  }
  if (data_forwardingto_e_utran_info_list_present) {
    j.write_int("id", 131);
    j.write_str("criticality", "ignore");
    j.start_array("Extension");
    for (const auto& e1 : data_forwardingto_e_utran_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (security_ind_modify_present) {
    j.write_int("id", 143);
    j.write_str("criticality", "ignore");
    security_ind_modify.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-To-Modify-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_to_modify_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(security_ind_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_res_dl_ambr_present, 1));
  HANDLE_CODE(bref.pack(ng_ul_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_data_forwarding_info_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(network_instance_present, 1));
  HANDLE_CODE(bref.pack(drb_to_setup_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_to_modify_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_to_rem_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  if (security_ind_present) {
    HANDLE_CODE(security_ind.pack(bref));
  }
  if (pdu_session_res_dl_ambr_present) {
    HANDLE_CODE(pack_integer(bref, pdu_session_res_dl_ambr, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (ng_ul_up_tnl_info_present) {
    HANDLE_CODE(ng_ul_up_tnl_info.pack(bref));
  }
  if (pdu_session_data_forwarding_info_request_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_request.pack(bref));
  }
  if (pdu_session_data_forwarding_info_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info.pack(bref));
  }
  if (pdu_session_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, pdu_session_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (network_instance_present) {
    HANDLE_CODE(pack_integer(bref, network_instance, (uint16_t)1u, (uint16_t)256u, true, true));
  }
  if (drb_to_setup_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_setup_list_ng_ran, 1, 32, true));
  }
  if (drb_to_modify_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_modify_list_ng_ran, 1, 32, true));
  }
  if (drb_to_rem_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_rem_list_ng_ran, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_modify_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(security_ind_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_res_dl_ambr_present, 1));
  HANDLE_CODE(bref.unpack(ng_ul_up_tnl_info_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_data_forwarding_info_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(network_instance_present, 1));
  bool drb_to_setup_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_to_setup_list_ng_ran_present, 1));
  bool drb_to_modify_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_to_modify_list_ng_ran_present, 1));
  bool drb_to_rem_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_to_rem_list_ng_ran_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (security_ind_present) {
    HANDLE_CODE(security_ind.unpack(bref));
  }
  if (pdu_session_res_dl_ambr_present) {
    HANDLE_CODE(unpack_integer(pdu_session_res_dl_ambr, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (ng_ul_up_tnl_info_present) {
    HANDLE_CODE(ng_ul_up_tnl_info.unpack(bref));
  }
  if (pdu_session_data_forwarding_info_request_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_request.unpack(bref));
  }
  if (pdu_session_data_forwarding_info_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info.unpack(bref));
  }
  if (pdu_session_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(pdu_session_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (network_instance_present) {
    HANDLE_CODE(unpack_integer(network_instance, bref, (uint16_t)1u, (uint16_t)256u, true, true));
  }
  if (drb_to_setup_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_setup_list_ng_ran, bref, 1, 32, true));
  }
  if (drb_to_modify_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_modify_list_ng_ran, bref, 1, 32, true));
  }
  if (drb_to_rem_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_rem_list_ng_ran, bref, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_to_modify_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  if (security_ind_present) {
    j.write_fieldname("securityIndication");
    security_ind.to_json(j);
  }
  if (pdu_session_res_dl_ambr_present) {
    j.write_int("pDU-Session-Resource-DL-AMBR", pdu_session_res_dl_ambr);
  }
  if (ng_ul_up_tnl_info_present) {
    j.write_fieldname("nG-UL-UP-TNL-Information");
    ng_ul_up_tnl_info.to_json(j);
  }
  if (pdu_session_data_forwarding_info_request_present) {
    j.write_fieldname("pDU-Session-Data-Forwarding-Information-Request");
    pdu_session_data_forwarding_info_request.to_json(j);
  }
  if (pdu_session_data_forwarding_info_present) {
    j.write_fieldname("pDU-Session-Data-Forwarding-Information");
    pdu_session_data_forwarding_info.to_json(j);
  }
  if (pdu_session_inactivity_timer_present) {
    j.write_int("pDU-Session-Inactivity-Timer", pdu_session_inactivity_timer);
  }
  if (network_instance_present) {
    j.write_int("networkInstance", network_instance);
  }
  if (drb_to_setup_list_ng_ran.size() > 0) {
    j.start_array("dRB-To-Setup-List-NG-RAN");
    for (const auto& e1 : drb_to_setup_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_modify_list_ng_ran.size() > 0) {
    j.start_array("dRB-To-Modify-List-NG-RAN");
    for (const auto& e1 : drb_to_modify_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_rem_list_ng_ran.size() > 0) {
    j.start_array("dRB-To-Remove-List-NG-RAN");
    for (const auto& e1 : drb_to_rem_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-To-Remove-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_to_rem_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_rem_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_to_rem_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<pdu_session_res_to_setup_mod_item_ext_ies_o>;

SRSASN_CODE pdu_session_res_to_setup_mod_item_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += network_instance_present ? 1 : 0;
  nof_ies += common_network_instance_present ? 1 : 0;
  nof_ies += redundant_n_g_ul_up_tnl_info_present ? 1 : 0;
  nof_ies += redundant_common_network_instance_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (network_instance_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)79, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, network_instance, (uint16_t)1u, (uint16_t)256u, true, true));
  }
  if (common_network_instance_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)78, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(common_network_instance.pack(bref));
  }
  if (redundant_n_g_ul_up_tnl_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)97, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_n_g_ul_up_tnl_info.pack(bref));
  }
  if (redundant_common_network_instance_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)96, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_common_network_instance.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_setup_mod_item_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 79: {
        network_instance_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(network_instance, bref, (uint16_t)1u, (uint16_t)256u, true, true));
        break;
      }
      case 78: {
        common_network_instance_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(common_network_instance.unpack(bref));
        break;
      }
      case 97: {
        redundant_n_g_ul_up_tnl_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_n_g_ul_up_tnl_info.unpack(bref));
        break;
      }
      case 96: {
        redundant_common_network_instance_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_common_network_instance.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_to_setup_mod_item_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (network_instance_present) {
    j.write_int("id", 79);
    j.write_str("criticality", "ignore");
    j.write_int("Extension", network_instance);
  }
  if (common_network_instance_present) {
    j.write_int("id", 78);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", common_network_instance.to_string());
  }
  if (redundant_n_g_ul_up_tnl_info_present) {
    j.write_int("id", 97);
    j.write_str("criticality", "ignore");
    redundant_n_g_ul_up_tnl_info.to_json(j);
  }
  if (redundant_common_network_instance_present) {
    j.write_int("id", 96);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", redundant_common_network_instance.to_string());
  }
  j.end_obj();
}

// PDU-Session-Resource-To-Setup-Mod-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_to_setup_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdu_session_res_ambr_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pdu_session_type.pack(bref));
  HANDLE_CODE(snssai.pack(bref));
  HANDLE_CODE(security_ind.pack(bref));
  if (pdu_session_res_ambr_present) {
    HANDLE_CODE(pack_integer(bref, pdu_session_res_ambr, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  HANDLE_CODE(ng_ul_up_tnl_info.pack(bref));
  if (pdu_session_data_forwarding_info_request_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_request.pack(bref));
  }
  if (pdu_session_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, pdu_session_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_setup_mod_list_ng_ran, 1, 32, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_setup_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdu_session_res_ambr_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pdu_session_type.unpack(bref));
  HANDLE_CODE(snssai.unpack(bref));
  HANDLE_CODE(security_ind.unpack(bref));
  if (pdu_session_res_ambr_present) {
    HANDLE_CODE(unpack_integer(pdu_session_res_ambr, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  HANDLE_CODE(ng_ul_up_tnl_info.unpack(bref));
  if (pdu_session_data_forwarding_info_request_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_request.unpack(bref));
  }
  if (pdu_session_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(pdu_session_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  HANDLE_CODE(unpack_dyn_seq_of(drb_to_setup_mod_list_ng_ran, bref, 1, 32, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_to_setup_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.write_str("pDU-Session-Type", pdu_session_type.to_string());
  j.write_fieldname("sNSSAI");
  snssai.to_json(j);
  j.write_fieldname("securityIndication");
  security_ind.to_json(j);
  if (pdu_session_res_ambr_present) {
    j.write_int("pDU-Session-Resource-AMBR", pdu_session_res_ambr);
  }
  j.write_fieldname("nG-UL-UP-TNL-Information");
  ng_ul_up_tnl_info.to_json(j);
  if (pdu_session_data_forwarding_info_request_present) {
    j.write_fieldname("pDU-Session-Data-Forwarding-Information-Request");
    pdu_session_data_forwarding_info_request.to_json(j);
  }
  if (pdu_session_inactivity_timer_present) {
    j.write_int("pDU-Session-Inactivity-Timer", pdu_session_inactivity_timer);
  }
  j.start_array("dRB-To-Setup-Mod-List-NG-RAN");
  for (const auto& e1 : drb_to_setup_mod_list_ng_ran) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// CipheringAlgorithm ::= ENUMERATED
const char* ciphering_algorithm_opts::to_string() const
{
  static const char* names[] = {"nEA0", "c-128-NEA1", "c-128-NEA2", "c-128-NEA3"};
  return convert_enum_idx(names, 4, value, "ciphering_algorithm_e");
}

// IntegrityProtectionAlgorithm ::= ENUMERATED
const char* integrity_protection_algorithm_opts::to_string() const
{
  static const char* names[] = {"nIA0", "i-128-NIA1", "i-128-NIA2", "i-128-NIA3"};
  return convert_enum_idx(names, 4, value, "integrity_protection_algorithm_e");
}

// SecurityAlgorithm ::= SEQUENCE
SRSASN_CODE security_algorithm_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(integrity_protection_algorithm_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(ciphering_algorithm.pack(bref));
  if (integrity_protection_algorithm_present) {
    HANDLE_CODE(integrity_protection_algorithm.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_algorithm_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(integrity_protection_algorithm_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(ciphering_algorithm.unpack(bref));
  if (integrity_protection_algorithm_present) {
    HANDLE_CODE(integrity_protection_algorithm.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_algorithm_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cipheringAlgorithm", ciphering_algorithm.to_string());
  if (integrity_protection_algorithm_present) {
    j.write_str("integrityProtectionAlgorithm", integrity_protection_algorithm.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// UPSecuritykey ::= SEQUENCE
SRSASN_CODE up_securitykey_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(integrity_protection_key.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(encryption_key.pack(bref));
  if (integrity_protection_key.size() > 0) {
    HANDLE_CODE(integrity_protection_key.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE up_securitykey_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool integrity_protection_key_present;
  HANDLE_CODE(bref.unpack(integrity_protection_key_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(encryption_key.unpack(bref));
  if (integrity_protection_key_present) {
    HANDLE_CODE(integrity_protection_key.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void up_securitykey_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("encryptionKey", encryption_key.to_string());
  if (integrity_protection_key.size() > 0) {
    j.write_str("integrityProtectionKey", integrity_protection_key.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// ActivityNotificationLevel ::= ENUMERATED
const char* activity_notif_level_opts::to_string() const
{
  static const char* names[] = {"drb", "pdu-session", "ue"};
  return convert_enum_idx(names, 3, value, "activity_notif_level_e");
}

// BearerContextStatusChange ::= ENUMERATED
const char* bearer_context_status_change_opts::to_string() const
{
  static const char* names[] = {"suspend", "resume"};
  return convert_enum_idx(names, 2, value, "bearer_context_status_change_e");
}

// DataDiscardRequired ::= ENUMERATED
const char* data_discard_required_opts::to_string() const
{
  static const char* names[] = {"required"};
  return convert_enum_idx(names, 1, value, "data_discard_required_e");
}

// New-UL-TNL-Information-Required ::= ENUMERATED
const char* new_ul_tnl_info_required_opts::to_string() const
{
  static const char* names[] = {"required"};
  return convert_enum_idx(names, 1, value, "new_ul_tnl_info_required_e");
}

// SecurityInformation ::= SEQUENCE
SRSASN_CODE security_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(security_algorithm.pack(bref));
  HANDLE_CODE(up_securitykey.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(security_algorithm.unpack(bref));
  HANDLE_CODE(up_securitykey.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("securityAlgorithm");
  security_algorithm.to_json(j);
  j.write_fieldname("uPSecuritykey");
  up_securitykey.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-UP-CellGroupRelatedConfiguration-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_up_cell_group_related_cfg_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_cfg_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, cell_group_id, (uint8_t)0u, (uint8_t)3u, true, true));
  HANDLE_CODE(up_tnl_info.pack(bref));
  if (ul_cfg_present) {
    HANDLE_CODE(ul_cfg.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_cell_group_related_cfg_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_cfg_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(cell_group_id, bref, (uint8_t)0u, (uint8_t)3u, true, true));
  HANDLE_CODE(up_tnl_info.unpack(bref));
  if (ul_cfg_present) {
    HANDLE_CODE(ul_cfg.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_up_cell_group_related_cfg_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cell-Group-ID", cell_group_id);
  j.write_fieldname("uP-TNL-Information");
  up_tnl_info.to_json(j);
  if (ul_cfg_present) {
    j.write_str("uL-Configuration", ul_cfg.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Required-To-Modify-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_required_to_modify_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gnb_cu_up_cell_group_related_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(flow_to_rem.size() > 0, 1));
  HANDLE_CODE(bref.pack(cause_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (gnb_cu_up_cell_group_related_cfg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_up_cell_group_related_cfg, 1, 8, true));
  }
  if (flow_to_rem.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, flow_to_rem, 1, 64, true));
  }
  if (cause_present) {
    HANDLE_CODE(cause.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_required_to_modify_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool gnb_cu_up_cell_group_related_cfg_present;
  HANDLE_CODE(bref.unpack(gnb_cu_up_cell_group_related_cfg_present, 1));
  bool flow_to_rem_present;
  HANDLE_CODE(bref.unpack(flow_to_rem_present, 1));
  HANDLE_CODE(bref.unpack(cause_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (gnb_cu_up_cell_group_related_cfg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_up_cell_group_related_cfg, bref, 1, 8, true));
  }
  if (flow_to_rem_present) {
    HANDLE_CODE(unpack_dyn_seq_of(flow_to_rem, bref, 1, 64, true));
  }
  if (cause_present) {
    HANDLE_CODE(cause.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_required_to_modify_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (gnb_cu_up_cell_group_related_cfg.size() > 0) {
    j.start_array("gNB-CU-UP-CellGroupRelatedConfiguration");
    for (const auto& e1 : gnb_cu_up_cell_group_related_cfg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (flow_to_rem.size() > 0) {
    j.start_array("flow-To-Remove");
    for (const auto& e1 : flow_to_rem) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cause_present) {
    j.write_fieldname("cause");
    cause.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Required-To-Remove-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_required_to_rem_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_required_to_rem_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_required_to_rem_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-Required-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_required_to_modify_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {98};
  return map_enum_number(names, 1, idx, "id");
}
bool pdu_session_res_required_to_modify_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 98 == id;
}
crit_e pdu_session_res_required_to_modify_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 98) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
pdu_session_res_required_to_modify_item_ext_ies_o::ext_c
pdu_session_res_required_to_modify_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 98) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_required_to_modify_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 98) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_required_to_modify_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UP-TNL-Information");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE pdu_session_res_required_to_modify_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_required_to_modify_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_required_to_modify_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UP-TNL-Information"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_required_to_modify_item_ext_ies_o::ext_c::types");
}

// DRB-Required-To-Modify-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_required_to_modify_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(s1_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_up_cell_group_related_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(cause_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (s1_dl_up_tnl_info_present) {
    HANDLE_CODE(s1_dl_up_tnl_info.pack(bref));
  }
  if (gnb_cu_up_cell_group_related_cfg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_up_cell_group_related_cfg, 1, 8, true));
  }
  if (cause_present) {
    HANDLE_CODE(cause.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_required_to_modify_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(s1_dl_up_tnl_info_present, 1));
  bool gnb_cu_up_cell_group_related_cfg_present;
  HANDLE_CODE(bref.unpack(gnb_cu_up_cell_group_related_cfg_present, 1));
  HANDLE_CODE(bref.unpack(cause_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (s1_dl_up_tnl_info_present) {
    HANDLE_CODE(s1_dl_up_tnl_info.unpack(bref));
  }
  if (gnb_cu_up_cell_group_related_cfg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_up_cell_group_related_cfg, bref, 1, 8, true));
  }
  if (cause_present) {
    HANDLE_CODE(cause.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_required_to_modify_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (s1_dl_up_tnl_info_present) {
    j.write_fieldname("s1-DL-UP-TNL-Information");
    s1_dl_up_tnl_info.to_json(j);
  }
  if (gnb_cu_up_cell_group_related_cfg.size() > 0) {
    j.start_array("gNB-CU-UP-CellGroupRelatedConfiguration");
    for (const auto& e1 : gnb_cu_up_cell_group_related_cfg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cause_present) {
    j.write_fieldname("cause");
    cause.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Required-To-Remove-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_required_to_rem_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_required_to_rem_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_required_to_rem_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-Required-To-Modify-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_required_to_modify_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ng_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(drb_required_to_modify_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_required_to_rem_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  if (ng_dl_up_tnl_info_present) {
    HANDLE_CODE(ng_dl_up_tnl_info.pack(bref));
  }
  if (drb_required_to_modify_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_required_to_modify_list_ng_ran, 1, 32, true));
  }
  if (drb_required_to_rem_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_required_to_rem_list_ng_ran, 1, 32, true));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_required_to_modify_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ng_dl_up_tnl_info_present, 1));
  bool drb_required_to_modify_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_required_to_modify_list_ng_ran_present, 1));
  bool drb_required_to_rem_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_required_to_rem_list_ng_ran_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (ng_dl_up_tnl_info_present) {
    HANDLE_CODE(ng_dl_up_tnl_info.unpack(bref));
  }
  if (drb_required_to_modify_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_required_to_modify_list_ng_ran, bref, 1, 32, true));
  }
  if (drb_required_to_rem_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_required_to_rem_list_ng_ran, bref, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_required_to_modify_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  if (ng_dl_up_tnl_info_present) {
    j.write_fieldname("nG-DL-UP-TNL-Information");
    ng_dl_up_tnl_info.to_json(j);
  }
  if (drb_required_to_modify_list_ng_ran.size() > 0) {
    j.start_array("dRB-Required-To-Modify-List-NG-RAN");
    for (const auto& e1 : drb_required_to_modify_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_required_to_rem_list_ng_ran.size() > 0) {
    j.start_array("dRB-Required-To-Remove-List-NG-RAN");
    for (const auto& e1 : drb_required_to_rem_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// QoS-Flow-Failed-Item ::= SEQUENCE
SRSASN_CODE qos_flow_failed_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_failed_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_flow_failed_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qos_flow_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Modified-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_modified_item_ng_ran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {123, 71};
  return map_enum_number(names, 2, idx, "id");
}
bool drb_modified_item_ng_ran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {123, 71};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e drb_modified_item_ng_ran_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 123:
      return crit_e::reject;
    case 71:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
drb_modified_item_ng_ran_ext_ies_o::ext_c drb_modified_item_ng_ran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 123:
      ret.set(ext_c::types::early_forwarding_count_info);
      break;
    case 71:
      ret.set(ext_c::types::old_qos_flow_map_ul_endmarkerexpected);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_modified_item_ng_ran_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 123:
      return presence_e::optional;
    case 71:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void drb_modified_item_ng_ran_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::early_forwarding_count_info:
      c = early_forwarding_count_info_c{};
      break;
    case types::old_qos_flow_map_ul_endmarkerexpected:
      c = qos_flow_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_modified_item_ng_ran_ext_ies_o::ext_c");
  }
}
early_forwarding_count_info_c& drb_modified_item_ng_ran_ext_ies_o::ext_c::early_forwarding_count_info()
{
  assert_choice_type(types::early_forwarding_count_info, type_, "Extension");
  return c.get<early_forwarding_count_info_c>();
}
qos_flow_list_l& drb_modified_item_ng_ran_ext_ies_o::ext_c::old_qos_flow_map_ul_endmarkerexpected()
{
  assert_choice_type(types::old_qos_flow_map_ul_endmarkerexpected, type_, "Extension");
  return c.get<qos_flow_list_l>();
}
const early_forwarding_count_info_c& drb_modified_item_ng_ran_ext_ies_o::ext_c::early_forwarding_count_info() const
{
  assert_choice_type(types::early_forwarding_count_info, type_, "Extension");
  return c.get<early_forwarding_count_info_c>();
}
const qos_flow_list_l& drb_modified_item_ng_ran_ext_ies_o::ext_c::old_qos_flow_map_ul_endmarkerexpected() const
{
  assert_choice_type(types::old_qos_flow_map_ul_endmarkerexpected, type_, "Extension");
  return c.get<qos_flow_list_l>();
}
void drb_modified_item_ng_ran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::early_forwarding_count_info:
      j.write_fieldname("EarlyForwardingCOUNTInfo");
      c.get<early_forwarding_count_info_c>().to_json(j);
      break;
    case types::old_qos_flow_map_ul_endmarkerexpected:
      j.start_array("QoS-Flow-List");
      for (const auto& e1 : c.get<qos_flow_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "drb_modified_item_ng_ran_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE drb_modified_item_ng_ran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::early_forwarding_count_info:
      HANDLE_CODE(c.get<early_forwarding_count_info_c>().pack(bref));
      break;
    case types::old_qos_flow_map_ul_endmarkerexpected:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<qos_flow_list_l>(), 1, 64, true));
      break;
    default:
      log_invalid_choice_id(type_, "drb_modified_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_modified_item_ng_ran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::early_forwarding_count_info:
      HANDLE_CODE(c.get<early_forwarding_count_info_c>().unpack(bref));
      break;
    case types::old_qos_flow_map_ul_endmarkerexpected:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<qos_flow_list_l>(), bref, 1, 64, true));
      break;
    default:
      log_invalid_choice_id(type_, "drb_modified_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drb_modified_item_ng_ran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"EarlyForwardingCOUNTInfo", "QoS-Flow-List"};
  return convert_enum_idx(names, 2, value, "drb_modified_item_ng_ran_ext_ies_o::ext_c::types");
}

// ConfidentialityProtectionResult ::= ENUMERATED
const char* confidentiality_protection_result_opts::to_string() const
{
  static const char* names[] = {"performed", "not-performed"};
  return convert_enum_idx(names, 2, value, "confidentiality_protection_result_e");
}

// DRB-Failed-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_failed_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_failed_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_failed_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Failed-Mod-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_failed_mod_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_failed_mod_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_failed_mod_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Failed-To-Modify-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_failed_to_modify_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_failed_to_modify_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_failed_to_modify_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<drb_modified_item_ng_ran_ext_ies_o>;

SRSASN_CODE drb_modified_item_ng_ran_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += early_forwarding_count_info_present ? 1 : 0;
  nof_ies += old_qos_flow_map_ul_endmarkerexpected_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (early_forwarding_count_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)123, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(early_forwarding_count_info.pack(bref));
  }
  if (old_qos_flow_map_ul_endmarkerexpected_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)71, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, old_qos_flow_map_ul_endmarkerexpected, 1, 64, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_modified_item_ng_ran_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 123: {
        early_forwarding_count_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(early_forwarding_count_info.unpack(bref));
        break;
      }
      case 71: {
        old_qos_flow_map_ul_endmarkerexpected_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(old_qos_flow_map_ul_endmarkerexpected, bref, 1, 64, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void drb_modified_item_ng_ran_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (early_forwarding_count_info_present) {
    j.write_int("id", 123);
    j.write_str("criticality", "reject");
    early_forwarding_count_info.to_json(j);
  }
  if (old_qos_flow_map_ul_endmarkerexpected_present) {
    j.write_int("id", 71);
    j.write_str("criticality", "ignore");
    j.start_array("Extension");
    for (const auto& e1 : old_qos_flow_map_ul_endmarkerexpected) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// DRB-Modified-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_modified_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_up_transport_params.size() > 0, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.pack(flow_setup_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(flow_failed_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (ul_up_transport_params.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_transport_params, 1, 8, true));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.pack(bref));
  }
  if (flow_setup_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, flow_setup_list, 1, 64, true));
  }
  if (flow_failed_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, flow_failed_list, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_modified_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ul_up_transport_params_present;
  HANDLE_CODE(bref.unpack(ul_up_transport_params_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_info_present, 1));
  bool flow_setup_list_present;
  HANDLE_CODE(bref.unpack(flow_setup_list_present, 1));
  bool flow_failed_list_present;
  HANDLE_CODE(bref.unpack(flow_failed_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (ul_up_transport_params_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_up_transport_params, bref, 1, 8, true));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.unpack(bref));
  }
  if (flow_setup_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(flow_setup_list, bref, 1, 64, true));
  }
  if (flow_failed_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(flow_failed_list, bref, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_modified_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (ul_up_transport_params.size() > 0) {
    j.start_array("uL-UP-Transport-Parameters");
    for (const auto& e1 : ul_up_transport_params) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdcp_sn_status_info_present) {
    j.write_fieldname("pDCP-SN-Status-Information");
    pdcp_sn_status_info.to_json(j);
  }
  if (flow_setup_list.size() > 0) {
    j.start_array("flow-Setup-List");
    for (const auto& e1 : flow_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (flow_failed_list.size() > 0) {
    j.start_array("flow-Failed-List");
    for (const auto& e1 : flow_failed_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Setup-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_setup_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.pack(flow_failed_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (drb_data_forwarding_info_resp_present) {
    HANDLE_CODE(drb_data_forwarding_info_resp.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_transport_params, 1, 8, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, flow_setup_list, 1, 64, true));
  if (flow_failed_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, flow_failed_list, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_setup_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_data_forwarding_info_resp_present, 1));
  bool flow_failed_list_present;
  HANDLE_CODE(bref.unpack(flow_failed_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (drb_data_forwarding_info_resp_present) {
    HANDLE_CODE(drb_data_forwarding_info_resp.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ul_up_transport_params, bref, 1, 8, true));
  HANDLE_CODE(unpack_dyn_seq_of(flow_setup_list, bref, 1, 64, true));
  if (flow_failed_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(flow_failed_list, bref, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_setup_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (drb_data_forwarding_info_resp_present) {
    j.write_fieldname("dRB-data-Forwarding-Information-Response");
    drb_data_forwarding_info_resp.to_json(j);
  }
  j.start_array("uL-UP-Transport-Parameters");
  for (const auto& e1 : ul_up_transport_params) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("flow-Setup-List");
  for (const auto& e1 : flow_setup_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (flow_failed_list.size() > 0) {
    j.start_array("flow-Failed-List");
    for (const auto& e1 : flow_failed_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Setup-Mod-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_setup_mod_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.pack(flow_failed_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (drb_data_forwarding_info_resp_present) {
    HANDLE_CODE(drb_data_forwarding_info_resp.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_transport_params, 1, 8, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, flow_setup_list, 1, 64, true));
  if (flow_failed_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, flow_failed_list, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_setup_mod_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_data_forwarding_info_resp_present, 1));
  bool flow_failed_list_present;
  HANDLE_CODE(bref.unpack(flow_failed_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (drb_data_forwarding_info_resp_present) {
    HANDLE_CODE(drb_data_forwarding_info_resp.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ul_up_transport_params, bref, 1, 8, true));
  HANDLE_CODE(unpack_dyn_seq_of(flow_setup_list, bref, 1, 64, true));
  if (flow_failed_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(flow_failed_list, bref, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_setup_mod_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (drb_data_forwarding_info_resp_present) {
    j.write_fieldname("dRB-data-Forwarding-Information-Response");
    drb_data_forwarding_info_resp.to_json(j);
  }
  j.start_array("uL-UP-Transport-Parameters");
  for (const auto& e1 : ul_up_transport_params) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("flow-Setup-List");
  for (const auto& e1 : flow_setup_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (flow_failed_list.size() > 0) {
    j.start_array("flow-Failed-List");
    for (const auto& e1 : flow_failed_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// IntegrityProtectionResult ::= ENUMERATED
const char* integrity_protection_result_opts::to_string() const
{
  static const char* names[] = {"performed", "not-performed"};
  return convert_enum_idx(names, 2, value, "integrity_protection_result_e");
}

// DRB-Setup-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_setup_mod_item_eutran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {142};
  return map_enum_number(names, 1, idx, "id");
}
bool drb_setup_mod_item_eutran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 142 == id;
}
crit_e drb_setup_mod_item_eutran_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 142) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drb_setup_mod_item_eutran_ext_ies_o::ext_c drb_setup_mod_item_eutran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 142) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_setup_mod_item_eutran_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 142) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void drb_setup_mod_item_eutran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("BIT STRING", c.to_string());
  j.end_obj();
}
SRSASN_CODE drb_setup_mod_item_eutran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_setup_mod_item_eutran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* drb_setup_mod_item_eutran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING"};
  return convert_enum_idx(names, 1, value, "drb_setup_mod_item_eutran_ext_ies_o::ext_c::types");
}

// PDU-Session-Resource-Modified-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_modified_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {98};
  return map_enum_number(names, 1, idx, "id");
}
bool pdu_session_res_modified_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 98 == id;
}
crit_e pdu_session_res_modified_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 98) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
pdu_session_res_modified_item_ext_ies_o::ext_c pdu_session_res_modified_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 98) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_modified_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 98) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_modified_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UP-TNL-Information");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE pdu_session_res_modified_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_modified_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_modified_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UP-TNL-Information"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_modified_item_ext_ies_o::ext_c::types");
}

// PDU-Session-Resource-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_setup_mod_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {98};
  return map_enum_number(names, 1, idx, "id");
}
bool pdu_session_res_setup_mod_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 98 == id;
}
crit_e pdu_session_res_setup_mod_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 98) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
pdu_session_res_setup_mod_item_ext_ies_o::ext_c pdu_session_res_setup_mod_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 98) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_setup_mod_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 98) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_setup_mod_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UP-TNL-Information");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE pdu_session_res_setup_mod_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_setup_mod_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_setup_mod_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UP-TNL-Information"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_setup_mod_item_ext_ies_o::ext_c::types");
}

// QoS-Flow-Removed-Item ::= SEQUENCE
SRSASN_CODE qos_flow_remd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qos_flow_released_in_session_present, 1));
  HANDLE_CODE(bref.pack(qos_flow_accumulated_session_time_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  if (qos_flow_released_in_session_present) {
    HANDLE_CODE(qos_flow_released_in_session.pack(bref));
  }
  if (qos_flow_accumulated_session_time_present) {
    HANDLE_CODE(qos_flow_accumulated_session_time.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_flow_remd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(qos_flow_released_in_session_present, 1));
  HANDLE_CODE(bref.unpack(qos_flow_accumulated_session_time_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  if (qos_flow_released_in_session_present) {
    HANDLE_CODE(qos_flow_released_in_session.unpack(bref));
  }
  if (qos_flow_accumulated_session_time_present) {
    HANDLE_CODE(qos_flow_accumulated_session_time.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_flow_remd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qos_flow_id);
  if (qos_flow_released_in_session_present) {
    j.write_str("qoS-Flow-Released-In-Session", qos_flow_released_in_session.to_string());
  }
  if (qos_flow_accumulated_session_time_present) {
    j.write_str("qoS-Flow-Accumulated-Session-Time", qos_flow_accumulated_session_time.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* qos_flow_remd_item_s::qos_flow_released_in_session_opts::to_string() const
{
  static const char* names[] = {"released-in-session", "not-released-in-session"};
  return convert_enum_idx(names, 2, value, "qos_flow_remd_item_s::qos_flow_released_in_session_e_");
}

// SecurityResult ::= SEQUENCE
SRSASN_CODE security_result_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(integrity_protection_result.pack(bref));
  HANDLE_CODE(confidentiality_protection_result.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_result_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(integrity_protection_result.unpack(bref));
  HANDLE_CODE(confidentiality_protection_result.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void security_result_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("integrityProtectionResult", integrity_protection_result.to_string());
  j.write_str("confidentialityProtectionResult", confidentiality_protection_result.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Failed-Mod-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_failed_mod_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_failed_mod_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_failed_mod_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Failed-To-Modify-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_failed_to_modify_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_failed_to_modify_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_failed_to_modify_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Modified-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_modified_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(s1_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.pack(ul_up_transport_params.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (s1_dl_up_tnl_info_present) {
    HANDLE_CODE(s1_dl_up_tnl_info.pack(bref));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.pack(bref));
  }
  if (ul_up_transport_params.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_transport_params, 1, 8, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_modified_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(s1_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_info_present, 1));
  bool ul_up_transport_params_present;
  HANDLE_CODE(bref.unpack(ul_up_transport_params_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (s1_dl_up_tnl_info_present) {
    HANDLE_CODE(s1_dl_up_tnl_info.unpack(bref));
  }
  if (pdcp_sn_status_info_present) {
    HANDLE_CODE(pdcp_sn_status_info.unpack(bref));
  }
  if (ul_up_transport_params_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_up_transport_params, bref, 1, 8, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_modified_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (s1_dl_up_tnl_info_present) {
    j.write_fieldname("s1-DL-UP-TNL-Information");
    s1_dl_up_tnl_info.to_json(j);
  }
  if (pdcp_sn_status_info_present) {
    j.write_fieldname("pDCP-SN-Status-Information");
    pdcp_sn_status_info.to_json(j);
  }
  if (ul_up_transport_params.size() > 0) {
    j.start_array("uL-UP-Transport-Parameters");
    for (const auto& e1 : ul_up_transport_params) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Removed-Item ::= SEQUENCE
SRSASN_CODE drb_remd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_released_in_session_present, 1));
  HANDLE_CODE(bref.pack(drb_accumulated_session_time_present, 1));
  HANDLE_CODE(bref.pack(qos_flow_remd_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (drb_released_in_session_present) {
    HANDLE_CODE(drb_released_in_session.pack(bref));
  }
  if (drb_accumulated_session_time_present) {
    HANDLE_CODE(drb_accumulated_session_time.pack(bref));
  }
  if (qos_flow_remd_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, qos_flow_remd_list, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_remd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_released_in_session_present, 1));
  HANDLE_CODE(bref.unpack(drb_accumulated_session_time_present, 1));
  bool qos_flow_remd_list_present;
  HANDLE_CODE(bref.unpack(qos_flow_remd_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (drb_released_in_session_present) {
    HANDLE_CODE(drb_released_in_session.unpack(bref));
  }
  if (drb_accumulated_session_time_present) {
    HANDLE_CODE(drb_accumulated_session_time.unpack(bref));
  }
  if (qos_flow_remd_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(qos_flow_remd_list, bref, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_remd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (drb_released_in_session_present) {
    j.write_str("dRB-Released-In-Session", drb_released_in_session.to_string());
  }
  if (drb_accumulated_session_time_present) {
    j.write_str("dRB-Accumulated-Session-Time", drb_accumulated_session_time.to_string());
  }
  if (qos_flow_remd_list.size() > 0) {
    j.start_array("qoS-Flow-Removed-List");
    for (const auto& e1 : qos_flow_remd_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* drb_remd_item_s::drb_released_in_session_opts::to_string() const
{
  static const char* names[] = {"released-in-session", "not-released-in-session"};
  return convert_enum_idx(names, 2, value, "drb_remd_item_s::drb_released_in_session_e_");
}

// DRB-Setup-Mod-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_setup_mod_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(s1_dl_up_tnl_info.pack(bref));
  if (data_forwarding_info_resp_present) {
    HANDLE_CODE(data_forwarding_info_resp.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_transport_params, 1, 8, true));
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_setup_mod_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_forwarding_info_resp_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(s1_dl_up_tnl_info.unpack(bref));
  if (data_forwarding_info_resp_present) {
    HANDLE_CODE(data_forwarding_info_resp.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ul_up_transport_params, bref, 1, 8, true));
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void drb_setup_mod_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("s1-DL-UP-TNL-Information");
  s1_dl_up_tnl_info.to_json(j);
  if (data_forwarding_info_resp_present) {
    j.write_fieldname("data-Forwarding-Information-Response");
    data_forwarding_info_resp.to_json(j);
  }
  j.start_array("uL-UP-Transport-Parameters");
  for (const auto& e1 : ul_up_transport_params) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// PDU-Session-Resource-Failed-Mod-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_failed_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_failed_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_failed_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-Failed-To-Modify-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_failed_to_modify_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_failed_to_modify_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_failed_to_modify_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-Modified-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_modified_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ng_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(security_result_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.pack(drb_setup_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_failed_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_modified_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_failed_to_modify_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  if (ng_dl_up_tnl_info_present) {
    HANDLE_CODE(ng_dl_up_tnl_info.pack(bref));
  }
  if (security_result_present) {
    HANDLE_CODE(security_result.pack(bref));
  }
  if (pdu_session_data_forwarding_info_resp_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_resp.pack(bref));
  }
  if (drb_setup_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_setup_list_ng_ran, 1, 32, true));
  }
  if (drb_failed_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_failed_list_ng_ran, 1, 32, true));
  }
  if (drb_modified_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_modified_list_ng_ran, 1, 32, true));
  }
  if (drb_failed_to_modify_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_failed_to_modify_list_ng_ran, 1, 32, true));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_modified_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ng_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.unpack(security_result_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_data_forwarding_info_resp_present, 1));
  bool drb_setup_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_setup_list_ng_ran_present, 1));
  bool drb_failed_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_failed_list_ng_ran_present, 1));
  bool drb_modified_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_modified_list_ng_ran_present, 1));
  bool drb_failed_to_modify_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_failed_to_modify_list_ng_ran_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (ng_dl_up_tnl_info_present) {
    HANDLE_CODE(ng_dl_up_tnl_info.unpack(bref));
  }
  if (security_result_present) {
    HANDLE_CODE(security_result.unpack(bref));
  }
  if (pdu_session_data_forwarding_info_resp_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_resp.unpack(bref));
  }
  if (drb_setup_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_setup_list_ng_ran, bref, 1, 32, true));
  }
  if (drb_failed_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_failed_list_ng_ran, bref, 1, 32, true));
  }
  if (drb_modified_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_modified_list_ng_ran, bref, 1, 32, true));
  }
  if (drb_failed_to_modify_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_failed_to_modify_list_ng_ran, bref, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_modified_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  if (ng_dl_up_tnl_info_present) {
    j.write_fieldname("nG-DL-UP-TNL-Information");
    ng_dl_up_tnl_info.to_json(j);
  }
  if (security_result_present) {
    j.write_fieldname("securityResult");
    security_result.to_json(j);
  }
  if (pdu_session_data_forwarding_info_resp_present) {
    j.write_fieldname("pDU-Session-Data-Forwarding-Information-Response");
    pdu_session_data_forwarding_info_resp.to_json(j);
  }
  if (drb_setup_list_ng_ran.size() > 0) {
    j.start_array("dRB-Setup-List-NG-RAN");
    for (const auto& e1 : drb_setup_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_failed_list_ng_ran.size() > 0) {
    j.start_array("dRB-Failed-List-NG-RAN");
    for (const auto& e1 : drb_failed_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_modified_list_ng_ran.size() > 0) {
    j.start_array("dRB-Modified-List-NG-RAN");
    for (const auto& e1 : drb_modified_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_failed_to_modify_list_ng_ran.size() > 0) {
    j.start_array("dRB-Failed-To-Modify-List-NG-RAN");
    for (const auto& e1 : drb_failed_to_modify_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// PDU-Session-Resource-Setup-Mod-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_setup_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(security_result_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.pack(drb_failed_mod_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  if (security_result_present) {
    HANDLE_CODE(security_result.pack(bref));
  }
  HANDLE_CODE(ng_dl_up_tnl_info.pack(bref));
  if (pdu_session_data_forwarding_info_resp_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_resp.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, drb_setup_mod_list_ng_ran, 1, 32, true));
  if (drb_failed_mod_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_failed_mod_list_ng_ran, 1, 32, true));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_setup_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(security_result_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_data_forwarding_info_resp_present, 1));
  bool drb_failed_mod_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_failed_mod_list_ng_ran_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (security_result_present) {
    HANDLE_CODE(security_result.unpack(bref));
  }
  HANDLE_CODE(ng_dl_up_tnl_info.unpack(bref));
  if (pdu_session_data_forwarding_info_resp_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_resp.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(drb_setup_mod_list_ng_ran, bref, 1, 32, true));
  if (drb_failed_mod_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_failed_mod_list_ng_ran, bref, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_setup_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  if (security_result_present) {
    j.write_fieldname("securityResult");
    security_result.to_json(j);
  }
  j.write_fieldname("nG-DL-UP-TNL-Information");
  ng_dl_up_tnl_info.to_json(j);
  if (pdu_session_data_forwarding_info_resp_present) {
    j.write_fieldname("pDU-Session-Data-Forwarding-Information-Response");
    pdu_session_data_forwarding_info_resp.to_json(j);
  }
  j.start_array("dRB-Setup-Mod-List-NG-RAN");
  for (const auto& e1 : drb_setup_mod_list_ng_ran) {
    e1.to_json(j);
  }
  j.end_array();
  if (drb_failed_mod_list_ng_ran.size() > 0) {
    j.start_array("dRB-Failed-Mod-List-NG-RAN");
    for (const auto& e1 : drb_failed_mod_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// DRB-Status-Item ::= SEQUENCE
SRSASN_CODE drb_status_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdcp_dl_count_present, 1));
  HANDLE_CODE(bref.pack(pdcp_ul_count_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (pdcp_dl_count_present) {
    HANDLE_CODE(pdcp_dl_count.pack(bref));
  }
  if (pdcp_ul_count_present) {
    HANDLE_CODE(pdcp_ul_count.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_status_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdcp_dl_count_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_ul_count_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (pdcp_dl_count_present) {
    HANDLE_CODE(pdcp_dl_count.unpack(bref));
  }
  if (pdcp_ul_count_present) {
    HANDLE_CODE(pdcp_ul_count.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_status_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (pdcp_dl_count_present) {
    j.write_fieldname("pDCP-DL-Count");
    pdcp_dl_count.to_json(j);
  }
  if (pdcp_ul_count_present) {
    j.write_fieldname("pDCP-UL-Count");
    pdcp_ul_count.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Links-to-log ::= ENUMERATED
const char* links_to_log_opts::to_string() const
{
  static const char* names[] = {"uplink", "downlink", "both-uplink-and-downlink"};
  return convert_enum_idx(names, 3, value, "links_to_log_e");
}

// M4period ::= ENUMERATED
const char* m4period_opts::to_string() const
{
  static const char* names[] = {"ms1024", "ms2048", "ms5120", "ms10240", "min1"};
  return convert_enum_idx(names, 5, value, "m4period_e");
}
uint16_t m4period_opts::to_number() const
{
  static const uint16_t numbers[] = {1024, 2048, 5120, 10240, 1};
  return map_enum_number(numbers, 5, value, "m4period_e");
}

// M6report-Interval ::= ENUMERATED
const char* m6report_interv_opts::to_string() const
{
  static const char* names[] = {"ms120",
                                "ms240",
                                "ms480",
                                "ms640",
                                "ms1024",
                                "ms2048",
                                "ms5120",
                                "ms10240",
                                "ms20480",
                                "ms40960",
                                "min1",
                                "min6",
                                "min12",
                                "min30"};
  return convert_enum_idx(names, 14, value, "m6report_interv_e");
}
uint16_t m6report_interv_opts::to_number() const
{
  static const uint16_t numbers[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 1, 6, 12, 30};
  return map_enum_number(numbers, 14, value, "m6report_interv_e");
}

// RSN ::= ENUMERATED
const char* r_sn_opts::to_string() const
{
  static const char* names[] = {"v1", "v2"};
  return convert_enum_idx(names, 2, value, "r_sn_e");
}
uint8_t r_sn_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "r_sn_e");
}

// M4Configuration ::= SEQUENCE
SRSASN_CODE m4_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(m4period.pack(bref));
  HANDLE_CODE(m4_links_to_log.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE m4_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(m4period.unpack(bref));
  HANDLE_CODE(m4_links_to_log.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void m4_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("m4period", m4period.to_string());
  j.write_str("m4-links-to-log", m4_links_to_log.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// M6Configuration ::= SEQUENCE
SRSASN_CODE m6_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(m6report_interv.pack(bref));
  HANDLE_CODE(m6_links_to_log.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE m6_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(m6report_interv.unpack(bref));
  HANDLE_CODE(m6_links_to_log.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void m6_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("m6report-Interval", m6report_interv.to_string());
  j.write_str("m6-links-to-log", m6_links_to_log.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// M7Configuration ::= SEQUENCE
SRSASN_CODE m7_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, m7period, (uint8_t)1u, (uint8_t)60u, true, true));
  HANDLE_CODE(m7_links_to_log.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE m7_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(m7period, bref, (uint8_t)1u, (uint8_t)60u, true, true));
  HANDLE_CODE(m7_links_to_log.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void m7_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("m7period", m7period);
  j.write_str("m7-links-to-log", m7_links_to_log.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// RedundantPDUSessionInformation ::= SEQUENCE
SRSASN_CODE redundant_pdu_session_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(rsn.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE redundant_pdu_session_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(rsn.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void redundant_pdu_session_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rSN", rsn.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-To-Setup-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_to_setup_item_eutran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {142};
  return map_enum_number(names, 1, idx, "id");
}
bool drb_to_setup_item_eutran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 142 == id;
}
crit_e drb_to_setup_item_eutran_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 142) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drb_to_setup_item_eutran_ext_ies_o::ext_c drb_to_setup_item_eutran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 142) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_to_setup_item_eutran_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 142) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void drb_to_setup_item_eutran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("BIT STRING", c.to_string());
  j.end_obj();
}
SRSASN_CODE drb_to_setup_item_eutran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_item_eutran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* drb_to_setup_item_eutran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING"};
  return convert_enum_idx(names, 1, value, "drb_to_setup_item_eutran_ext_ies_o::ext_c::types");
}

// ImmediateMDT ::= SEQUENCE
SRSASN_CODE immediate_mdt_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_four_present, 1));
  HANDLE_CODE(bref.pack(meas_six_present, 1));
  HANDLE_CODE(bref.pack(meas_seven_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(meass_to_activ.pack(bref));
  if (meas_four_present) {
    HANDLE_CODE(meas_four.pack(bref));
  }
  if (meas_six_present) {
    HANDLE_CODE(meas_six.pack(bref));
  }
  if (meas_seven_present) {
    HANDLE_CODE(meas_seven.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE immediate_mdt_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_four_present, 1));
  HANDLE_CODE(bref.unpack(meas_six_present, 1));
  HANDLE_CODE(bref.unpack(meas_seven_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(meass_to_activ.unpack(bref));
  if (meas_four_present) {
    HANDLE_CODE(meas_four.unpack(bref));
  }
  if (meas_six_present) {
    HANDLE_CODE(meas_six.unpack(bref));
  }
  if (meas_seven_present) {
    HANDLE_CODE(meas_seven.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void immediate_mdt_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measurementsToActivate", meass_to_activ.to_string());
  if (meas_four_present) {
    j.write_fieldname("measurementFour");
    meas_four.to_json(j);
  }
  if (meas_six_present) {
    j.write_fieldname("measurementSix");
    meas_six.to_json(j);
  }
  if (meas_seven_present) {
    j.write_fieldname("measurementSeven");
    meas_seven.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-To-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_to_setup_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {78, 97, 96, 105};
  return map_enum_number(names, 4, idx, "id");
}
bool pdu_session_res_to_setup_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {78, 97, 96, 105};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e pdu_session_res_to_setup_item_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 78:
      return crit_e::ignore;
    case 97:
      return crit_e::ignore;
    case 96:
      return crit_e::ignore;
    case 105:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
pdu_session_res_to_setup_item_ext_ies_o::ext_c pdu_session_res_to_setup_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 78:
      ret.set(ext_c::types::common_network_instance);
      break;
    case 97:
      ret.set(ext_c::types::redundant_n_g_ul_up_tnl_info);
      break;
    case 96:
      ret.set(ext_c::types::redundant_common_network_instance);
      break;
    case 105:
      ret.set(ext_c::types::redundant_pdu_session_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_to_setup_item_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 78:
      return presence_e::optional;
    case 97:
      return presence_e::optional;
    case 96:
      return presence_e::optional;
    case 105:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_to_setup_item_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::common_network_instance:
      c = unbounded_octstring<true>{};
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      c = up_tnl_info_c{};
      break;
    case types::redundant_common_network_instance:
      c = unbounded_octstring<true>{};
      break;
    case types::redundant_pdu_session_info:
      c = redundant_pdu_session_info_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_item_ext_ies_o::ext_c");
  }
}
unbounded_octstring<true>& pdu_session_res_to_setup_item_ext_ies_o::ext_c::common_network_instance()
{
  assert_choice_type(types::common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
up_tnl_info_c& pdu_session_res_to_setup_item_ext_ies_o::ext_c::redundant_n_g_ul_up_tnl_info()
{
  assert_choice_type(types::redundant_n_g_ul_up_tnl_info, type_, "Extension");
  return c.get<up_tnl_info_c>();
}
unbounded_octstring<true>& pdu_session_res_to_setup_item_ext_ies_o::ext_c::redundant_common_network_instance()
{
  assert_choice_type(types::redundant_common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
redundant_pdu_session_info_s& pdu_session_res_to_setup_item_ext_ies_o::ext_c::redundant_pdu_session_info()
{
  assert_choice_type(types::redundant_pdu_session_info, type_, "Extension");
  return c.get<redundant_pdu_session_info_s>();
}
const unbounded_octstring<true>& pdu_session_res_to_setup_item_ext_ies_o::ext_c::common_network_instance() const
{
  assert_choice_type(types::common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
const up_tnl_info_c& pdu_session_res_to_setup_item_ext_ies_o::ext_c::redundant_n_g_ul_up_tnl_info() const
{
  assert_choice_type(types::redundant_n_g_ul_up_tnl_info, type_, "Extension");
  return c.get<up_tnl_info_c>();
}
const unbounded_octstring<true>&
pdu_session_res_to_setup_item_ext_ies_o::ext_c::redundant_common_network_instance() const
{
  assert_choice_type(types::redundant_common_network_instance, type_, "Extension");
  return c.get<unbounded_octstring<true>>();
}
const redundant_pdu_session_info_s& pdu_session_res_to_setup_item_ext_ies_o::ext_c::redundant_pdu_session_info() const
{
  assert_choice_type(types::redundant_pdu_session_info, type_, "Extension");
  return c.get<redundant_pdu_session_info_s>();
}
void pdu_session_res_to_setup_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::common_network_instance:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      j.write_fieldname("UP-TNL-Information");
      c.get<up_tnl_info_c>().to_json(j);
      break;
    case types::redundant_common_network_instance:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::redundant_pdu_session_info:
      j.write_fieldname("RedundantPDUSessionInformation");
      c.get<redundant_pdu_session_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_item_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE pdu_session_res_to_setup_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      HANDLE_CODE(c.get<up_tnl_info_c>().pack(bref));
      break;
    case types::redundant_common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::redundant_pdu_session_info:
      HANDLE_CODE(c.get<redundant_pdu_session_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_setup_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::redundant_n_g_ul_up_tnl_info:
      HANDLE_CODE(c.get<up_tnl_info_c>().unpack(bref));
      break;
    case types::redundant_common_network_instance:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::redundant_pdu_session_info:
      HANDLE_CODE(c.get<redundant_pdu_session_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_to_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING", "UP-TNL-Information", "OCTET STRING", "RedundantPDUSessionInformation"};
  return convert_enum_idx(names, 4, value, "pdu_session_res_to_setup_item_ext_ies_o::ext_c::types");
}

// DRB-To-Setup-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_to_setup_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(dl_up_params.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(existing_allocated_s1_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(pdcp_cfg.pack(bref));
  HANDLE_CODE(eutran_qos.pack(bref));
  HANDLE_CODE(s1_ul_up_tnl_info.pack(bref));
  if (data_forwarding_info_request_present) {
    HANDLE_CODE(data_forwarding_info_request.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, cell_group_info, 1, 4, true));
  if (dl_up_params.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_up_params, 1, 8, true));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, drb_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (existing_allocated_s1_dl_up_tnl_info_present) {
    HANDLE_CODE(existing_allocated_s1_dl_up_tnl_info.pack(bref));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_forwarding_info_request_present, 1));
  bool dl_up_params_present;
  HANDLE_CODE(bref.unpack(dl_up_params_present, 1));
  HANDLE_CODE(bref.unpack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(existing_allocated_s1_dl_up_tnl_info_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(pdcp_cfg.unpack(bref));
  HANDLE_CODE(eutran_qos.unpack(bref));
  HANDLE_CODE(s1_ul_up_tnl_info.unpack(bref));
  if (data_forwarding_info_request_present) {
    HANDLE_CODE(data_forwarding_info_request.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(cell_group_info, bref, 1, 4, true));
  if (dl_up_params_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dl_up_params, bref, 1, 8, true));
  }
  if (drb_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(drb_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (existing_allocated_s1_dl_up_tnl_info_present) {
    HANDLE_CODE(existing_allocated_s1_dl_up_tnl_info.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void drb_to_setup_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("pDCP-Configuration");
  pdcp_cfg.to_json(j);
  j.write_fieldname("eUTRAN-QoS");
  eutran_qos.to_json(j);
  j.write_fieldname("s1-UL-UP-TNL-Information");
  s1_ul_up_tnl_info.to_json(j);
  if (data_forwarding_info_request_present) {
    j.write_fieldname("data-Forwarding-Information-Request");
    data_forwarding_info_request.to_json(j);
  }
  j.start_array("cell-Group-Information");
  for (const auto& e1 : cell_group_info) {
    e1.to_json(j);
  }
  j.end_array();
  if (dl_up_params.size() > 0) {
    j.start_array("dL-UP-Parameters");
    for (const auto& e1 : dl_up_params) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_inactivity_timer_present) {
    j.write_int("dRB-Inactivity-Timer", drb_inactivity_timer);
  }
  if (existing_allocated_s1_dl_up_tnl_info_present) {
    j.write_fieldname("existing-Allocated-S1-DL-UP-TNL-Info");
    existing_allocated_s1_dl_up_tnl_info.to_json(j);
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// MDT-Activation ::= ENUMERATED
const char* mdt_activation_opts::to_string() const
{
  static const char* names[] = {"immediate-MDT-only", "immediate-MDT-and-Trace"};
  return convert_enum_idx(names, 2, value, "mdt_activation_e");
}

// MDTMode ::= CHOICE
void mdt_mode_c::destroy_()
{
  switch (type_) {
    case types::immediate_mdt:
      c.destroy<immediate_mdt_s>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void mdt_mode_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::immediate_mdt:
      c.init<immediate_mdt_s>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mdt_mode_c");
  }
}
mdt_mode_c::mdt_mode_c(const mdt_mode_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::immediate_mdt:
      c.init(other.c.get<immediate_mdt_s>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mdt_mode_c");
  }
}
mdt_mode_c& mdt_mode_c::operator=(const mdt_mode_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::immediate_mdt:
      c.set(other.c.get<immediate_mdt_s>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mdt_mode_c");
  }

  return *this;
}
immediate_mdt_s& mdt_mode_c::set_immediate_mdt()
{
  set(types::immediate_mdt);
  return c.get<immediate_mdt_s>();
}
protocol_ie_single_container_s<mdt_mode_ext_ies_o>& mdt_mode_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>();
}
void mdt_mode_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::immediate_mdt:
      j.write_fieldname("immediateMDT");
      c.get<immediate_mdt_s>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "mdt_mode_c");
  }
  j.end_obj();
}
SRSASN_CODE mdt_mode_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::immediate_mdt:
      HANDLE_CODE(c.get<immediate_mdt_s>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mdt_mode_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mdt_mode_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::immediate_mdt:
      HANDLE_CODE(c.get<immediate_mdt_s>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mdt_mode_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mdt_mode_c::types_opts::to_string() const
{
  static const char* names[] = {"immediateMDT", "choice-extension"};
  return convert_enum_idx(names, 2, value, "mdt_mode_c::types");
}

template struct asn1::protocol_ext_field_s<pdu_session_res_to_setup_item_ext_ies_o>;

SRSASN_CODE pdu_session_res_to_setup_item_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += common_network_instance_present ? 1 : 0;
  nof_ies += redundant_n_g_ul_up_tnl_info_present ? 1 : 0;
  nof_ies += redundant_common_network_instance_present ? 1 : 0;
  nof_ies += redundant_pdu_session_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (common_network_instance_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)78, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(common_network_instance.pack(bref));
  }
  if (redundant_n_g_ul_up_tnl_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)97, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_n_g_ul_up_tnl_info.pack(bref));
  }
  if (redundant_common_network_instance_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)96, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_common_network_instance.pack(bref));
  }
  if (redundant_pdu_session_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)105, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_pdu_session_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_setup_item_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 78: {
        common_network_instance_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(common_network_instance.unpack(bref));
        break;
      }
      case 97: {
        redundant_n_g_ul_up_tnl_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_n_g_ul_up_tnl_info.unpack(bref));
        break;
      }
      case 96: {
        redundant_common_network_instance_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_common_network_instance.unpack(bref));
        break;
      }
      case 105: {
        redundant_pdu_session_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_pdu_session_info.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_to_setup_item_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (common_network_instance_present) {
    j.write_int("id", 78);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", common_network_instance.to_string());
  }
  if (redundant_n_g_ul_up_tnl_info_present) {
    j.write_int("id", 97);
    j.write_str("criticality", "ignore");
    redundant_n_g_ul_up_tnl_info.to_json(j);
  }
  if (redundant_common_network_instance_present) {
    j.write_int("id", 96);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", redundant_common_network_instance.to_string());
  }
  if (redundant_pdu_session_info_present) {
    j.write_int("id", 105);
    j.write_str("criticality", "ignore");
    redundant_pdu_session_info.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-To-Setup-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_to_setup_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdu_session_res_dl_ambr_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(existing_allocated_ng_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(network_instance_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pdu_session_type.pack(bref));
  HANDLE_CODE(snssai.pack(bref));
  HANDLE_CODE(security_ind.pack(bref));
  if (pdu_session_res_dl_ambr_present) {
    HANDLE_CODE(pack_integer(bref, pdu_session_res_dl_ambr, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  HANDLE_CODE(ng_ul_up_tnl_info.pack(bref));
  if (pdu_session_data_forwarding_info_request_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_request.pack(bref));
  }
  if (pdu_session_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, pdu_session_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (existing_allocated_ng_dl_up_tnl_info_present) {
    HANDLE_CODE(existing_allocated_ng_dl_up_tnl_info.pack(bref));
  }
  if (network_instance_present) {
    HANDLE_CODE(pack_integer(bref, network_instance, (uint16_t)1u, (uint16_t)256u, true, true));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_setup_list_ng_ran, 1, 32, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_setup_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdu_session_res_dl_ambr_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(existing_allocated_ng_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.unpack(network_instance_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pdu_session_type.unpack(bref));
  HANDLE_CODE(snssai.unpack(bref));
  HANDLE_CODE(security_ind.unpack(bref));
  if (pdu_session_res_dl_ambr_present) {
    HANDLE_CODE(unpack_integer(pdu_session_res_dl_ambr, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  HANDLE_CODE(ng_ul_up_tnl_info.unpack(bref));
  if (pdu_session_data_forwarding_info_request_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_request.unpack(bref));
  }
  if (pdu_session_inactivity_timer_present) {
    HANDLE_CODE(unpack_integer(pdu_session_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (existing_allocated_ng_dl_up_tnl_info_present) {
    HANDLE_CODE(existing_allocated_ng_dl_up_tnl_info.unpack(bref));
  }
  if (network_instance_present) {
    HANDLE_CODE(unpack_integer(network_instance, bref, (uint16_t)1u, (uint16_t)256u, true, true));
  }
  HANDLE_CODE(unpack_dyn_seq_of(drb_to_setup_list_ng_ran, bref, 1, 32, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_to_setup_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.write_str("pDU-Session-Type", pdu_session_type.to_string());
  j.write_fieldname("sNSSAI");
  snssai.to_json(j);
  j.write_fieldname("securityIndication");
  security_ind.to_json(j);
  if (pdu_session_res_dl_ambr_present) {
    j.write_int("pDU-Session-Resource-DL-AMBR", pdu_session_res_dl_ambr);
  }
  j.write_fieldname("nG-UL-UP-TNL-Information");
  ng_ul_up_tnl_info.to_json(j);
  if (pdu_session_data_forwarding_info_request_present) {
    j.write_fieldname("pDU-Session-Data-Forwarding-Information-Request");
    pdu_session_data_forwarding_info_request.to_json(j);
  }
  if (pdu_session_inactivity_timer_present) {
    j.write_int("pDU-Session-Inactivity-Timer", pdu_session_inactivity_timer);
  }
  if (existing_allocated_ng_dl_up_tnl_info_present) {
    j.write_fieldname("existing-Allocated-NG-DL-UP-TNL-Info");
    existing_allocated_ng_dl_up_tnl_info.to_json(j);
  }
  if (network_instance_present) {
    j.write_int("networkInstance", network_instance);
  }
  j.start_array("dRB-To-Setup-List-NG-RAN");
  for (const auto& e1 : drb_to_setup_list_ng_ran) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// MDT-Configuration ::= SEQUENCE
SRSASN_CODE mdt_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(mdt_activation.pack(bref));
  HANDLE_CODE(mdt_mode.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mdt_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(mdt_activation.unpack(bref));
  HANDLE_CODE(mdt_mode.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mdt_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("mdt-Activation", mdt_activation.to_string());
  j.write_fieldname("mDTMode");
  mdt_mode.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// NPNContextInfo-SNPN ::= SEQUENCE
SRSASN_CODE npn_context_info_sn_pn_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(nid.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE npn_context_info_sn_pn_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(nid.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void npn_context_info_sn_pn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("nID", nid.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// TraceActivation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t trace_activation_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {112, 116};
  return map_enum_number(names, 2, idx, "id");
}
bool trace_activation_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {112, 116};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e trace_activation_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 112:
      return crit_e::ignore;
    case 116:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
trace_activation_ext_ies_o::ext_c trace_activation_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 112:
      ret.set(ext_c::types::mdt_cfg);
      break;
    case 116:
      ret.set(ext_c::types::trace_collection_entity_uri);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e trace_activation_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 112:
      return presence_e::optional;
    case 116:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void trace_activation_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::mdt_cfg:
      c = mdt_cfg_s{};
      break;
    case types::trace_collection_entity_uri:
      c = visible_string<0, MAX_ASN_STRING_LENGTH, false, true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trace_activation_ext_ies_o::ext_c");
  }
}
mdt_cfg_s& trace_activation_ext_ies_o::ext_c::mdt_cfg()
{
  assert_choice_type(types::mdt_cfg, type_, "Extension");
  return c.get<mdt_cfg_s>();
}
visible_string<0, MAX_ASN_STRING_LENGTH, false, true>& trace_activation_ext_ies_o::ext_c::trace_collection_entity_uri()
{
  assert_choice_type(types::trace_collection_entity_uri, type_, "Extension");
  return c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
}
const mdt_cfg_s& trace_activation_ext_ies_o::ext_c::mdt_cfg() const
{
  assert_choice_type(types::mdt_cfg, type_, "Extension");
  return c.get<mdt_cfg_s>();
}
const visible_string<0, MAX_ASN_STRING_LENGTH, false, true>&
trace_activation_ext_ies_o::ext_c::trace_collection_entity_uri() const
{
  assert_choice_type(types::trace_collection_entity_uri, type_, "Extension");
  return c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
}
void trace_activation_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mdt_cfg:
      j.write_fieldname("MDT-Configuration");
      c.get<mdt_cfg_s>().to_json(j);
      break;
    case types::trace_collection_entity_uri:
      j.write_str("VisibleString", c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "trace_activation_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE trace_activation_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::mdt_cfg:
      HANDLE_CODE(c.get<mdt_cfg_s>().pack(bref));
      break;
    case types::trace_collection_entity_uri:
      HANDLE_CODE((c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "trace_activation_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE trace_activation_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::mdt_cfg:
      HANDLE_CODE(c.get<mdt_cfg_s>().unpack(bref));
      break;
    case types::trace_collection_entity_uri:
      HANDLE_CODE((c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "trace_activation_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* trace_activation_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"MDT-Configuration", "VisibleString"};
  return convert_enum_idx(names, 2, value, "trace_activation_ext_ies_o::ext_c::types");
}

// TraceDepth ::= ENUMERATED
const char* trace_depth_opts::to_string() const
{
  static const char* names[] = {"minimum",
                                "medium",
                                "maximum",
                                "minimumWithoutVendorSpecificExtension",
                                "mediumWithoutVendorSpecificExtension",
                                "maximumWithoutVendorSpecificExtension"};
  return convert_enum_idx(names, 6, value, "trace_depth_e");
}

// AdditionalHandoverInfo ::= ENUMERATED
const char* add_ho_info_opts::to_string() const
{
  static const char* names[] = {"discard-pdpc-SN"};
  return convert_enum_idx(names, 1, value, "add_ho_info_e");
}

// CHOInitiation ::= ENUMERATED
const char* cho_initiation_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "cho_initiation_e");
}

// DirectForwardingPathAvailability ::= ENUMERATED
const char* direct_forwarding_path_availability_opts::to_string() const
{
  static const char* names[] = {"inter-system-direct-path-available", "intra-system-direct-path-available"};
  return convert_enum_idx(names, 2, value, "direct_forwarding_path_availability_e");
}

// NPNContextInfo ::= CHOICE
void npn_context_info_c::destroy_()
{
  switch (type_) {
    case types::sn_pn:
      c.destroy<npn_context_info_sn_pn_s>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void npn_context_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sn_pn:
      c.init<npn_context_info_sn_pn_s>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_context_info_c");
  }
}
npn_context_info_c::npn_context_info_c(const npn_context_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sn_pn:
      c.init(other.c.get<npn_context_info_sn_pn_s>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_context_info_c");
  }
}
npn_context_info_c& npn_context_info_c::operator=(const npn_context_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sn_pn:
      c.set(other.c.get<npn_context_info_sn_pn_s>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_context_info_c");
  }

  return *this;
}
npn_context_info_sn_pn_s& npn_context_info_c::set_sn_pn()
{
  set(types::sn_pn);
  return c.get<npn_context_info_sn_pn_s>();
}
protocol_ie_single_container_s<npn_context_info_ext_ies_o>& npn_context_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>();
}
void npn_context_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sn_pn:
      j.write_fieldname("sNPN");
      c.get<npn_context_info_sn_pn_s>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "npn_context_info_c");
  }
  j.end_obj();
}
SRSASN_CODE npn_context_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sn_pn:
      HANDLE_CODE(c.get<npn_context_info_sn_pn_s>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "npn_context_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE npn_context_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sn_pn:
      HANDLE_CODE(c.get<npn_context_info_sn_pn_s>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "npn_context_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* npn_context_info_c::types_opts::to_string() const
{
  static const char* names[] = {"sNPN", "choice-extension"};
  return convert_enum_idx(names, 2, value, "npn_context_info_c::types");
}

template struct asn1::protocol_ext_field_s<trace_activation_ext_ies_o>;

SRSASN_CODE trace_activation_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += mdt_cfg_present ? 1 : 0;
  nof_ies += trace_collection_entity_uri_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (mdt_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)112, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(mdt_cfg.pack(bref));
  }
  if (trace_collection_entity_uri_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)116, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(trace_collection_entity_uri.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE trace_activation_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 112: {
        mdt_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(mdt_cfg.unpack(bref));
        break;
      }
      case 116: {
        trace_collection_entity_uri_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(trace_collection_entity_uri.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void trace_activation_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (mdt_cfg_present) {
    j.write_int("id", 112);
    j.write_str("criticality", "ignore");
    mdt_cfg.to_json(j);
  }
  if (trace_collection_entity_uri_present) {
    j.write_int("id", 116);
    j.write_str("criticality", "ignore");
    j.write_str("Extension", trace_collection_entity_uri.to_string());
  }
  j.end_obj();
}

// TraceActivation ::= SEQUENCE
SRSASN_CODE trace_activation_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(trace_id.pack(bref));
  HANDLE_CODE(interfaces_to_trace.pack(bref));
  HANDLE_CODE(trace_depth.pack(bref));
  HANDLE_CODE(trace_collection_entity_ip_address.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE trace_activation_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(trace_id.unpack(bref));
  HANDLE_CODE(interfaces_to_trace.unpack(bref));
  HANDLE_CODE(trace_depth.unpack(bref));
  HANDLE_CODE(trace_collection_entity_ip_address.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void trace_activation_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("traceID", trace_id.to_string());
  j.write_str("interfacesToTrace", interfaces_to_trace.to_string());
  j.write_str("traceDepth", trace_depth.to_string());
  j.write_str("traceCollectionEntityIPAddress", trace_collection_entity_ip_address.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Setup-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_setup_item_eutran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {142};
  return map_enum_number(names, 1, idx, "id");
}
bool drb_setup_item_eutran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 142 == id;
}
crit_e drb_setup_item_eutran_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 142) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drb_setup_item_eutran_ext_ies_o::ext_c drb_setup_item_eutran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 142) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_setup_item_eutran_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 142) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void drb_setup_item_eutran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("BIT STRING", c.to_string());
  j.end_obj();
}
SRSASN_CODE drb_setup_item_eutran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_setup_item_eutran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* drb_setup_item_eutran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING"};
  return convert_enum_idx(names, 1, value, "drb_setup_item_eutran_ext_ies_o::ext_c::types");
}

// PDU-Session-Resource-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_setup_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {98, 106};
  return map_enum_number(names, 2, idx, "id");
}
bool pdu_session_res_setup_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {98, 106};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e pdu_session_res_setup_item_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 98:
      return crit_e::ignore;
    case 106:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
pdu_session_res_setup_item_ext_ies_o::ext_c pdu_session_res_setup_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 98:
      ret.set(ext_c::types::redundant_n_g_dl_up_tnl_info);
      break;
    case 106:
      ret.set(ext_c::types::redundant_pdu_session_info_used);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_setup_item_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 98:
      return presence_e::optional;
    case 106:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_setup_item_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::redundant_n_g_dl_up_tnl_info:
      c = up_tnl_info_c{};
      break;
    case types::redundant_pdu_session_info_used:
      c = redundant_pdu_session_info_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_setup_item_ext_ies_o::ext_c");
  }
}
up_tnl_info_c& pdu_session_res_setup_item_ext_ies_o::ext_c::redundant_n_g_dl_up_tnl_info()
{
  assert_choice_type(types::redundant_n_g_dl_up_tnl_info, type_, "Extension");
  return c.get<up_tnl_info_c>();
}
redundant_pdu_session_info_s& pdu_session_res_setup_item_ext_ies_o::ext_c::redundant_pdu_session_info_used()
{
  assert_choice_type(types::redundant_pdu_session_info_used, type_, "Extension");
  return c.get<redundant_pdu_session_info_s>();
}
const up_tnl_info_c& pdu_session_res_setup_item_ext_ies_o::ext_c::redundant_n_g_dl_up_tnl_info() const
{
  assert_choice_type(types::redundant_n_g_dl_up_tnl_info, type_, "Extension");
  return c.get<up_tnl_info_c>();
}
const redundant_pdu_session_info_s& pdu_session_res_setup_item_ext_ies_o::ext_c::redundant_pdu_session_info_used() const
{
  assert_choice_type(types::redundant_pdu_session_info_used, type_, "Extension");
  return c.get<redundant_pdu_session_info_s>();
}
void pdu_session_res_setup_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::redundant_n_g_dl_up_tnl_info:
      j.write_fieldname("UP-TNL-Information");
      c.get<up_tnl_info_c>().to_json(j);
      break;
    case types::redundant_pdu_session_info_used:
      j.write_fieldname("RedundantPDUSessionInformation");
      c.get<redundant_pdu_session_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_setup_item_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE pdu_session_res_setup_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::redundant_n_g_dl_up_tnl_info:
      HANDLE_CODE(c.get<up_tnl_info_c>().pack(bref));
      break;
    case types::redundant_pdu_session_info_used:
      HANDLE_CODE(c.get<redundant_pdu_session_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_setup_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_setup_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::redundant_n_g_dl_up_tnl_info:
      HANDLE_CODE(c.get<up_tnl_info_c>().unpack(bref));
      break;
    case types::redundant_pdu_session_info_used:
      HANDLE_CODE(c.get<redundant_pdu_session_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_setup_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UP-TNL-Information", "RedundantPDUSessionInformation"};
  return convert_enum_idx(names, 2, value, "pdu_session_res_setup_item_ext_ies_o::ext_c::types");
}

// DRB-Failed-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_failed_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_failed_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_failed_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Setup-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_setup_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.pack(s1_dl_up_unchanged_present, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(s1_dl_up_tnl_info.pack(bref));
  if (data_forwarding_info_resp_present) {
    HANDLE_CODE(data_forwarding_info_resp.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_transport_params, 1, 8, true));
  if (s1_dl_up_unchanged_present) {
    HANDLE_CODE(s1_dl_up_unchanged.pack(bref));
  }
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_setup_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.unpack(s1_dl_up_unchanged_present, 1));
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(s1_dl_up_tnl_info.unpack(bref));
  if (data_forwarding_info_resp_present) {
    HANDLE_CODE(data_forwarding_info_resp.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ul_up_transport_params, bref, 1, 8, true));
  if (s1_dl_up_unchanged_present) {
    HANDLE_CODE(s1_dl_up_unchanged.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void drb_setup_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("s1-DL-UP-TNL-Information");
  s1_dl_up_tnl_info.to_json(j);
  if (data_forwarding_info_resp_present) {
    j.write_fieldname("data-Forwarding-Information-Response");
    data_forwarding_info_resp.to_json(j);
  }
  j.start_array("uL-UP-Transport-Parameters");
  for (const auto& e1 : ul_up_transport_params) {
    e1.to_json(j);
  }
  j.end_array();
  if (s1_dl_up_unchanged_present) {
    j.write_str("s1-DL-UP-Unchanged", "true");
  }
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

const char* drb_setup_item_eutran_s::s1_dl_up_unchanged_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "drb_setup_item_eutran_s::s1_dl_up_unchanged_e_");
}

// PDU-Session-Resource-Failed-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_failed_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_failed_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_failed_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<pdu_session_res_setup_item_ext_ies_o>;

SRSASN_CODE pdu_session_res_setup_item_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += redundant_n_g_dl_up_tnl_info_present ? 1 : 0;
  nof_ies += redundant_pdu_session_info_used_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (redundant_n_g_dl_up_tnl_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)98, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_n_g_dl_up_tnl_info.pack(bref));
  }
  if (redundant_pdu_session_info_used_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)106, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redundant_pdu_session_info_used.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_setup_item_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 98: {
        redundant_n_g_dl_up_tnl_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_n_g_dl_up_tnl_info.unpack(bref));
        break;
      }
      case 106: {
        redundant_pdu_session_info_used_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redundant_pdu_session_info_used.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_setup_item_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (redundant_n_g_dl_up_tnl_info_present) {
    j.write_int("id", 98);
    j.write_str("criticality", "ignore");
    redundant_n_g_dl_up_tnl_info.to_json(j);
  }
  if (redundant_pdu_session_info_used_present) {
    j.write_int("id", 106);
    j.write_str("criticality", "ignore");
    redundant_pdu_session_info_used.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-Setup-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_setup_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(security_result_present, 1));
  HANDLE_CODE(bref.pack(pdu_session_data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.pack(ng_dl_up_unchanged_present, 1));
  HANDLE_CODE(bref.pack(drb_failed_list_ng_ran.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  if (security_result_present) {
    HANDLE_CODE(security_result.pack(bref));
  }
  HANDLE_CODE(ng_dl_up_tnl_info.pack(bref));
  if (pdu_session_data_forwarding_info_resp_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_resp.pack(bref));
  }
  if (ng_dl_up_unchanged_present) {
    HANDLE_CODE(ng_dl_up_unchanged.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, drb_setup_list_ng_ran, 1, 32, true));
  if (drb_failed_list_ng_ran.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_failed_list_ng_ran, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_setup_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(security_result_present, 1));
  HANDLE_CODE(bref.unpack(pdu_session_data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.unpack(ng_dl_up_unchanged_present, 1));
  bool drb_failed_list_ng_ran_present;
  HANDLE_CODE(bref.unpack(drb_failed_list_ng_ran_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (security_result_present) {
    HANDLE_CODE(security_result.unpack(bref));
  }
  HANDLE_CODE(ng_dl_up_tnl_info.unpack(bref));
  if (pdu_session_data_forwarding_info_resp_present) {
    HANDLE_CODE(pdu_session_data_forwarding_info_resp.unpack(bref));
  }
  if (ng_dl_up_unchanged_present) {
    HANDLE_CODE(ng_dl_up_unchanged.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(drb_setup_list_ng_ran, bref, 1, 32, true));
  if (drb_failed_list_ng_ran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_failed_list_ng_ran, bref, 1, 32, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_setup_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  if (security_result_present) {
    j.write_fieldname("securityResult");
    security_result.to_json(j);
  }
  j.write_fieldname("nG-DL-UP-TNL-Information");
  ng_dl_up_tnl_info.to_json(j);
  if (pdu_session_data_forwarding_info_resp_present) {
    j.write_fieldname("pDU-Session-Data-Forwarding-Information-Response");
    pdu_session_data_forwarding_info_resp.to_json(j);
  }
  if (ng_dl_up_unchanged_present) {
    j.write_str("nG-DL-UP-Unchanged", "true");
  }
  j.start_array("dRB-Setup-List-NG-RAN");
  for (const auto& e1 : drb_setup_list_ng_ran) {
    e1.to_json(j);
  }
  j.end_array();
  if (drb_failed_list_ng_ran.size() > 0) {
    j.start_array("dRB-Failed-List-NG-RAN");
    for (const auto& e1 : drb_failed_list_ng_ran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* pdu_session_res_setup_item_s::ng_dl_up_unchanged_opts::to_string() const
{
  static const char* names[] = {"true"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_setup_item_s::ng_dl_up_unchanged_e_");
}

// Endpoint-IP-address-and-port ::= SEQUENCE
SRSASN_CODE endpoint_ip_address_and_port_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(endpoint_ip_address.pack(bref));
  HANDLE_CODE(port_num.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE endpoint_ip_address_and_port_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(endpoint_ip_address.unpack(bref));
  HANDLE_CODE(port_num.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void endpoint_ip_address_and_port_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("endpoint-IP-Address", endpoint_ip_address.to_string());
  j.write_str("portNumber", port_num.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// CP-TNL-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t cp_tnl_info_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {74};
  return map_enum_number(names, 1, idx, "id");
}
bool cp_tnl_info_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 74 == id;
}
crit_e cp_tnl_info_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 74) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cp_tnl_info_ext_ies_o::value_c cp_tnl_info_ext_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 74) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cp_tnl_info_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 74) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cp_tnl_info_ext_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Endpoint-IP-address-and-port");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cp_tnl_info_ext_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cp_tnl_info_ext_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* cp_tnl_info_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Endpoint-IP-address-and-port"};
  return convert_enum_idx(names, 1, value, "cp_tnl_info_ext_ies_o::value_c::types");
}

template struct asn1::protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>;

// CP-TNL-Information ::= CHOICE
void cp_tnl_info_c::destroy_()
{
  switch (type_) {
    case types::endpoint_ip_address:
      c.destroy<bounded_bitstring<1, 160, true, true>>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void cp_tnl_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::endpoint_ip_address:
      c.init<bounded_bitstring<1, 160, true, true>>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cp_tnl_info_c");
  }
}
cp_tnl_info_c::cp_tnl_info_c(const cp_tnl_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::endpoint_ip_address:
      c.init(other.c.get<bounded_bitstring<1, 160, true, true>>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cp_tnl_info_c");
  }
}
cp_tnl_info_c& cp_tnl_info_c::operator=(const cp_tnl_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::endpoint_ip_address:
      c.set(other.c.get<bounded_bitstring<1, 160, true, true>>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cp_tnl_info_c");
  }

  return *this;
}
bounded_bitstring<1, 160, true, true>& cp_tnl_info_c::set_endpoint_ip_address()
{
  set(types::endpoint_ip_address);
  return c.get<bounded_bitstring<1, 160, true, true>>();
}
protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>& cp_tnl_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>();
}
void cp_tnl_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::endpoint_ip_address:
      j.write_str("endpoint-IP-Address", c.get<bounded_bitstring<1, 160, true, true>>().to_string());
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "cp_tnl_info_c");
  }
  j.end_obj();
}
SRSASN_CODE cp_tnl_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::endpoint_ip_address:
      HANDLE_CODE((c.get<bounded_bitstring<1, 160, true, true>>().pack(bref)));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cp_tnl_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cp_tnl_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::endpoint_ip_address:
      HANDLE_CODE((c.get<bounded_bitstring<1, 160, true, true>>().unpack(bref)));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cp_tnl_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cp_tnl_info_c::types_opts::to_string() const
{
  static const char* names[] = {"endpoint-IP-Address", "choice-extension"};
  return convert_enum_idx(names, 2, value, "cp_tnl_info_c::types");
}

// PrivacyIndicator ::= ENUMERATED
const char* privacy_ind_opts::to_string() const
{
  static const char* names[] = {"immediate-MDT", "logged-MDT"};
  return convert_enum_idx(names, 2, value, "privacy_ind_e");
}

// PDU-Session-To-Notify-Item ::= SEQUENCE
SRSASN_CODE pdu_session_to_notify_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, qos_flow_list, 1, 64, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_to_notify_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(unpack_dyn_seq_of(qos_flow_list, bref, 1, 64, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_to_notify_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.start_array("qoS-Flow-List");
  for (const auto& e1 : qos_flow_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DLUPTNLAddressToUpdateItem ::= SEQUENCE
SRSASN_CODE dl_up_tnl_address_to_upd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(old_tnl_adress.pack(bref));
  HANDLE_CODE(new_tnl_adress.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_up_tnl_address_to_upd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(old_tnl_adress.unpack(bref));
  HANDLE_CODE(new_tnl_adress.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dl_up_tnl_address_to_upd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("oldTNLAdress", old_tnl_adress.to_string());
  j.write_str("newTNLAdress", new_tnl_adress.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Measurement-Results-Information-Item ::= SEQUENCE
SRSASN_CODE drb_meas_results_info_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_d1_result_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  if (ul_d1_result_present) {
    HANDLE_CODE(pack_integer(bref, ul_d1_result, (uint16_t)0u, (uint16_t)10000u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_meas_results_info_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_d1_result_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  if (ul_d1_result_present) {
    HANDLE_CODE(unpack_integer(ul_d1_result, bref, (uint16_t)0u, (uint16_t)10000u, true, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_meas_results_info_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  if (ul_d1_result_present) {
    j.write_int("uL-D1-Result", ul_d1_result);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-Usage-Report-Item ::= SEQUENCE
SRSASN_CODE drb_usage_report_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(start_time_stamp.pack(bref));
  HANDLE_CODE(end_time_stamp.pack(bref));
  HANDLE_CODE(pack_integer(bref, usage_count_ul, (uint64_t)0u, (uint64_t)18446744073709551615u, false, true));
  HANDLE_CODE(pack_integer(bref, usage_count_dl, (uint64_t)0u, (uint64_t)18446744073709551615u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_usage_report_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(start_time_stamp.unpack(bref));
  HANDLE_CODE(end_time_stamp.unpack(bref));
  HANDLE_CODE(unpack_integer(usage_count_ul, bref, (uint64_t)0u, (uint64_t)18446744073709551615u, false, true));
  HANDLE_CODE(unpack_integer(usage_count_dl, bref, (uint64_t)0u, (uint64_t)18446744073709551615u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_usage_report_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("startTimeStamp", start_time_stamp.to_string());
  j.write_str("endTimeStamp", end_time_stamp.to_string());
  j.write_int("usageCountUL", usage_count_ul);
  j.write_int("usageCountDL", usage_count_dl);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRBs-Subject-To-Counter-Check-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drbs_subject_to_counter_check_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(pdcp_ul_count.pack(bref));
  HANDLE_CODE(pdcp_dl_count.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_subject_to_counter_check_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(pdcp_ul_count.unpack(bref));
  HANDLE_CODE(pdcp_dl_count.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drbs_subject_to_counter_check_item_eutran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("pDCP-UL-Count");
  pdcp_ul_count.to_json(j);
  j.write_fieldname("pDCP-DL-Count");
  pdcp_dl_count.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRBs-Subject-To-Counter-Check-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drbs_subject_to_counter_check_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(pdcp_ul_count.pack(bref));
  HANDLE_CODE(pdcp_dl_count.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_subject_to_counter_check_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(pdcp_ul_count.unpack(bref));
  HANDLE_CODE(pdcp_dl_count.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drbs_subject_to_counter_check_item_ng_ran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("pDCP-UL-Count");
  pdcp_ul_count.to_json(j);
  j.write_fieldname("pDCP-DL-Count");
  pdcp_dl_count.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRBs-Subject-To-Early-Forwarding-Item ::= SEQUENCE
SRSASN_CODE drbs_subject_to_early_forwarding_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(dl_count_value.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_subject_to_early_forwarding_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(dl_count_value.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drbs_subject_to_early_forwarding_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_fieldname("dLCountValue");
  dl_count_value.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Data-Usage-Report-Item ::= SEQUENCE
SRSASN_CODE data_usage_report_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(rat_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, drb_usage_report_list, 1, 2, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_usage_report_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(rat_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(drb_usage_report_list, bref, 1, 2, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void data_usage_report_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dRB-ID", drb_id);
  j.write_str("rAT-Type", rat_type.to_string());
  j.start_array("dRB-Usage-Report-List");
  for (const auto& e1 : drb_usage_report_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// MRDC-Data-Usage-Report-Item ::= SEQUENCE
SRSASN_CODE mrdc_data_usage_report_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(start_time_stamp.pack(bref));
  HANDLE_CODE(end_time_stamp.pack(bref));
  HANDLE_CODE(pack_integer(bref, usage_count_ul, (uint64_t)0u, (uint64_t)18446744073709551615u, false, true));
  HANDLE_CODE(pack_integer(bref, usage_count_dl, (uint64_t)0u, (uint64_t)18446744073709551615u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_data_usage_report_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(start_time_stamp.unpack(bref));
  HANDLE_CODE(end_time_stamp.unpack(bref));
  HANDLE_CODE(unpack_integer(usage_count_ul, bref, (uint64_t)0u, (uint64_t)18446744073709551615u, false, true));
  HANDLE_CODE(unpack_integer(usage_count_dl, bref, (uint64_t)0u, (uint64_t)18446744073709551615u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mrdc_data_usage_report_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("startTimeStamp", start_time_stamp.to_string());
  j.write_str("endTimeStamp", end_time_stamp.to_string());
  j.write_int("usageCountUL", usage_count_ul);
  j.write_int("usageCountDL", usage_count_dl);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Data-Usage-per-PDU-Session-Report ::= SEQUENCE
SRSASN_CODE data_usage_per_pdu_session_report_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(secondary_rat_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_timed_report_list, 1, 2, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_usage_per_pdu_session_report_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(secondary_rat_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(pdu_session_timed_report_list, bref, 1, 2, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void data_usage_per_pdu_session_report_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("secondaryRATType", secondary_rat_type.to_string());
  j.start_array("pDU-session-Timed-Report-List");
  for (const auto& e1 : pdu_session_timed_report_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* data_usage_per_pdu_session_report_s::secondary_rat_type_opts::to_string() const
{
  static const char* names[] = {"nR", "e-UTRA"};
  return convert_enum_idx(names, 2, value, "data_usage_per_pdu_session_report_s::secondary_rat_type_e_");
}

// Data-Usage-per-QoS-Flow-Item ::= SEQUENCE
SRSASN_CODE data_usage_per_qos_flow_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qos_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(secondary_rat_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, qos_flow_timed_report_list, 1, 2, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_usage_per_qos_flow_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qos_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(secondary_rat_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(qos_flow_timed_report_list, bref, 1, 2, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void data_usage_per_qos_flow_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qos_flow_id);
  j.write_str("secondaryRATType", secondary_rat_type.to_string());
  j.start_array("qoS-Flow-Timed-Report-List");
  for (const auto& e1 : qos_flow_timed_report_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* data_usage_per_qos_flow_item_s::secondary_rat_type_opts::to_string() const
{
  static const char* names[] = {"nR", "e-UTRA"};
  return convert_enum_idx(names, 2, value, "data_usage_per_qos_flow_item_s::secondary_rat_type_e_");
}

// NR-CGI ::= SEQUENCE
SRSASN_CODE nr_cgi_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(nr_cell_id.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_cgi_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(nr_cell_id.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nr_cgi_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_str("nR-Cell-Identity", nr_cell_id.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// EUTRAN-QoS-Support-Item ::= SEQUENCE
SRSASN_CODE eutran_qos_support_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(eutran_qos.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_qos_support_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(eutran_qos.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void eutran_qos_support_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eUTRAN-QoS");
  eutran_qos.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Extended-NR-CGI-Support-Item ::= SEQUENCE
SRSASN_CODE extended_nr_cgi_support_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(nr_cgi.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE extended_nr_cgi_support_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(nr_cgi.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void extended_nr_cgi_support_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("nR-CGI");
  nr_cgi.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GTPTLA-Item ::= SEQUENCE
SRSASN_CODE gtp_tla_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(gtp_transport_layer_addresses.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gtp_tla_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(gtp_transport_layer_addresses.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gtp_tla_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gTPTransportLayerAddresses", gtp_transport_layer_addresses.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// NG-RAN-QoS-Support-Item ::= SEQUENCE
SRSASN_CODE ng_ran_qos_support_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(non_dyn_5qi_descriptor.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_qos_support_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(non_dyn_5qi_descriptor.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ng_ran_qos_support_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("non-Dynamic5QIDescriptor");
  non_dyn_5qi_descriptor.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// NPNSupportInfo-SNPN ::= SEQUENCE
SRSASN_CODE npn_support_info_sn_pn_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(nid.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE npn_support_info_sn_pn_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(nid.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void npn_support_info_sn_pn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("nID", nid.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Slice-Support-Item ::= SEQUENCE
SRSASN_CODE slice_support_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(snssai.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE slice_support_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(snssai.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void slice_support_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sNSSAI");
  snssai.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// NPNSupportInfo ::= CHOICE
void npn_support_info_c::destroy_()
{
  switch (type_) {
    case types::sn_pn:
      c.destroy<npn_support_info_sn_pn_s>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void npn_support_info_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sn_pn:
      c.init<npn_support_info_sn_pn_s>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_support_info_c");
  }
}
npn_support_info_c::npn_support_info_c(const npn_support_info_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sn_pn:
      c.init(other.c.get<npn_support_info_sn_pn_s>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_support_info_c");
  }
}
npn_support_info_c& npn_support_info_c::operator=(const npn_support_info_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sn_pn:
      c.set(other.c.get<npn_support_info_sn_pn_s>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "npn_support_info_c");
  }

  return *this;
}
npn_support_info_sn_pn_s& npn_support_info_c::set_sn_pn()
{
  set(types::sn_pn);
  return c.get<npn_support_info_sn_pn_s>();
}
protocol_ie_single_container_s<npn_support_info_ext_ies_o>& npn_support_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>();
}
void npn_support_info_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sn_pn:
      j.write_fieldname("sNPN");
      c.get<npn_support_info_sn_pn_s>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "npn_support_info_c");
  }
  j.end_obj();
}
SRSASN_CODE npn_support_info_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sn_pn:
      HANDLE_CODE(c.get<npn_support_info_sn_pn_s>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "npn_support_info_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE npn_support_info_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sn_pn:
      HANDLE_CODE(c.get<npn_support_info_sn_pn_s>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "npn_support_info_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* npn_support_info_c::types_opts::to_string() const
{
  static const char* names[] = {"sNPN", "choice-extension"};
  return convert_enum_idx(names, 2, value, "npn_support_info_c::types");
}

// NR-CGI-Support-Item ::= SEQUENCE
SRSASN_CODE nr_cgi_support_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(nr_cgi.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_cgi_support_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(nr_cgi.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nr_cgi_support_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("nR-CGI");
  nr_cgi.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// UE-associatedLogicalE1-ConnectionItem ::= SEQUENCE
SRSASN_CODE ue_associated_lc_e1_conn_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gnb_cu_cp_ue_e1ap_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_up_ue_e1ap_id_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (gnb_cu_cp_ue_e1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_cu_up_ue_e1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_associated_lc_e1_conn_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gnb_cu_cp_ue_e1ap_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_up_ue_e1ap_id_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (gnb_cu_cp_ue_e1ap_id_present) {
    HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_cu_up_ue_e1ap_id_present) {
    HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_associated_lc_e1_conn_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnb_cu_cp_ue_e1ap_id_present) {
    j.write_int("gNB-CU-CP-UE-E1AP-ID", gnb_cu_cp_ue_e1ap_id);
  }
  if (gnb_cu_up_ue_e1ap_id_present) {
    j.write_int("gNB-CU-UP-UE-E1AP-ID", gnb_cu_up_ue_e1ap_id);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {75};
  return map_enum_number(names, 1, idx, "id");
}
bool gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 75 == id;
}
crit_e gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 75) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::ext_c gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 75) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 75) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("CP-TNL-Information");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"CP-TNL-Information"};
  return convert_enum_idx(names, 1, value, "gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o::ext_c::types");
}

// MRDC-Usage-Information ::= SEQUENCE
SRSASN_CODE mrdc_usage_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_usage_per_pdu_session_report_present, 1));
  HANDLE_CODE(bref.pack(data_usage_per_qos_flow_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (data_usage_per_pdu_session_report_present) {
    HANDLE_CODE(data_usage_per_pdu_session_report.pack(bref));
  }
  if (data_usage_per_qos_flow_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, data_usage_per_qos_flow_list, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_usage_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_usage_per_pdu_session_report_present, 1));
  bool data_usage_per_qos_flow_list_present;
  HANDLE_CODE(bref.unpack(data_usage_per_qos_flow_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (data_usage_per_pdu_session_report_present) {
    HANDLE_CODE(data_usage_per_pdu_session_report.unpack(bref));
  }
  if (data_usage_per_qos_flow_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(data_usage_per_qos_flow_list, bref, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mrdc_usage_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (data_usage_per_pdu_session_report_present) {
    j.write_fieldname("data-Usage-per-PDU-Session-Report");
    data_usage_per_pdu_session_report.to_json(j);
  }
  if (data_usage_per_qos_flow_list.size() > 0) {
    j.start_array("data-Usage-per-QoS-Flow-List");
    for (const auto& e1 : data_usage_per_qos_flow_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// QoS-Parameters-Support-List ::= SEQUENCE
SRSASN_CODE qos_params_support_list_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(eutran_qos_support_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ng_ran_qos_support_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (eutran_qos_support_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutran_qos_support_list, 1, 256, true));
  }
  if (ng_ran_qos_support_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ng_ran_qos_support_list, 1, 256, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qos_params_support_list_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool eutran_qos_support_list_present;
  HANDLE_CODE(bref.unpack(eutran_qos_support_list_present, 1));
  bool ng_ran_qos_support_list_present;
  HANDLE_CODE(bref.unpack(ng_ran_qos_support_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (eutran_qos_support_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutran_qos_support_list, bref, 1, 256, true));
  }
  if (ng_ran_qos_support_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ng_ran_qos_support_list, bref, 1, 256, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qos_params_support_list_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutran_qos_support_list.size() > 0) {
    j.start_array("eUTRAN-QoS-Support-List");
    for (const auto& e1 : eutran_qos_support_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ng_ran_qos_support_list.size() > 0) {
    j.start_array("nG-RAN-QoS-Support-List");
    for (const auto& e1 : ng_ran_qos_support_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// TNLAssociationUsage ::= ENUMERATED
const char* tnl_assoc_usage_opts::to_string() const
{
  static const char* names[] = {"ue", "non-ue", "both"};
  return convert_enum_idx(names, 3, value, "tnl_assoc_usage_e");
}

// Transport-UP-Layer-Addresses-Info-To-Add-Item ::= SEQUENCE
SRSASN_CODE transport_up_layer_addresses_info_to_add_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gtp_transport_layer_addresses_to_add.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(ip_sec_transport_layer_address.pack(bref));
  if (gtp_transport_layer_addresses_to_add.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gtp_transport_layer_addresses_to_add, 1, 16, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE transport_up_layer_addresses_info_to_add_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool gtp_transport_layer_addresses_to_add_present;
  HANDLE_CODE(bref.unpack(gtp_transport_layer_addresses_to_add_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(ip_sec_transport_layer_address.unpack(bref));
  if (gtp_transport_layer_addresses_to_add_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gtp_transport_layer_addresses_to_add, bref, 1, 16, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void transport_up_layer_addresses_info_to_add_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("iP-SecTransportLayerAddress", ip_sec_transport_layer_address.to_string());
  if (gtp_transport_layer_addresses_to_add.size() > 0) {
    j.start_array("gTPTransportLayerAddressesToAdd");
    for (const auto& e1 : gtp_transport_layer_addresses_to_add) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Transport-UP-Layer-Addresses-Info-To-Remove-Item ::= SEQUENCE
SRSASN_CODE transport_up_layer_addresses_info_to_rem_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gtp_transport_layer_addresses_to_rem.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(ip_sec_transport_layer_address.pack(bref));
  if (gtp_transport_layer_addresses_to_rem.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gtp_transport_layer_addresses_to_rem, 1, 16, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE transport_up_layer_addresses_info_to_rem_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool gtp_transport_layer_addresses_to_rem_present;
  HANDLE_CODE(bref.unpack(gtp_transport_layer_addresses_to_rem_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(ip_sec_transport_layer_address.unpack(bref));
  if (gtp_transport_layer_addresses_to_rem_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gtp_transport_layer_addresses_to_rem, bref, 1, 16, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void transport_up_layer_addresses_info_to_rem_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("iP-SecTransportLayerAddress", ip_sec_transport_layer_address.to_string());
  if (gtp_transport_layer_addresses_to_rem.size() > 0) {
    j.start_array("gTPTransportLayerAddressesToRemove");
    for (const auto& e1 : gtp_transport_layer_addresses_to_rem) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-Failed-To-Setup-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnl_a_failed_to_setup_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnl_a_failed_to_setup_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.unpack(bref));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnl_a_failed_to_setup_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnl_assoc_transport_layer_address.to_json(j);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-Setup-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnl_a_setup_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnl_a_setup_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnl_a_setup_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnl_assoc_transport_layer_address.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-To-Add-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnl_a_to_add_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.pack(bref));
  HANDLE_CODE(tnl_assoc_usage.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnl_a_to_add_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.unpack(bref));
  HANDLE_CODE(tnl_assoc_usage.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnl_a_to_add_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnl_assoc_transport_layer_address.to_json(j);
  j.write_str("tNLAssociationUsage", tnl_assoc_usage.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-To-Remove-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnl_a_to_rem_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.pack(bref));
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnl_a_to_rem_item_s::unpack(cbit_ref& bref)
{
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnl_a_to_rem_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnl_assoc_transport_layer_address.to_json(j);
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-To-Update-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnl_a_to_upd_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tnl_assoc_usage_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.pack(bref));
  if (tnl_assoc_usage_present) {
    HANDLE_CODE(tnl_assoc_usage.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnl_a_to_upd_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tnl_assoc_usage_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.unpack(bref));
  if (tnl_assoc_usage_present) {
    HANDLE_CODE(tnl_assoc_usage.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnl_a_to_upd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnl_assoc_transport_layer_address.to_json(j);
  if (tnl_assoc_usage_present) {
    j.write_str("tNLAssociationUsage", tnl_assoc_usage.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-UP-TNLA-To-Remove-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_up_tnl_a_to_rem_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tnl_assoc_transport_layer_address_gnb_cu_cp_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.pack(bref));
  if (tnl_assoc_transport_layer_address_gnb_cu_cp_present) {
    HANDLE_CODE(tnl_assoc_transport_layer_address_gnb_cu_cp.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_tnl_a_to_rem_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tnl_assoc_transport_layer_address_gnb_cu_cp_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnl_assoc_transport_layer_address.unpack(bref));
  if (tnl_assoc_transport_layer_address_gnb_cu_cp_present) {
    HANDLE_CODE(tnl_assoc_transport_layer_address_gnb_cu_cp.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_up_tnl_a_to_rem_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnl_assoc_transport_layer_address.to_json(j);
  if (tnl_assoc_transport_layer_address_gnb_cu_cp_present) {
    j.write_fieldname("tNLAssociationTransportLayerAddressgNBCUCP");
    tnl_assoc_transport_layer_address_gnb_cu_cp.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// IAB-Donor-CU-UPPSKInfo-Item ::= SEQUENCE
SRSASN_CODE iab_donor_cu_up_psk_info_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(iab_donor_cu_up_psk.pack(bref));
  HANDLE_CODE(iab_donor_cu_up_ip_address.pack(bref));
  HANDLE_CODE(iab_du_ip_address.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_donor_cu_up_psk_info_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(iab_donor_cu_up_psk.unpack(bref));
  HANDLE_CODE(iab_donor_cu_up_ip_address.unpack(bref));
  HANDLE_CODE(iab_du_ip_address.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void iab_donor_cu_up_psk_info_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("iAB-donor-CU-UPPSK", iab_donor_cu_up_psk.to_string());
  j.write_str("iAB-donor-CU-UPIPAddress", iab_donor_cu_up_ip_address.to_string());
  j.write_str("iAB-DUIPAddress", iab_du_ip_address.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-Data-Usage-Item ::= SEQUENCE
SRSASN_CODE pdu_session_res_data_usage_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(mrdc_usage_info.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_data_usage_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(pdu_session_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(mrdc_usage_info.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdu_session_res_data_usage_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pDU-Session-ID", pdu_session_id);
  j.write_fieldname("mRDC-Usage-Information");
  mrdc_usage_info.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// ULUPTNLAddressToUpdateItem ::= SEQUENCE
SRSASN_CODE ul_up_tnl_address_to_upd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(old_tnl_adress.pack(bref));
  HANDLE_CODE(new_tnl_adress.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_up_tnl_address_to_upd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(old_tnl_adress.unpack(bref));
  HANDLE_CODE(new_tnl_adress.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_up_tnl_address_to_upd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("oldTNLAdress", old_tnl_adress.to_string());
  j.write_str("newTNLAdress", new_tnl_adress.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// CNSupport ::= ENUMERATED
const char* cn_support_opts::to_string() const
{
  static const char* names[] = {"c-epc", "c-5gc", "both"};
  return convert_enum_idx(names, 3, value, "cn_support_e");
}
int8_t cn_support_opts::to_number() const
{
  if (value == c_5gc) {
    return -5;
  }
  invalid_enum_number(value, "cn_support_e");
  return 0;
}

// Extended-GNB-CU-CP-Name ::= SEQUENCE
SRSASN_CODE extended_gnb_cu_cp_name_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gnb_cu_cp_name_visible_string_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_cp_name_utf8_string_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (gnb_cu_cp_name_visible_string_present) {
    HANDLE_CODE(gnb_cu_cp_name_visible_string.pack(bref));
  }
  if (gnb_cu_cp_name_utf8_string_present) {
    HANDLE_CODE(gnb_cu_cp_name_utf8_string.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE extended_gnb_cu_cp_name_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gnb_cu_cp_name_visible_string_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_cp_name_utf8_string_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (gnb_cu_cp_name_visible_string_present) {
    HANDLE_CODE(gnb_cu_cp_name_visible_string.unpack(bref));
  }
  if (gnb_cu_cp_name_utf8_string_present) {
    HANDLE_CODE(gnb_cu_cp_name_utf8_string.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void extended_gnb_cu_cp_name_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnb_cu_cp_name_visible_string_present) {
    j.write_str("gNB-CU-CP-NameVisibleString", gnb_cu_cp_name_visible_string.to_string());
  }
  if (gnb_cu_cp_name_utf8_string_present) {
    j.write_str("gNB-CU-CP-NameUTF8String", gnb_cu_cp_name_utf8_string.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Extended-GNB-CU-UP-Name ::= SEQUENCE
SRSASN_CODE extended_gnb_cu_up_name_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gnb_cu_up_name_visible_string_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_up_name_utf8_string_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (gnb_cu_up_name_visible_string_present) {
    HANDLE_CODE(gnb_cu_up_name_visible_string.pack(bref));
  }
  if (gnb_cu_up_name_utf8_string_present) {
    HANDLE_CODE(gnb_cu_up_name_utf8_string.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE extended_gnb_cu_up_name_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gnb_cu_up_name_visible_string_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_up_name_utf8_string_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (gnb_cu_up_name_visible_string_present) {
    HANDLE_CODE(gnb_cu_up_name_visible_string.unpack(bref));
  }
  if (gnb_cu_up_name_utf8_string_present) {
    HANDLE_CODE(gnb_cu_up_name_utf8_string.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void extended_gnb_cu_up_name_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnb_cu_up_name_visible_string_present) {
    j.write_str("gNB-CU-UP-NameVisibleString", gnb_cu_up_name_visible_string.to_string());
  }
  if (gnb_cu_up_name_utf8_string_present) {
    j.write_str("gNB-CU-UP-NameUTF8String", gnb_cu_up_name_utf8_string.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-UP-OverloadInformation ::= ENUMERATED
const char* gnb_cu_up_overload_info_opts::to_string() const
{
  static const char* names[] = {"overloaded", "not-overloaded"};
  return convert_enum_idx(names, 2, value, "gnb_cu_up_overload_info_e");
}

// HW-CapacityIndicator ::= SEQUENCE
SRSASN_CODE hw_capacity_ind_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, offered_throughput, (uint32_t)1u, (uint32_t)16777216u, true, true));
  HANDLE_CODE(pack_integer(bref, available_throughput, (uint8_t)0u, (uint8_t)100u, true, true));
  HANDLE_CODE(ie_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE hw_capacity_ind_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(offered_throughput, bref, (uint32_t)1u, (uint32_t)16777216u, true, true));
  HANDLE_CODE(unpack_integer(available_throughput, bref, (uint8_t)0u, (uint8_t)100u, true, true));
  HANDLE_CODE(ie_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void hw_capacity_ind_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("offeredThroughput", offered_throughput);
  j.write_int("availableThroughput", available_throughput);
  j.write_fieldname("iE-Extensions");
  ie_exts.to_json(j);
  j.end_obj();
}

// RegistrationRequest ::= ENUMERATED
const char* regist_request_opts::to_string() const
{
  static const char* names[] = {"start", "stop"};
  return convert_enum_idx(names, 2, value, "regist_request_e");
}

// ReportingPeriodicity ::= ENUMERATED
const char* report_periodicity_opts::to_string() const
{
  static const char* names[] = {"ms500",
                                "ms1000",
                                "ms2000",
                                "ms5000",
                                "ms10000",
                                "ms20000",
                                "ms30000",
                                "ms40000",
                                "ms50000",
                                "ms60000",
                                "ms70000",
                                "ms80000",
                                "ms90000",
                                "ms100000",
                                "ms110000",
                                "ms120000"};
  return convert_enum_idx(names, 16, value, "report_periodicity_e");
}
uint32_t report_periodicity_opts::to_number() const
{
  static const uint32_t numbers[] = {
      500, 1000, 2000, 5000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000, 110000, 120000};
  return map_enum_number(numbers, 16, value, "report_periodicity_e");
}

// TNL-AvailableCapacityIndicator ::= SEQUENCE
SRSASN_CODE tnl_available_capacity_ind_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, dl_tnl_offered_capacity, (uint32_t)0u, (uint32_t)16777216u, true, true));
  HANDLE_CODE(pack_integer(bref, dl_tnl_available_capacity, (uint8_t)0u, (uint8_t)100u, true, true));
  HANDLE_CODE(pack_integer(bref, ul_tnl_offered_capacity, (uint32_t)0u, (uint32_t)16777216u, true, true));
  HANDLE_CODE(pack_integer(bref, ul_tnl_available_capacity, (uint8_t)0u, (uint8_t)100u, true, true));
  HANDLE_CODE(ie_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tnl_available_capacity_ind_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(dl_tnl_offered_capacity, bref, (uint32_t)0u, (uint32_t)16777216u, true, true));
  HANDLE_CODE(unpack_integer(dl_tnl_available_capacity, bref, (uint8_t)0u, (uint8_t)100u, true, true));
  HANDLE_CODE(unpack_integer(ul_tnl_offered_capacity, bref, (uint32_t)0u, (uint32_t)16777216u, true, true));
  HANDLE_CODE(unpack_integer(ul_tnl_available_capacity, bref, (uint8_t)0u, (uint8_t)100u, true, true));
  HANDLE_CODE(ie_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void tnl_available_capacity_ind_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dL-TNL-OfferedCapacity", dl_tnl_offered_capacity);
  j.write_int("dL-TNL-AvailableCapacity", dl_tnl_available_capacity);
  j.write_int("uL-TNL-OfferedCapacity", ul_tnl_offered_capacity);
  j.write_int("uL-TNL-AvailableCapacity", ul_tnl_available_capacity);
  j.write_fieldname("iE-Extensions");
  ie_exts.to_json(j);
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

// Transport-Layer-Address-Info ::= SEQUENCE
SRSASN_CODE transport_layer_address_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(transport_up_layer_addresses_info_to_add_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(transport_up_layer_addresses_info_to_rem_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (transport_up_layer_addresses_info_to_add_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, transport_up_layer_addresses_info_to_add_list, 1, 16, true));
  }
  if (transport_up_layer_addresses_info_to_rem_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, transport_up_layer_addresses_info_to_rem_list, 1, 16, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE transport_layer_address_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool transport_up_layer_addresses_info_to_add_list_present;
  HANDLE_CODE(bref.unpack(transport_up_layer_addresses_info_to_add_list_present, 1));
  bool transport_up_layer_addresses_info_to_rem_list_present;
  HANDLE_CODE(bref.unpack(transport_up_layer_addresses_info_to_rem_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (transport_up_layer_addresses_info_to_add_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(transport_up_layer_addresses_info_to_add_list, bref, 1, 16, true));
  }
  if (transport_up_layer_addresses_info_to_rem_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(transport_up_layer_addresses_info_to_rem_list, bref, 1, 16, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void transport_layer_address_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (transport_up_layer_addresses_info_to_add_list.size() > 0) {
    j.start_array("transport-UP-Layer-Addresses-Info-To-Add-List");
    for (const auto& e1 : transport_up_layer_addresses_info_to_add_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (transport_up_layer_addresses_info_to_rem_list.size() > 0) {
    j.start_array("transport-UP-Layer-Addresses-Info-To-Remove-List");
    for (const auto& e1 : transport_up_layer_addresses_info_to_rem_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}
