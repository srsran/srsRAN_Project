/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/asn1/e1ap.h"
#include <sstream>

using namespace asn1;
using namespace asn1::e1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// DRB-Activity ::= ENUMERATED
const char* drb_activity_opts::to_string() const
{
  static const char* options[] = {"active", "not-active"};
  return convert_enum_idx(options, 2, value, "drb_activity_e");
}

// PDU-Session-Resource-Activity ::= ENUMERATED
const char* pdu_session_res_activity_opts::to_string() const
{
  static const char* options[] = {"active", "not-active"};
  return convert_enum_idx(options, 2, value, "pdu_session_res_activity_e");
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
  static const char* options[] = {"active", "not-active"};
  return convert_enum_idx(options, 2, value, "ue_activity_e");
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
  static const char* options[] = {
      "dRB-Activity-List", "pDU-Session-Resource-Activity-List", "uE-Activity", "choice-extension"};
  return convert_enum_idx(options, 4, value, "activity_info_c::types");
}

// BearerContextInactivityNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_inactivity_notif_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 24};
  return map_enum_number(options, 3, idx, "id");
}
bool bearer_context_inactivity_notif_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 24};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_inactivity_notif_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::activity_info:
      c.destroy<activity_info_c>();
      break;
    default:
      break;
  }
}
void bearer_context_inactivity_notif_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::activity_info:
      c.init<activity_info_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_inactivity_notif_ies_o::value_c");
  }
}
bearer_context_inactivity_notif_ies_o::value_c::value_c(const bearer_context_inactivity_notif_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::activity_info:
      c.init(other.c.get<activity_info_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_inactivity_notif_ies_o::value_c");
  }
}
bearer_context_inactivity_notif_ies_o::value_c&
bearer_context_inactivity_notif_ies_o::value_c::operator=(const bearer_context_inactivity_notif_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::activity_info:
      c.set(other.c.get<activity_info_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_inactivity_notif_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_inactivity_notif_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_inactivity_notif_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
activity_info_c& bearer_context_inactivity_notif_ies_o::value_c::activity_info()
{
  assert_choice_type(types::activity_info, type_, "Value");
  return c.get<activity_info_c>();
}
const uint64_t& bearer_context_inactivity_notif_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_inactivity_notif_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "ActivityInformation"};
  return convert_enum_idx(options, 3, value, "bearer_context_inactivity_notif_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_inactivity_notif_ies_o>;

bearer_context_inactivity_notif_ies_container::bearer_context_inactivity_notif_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject), gnb_cu_up_ue_e1_ap_id(3, crit_e::reject), activity_info(24, crit_e::reject)
{
}
SRSASN_CODE bearer_context_inactivity_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(activity_info.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 24: {
        nof_mandatory_ies--;
        activity_info.id = id;
        HANDLE_CODE(activity_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(activity_info.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  activity_info.to_json(j);
  j.end_obj();
}

// DL-TX-Stop ::= ENUMERATED
const char* dl_tx_stop_opts::to_string() const
{
  static const char* options[] = {"stop", "resume"};
  return convert_enum_idx(options, 2, value, "dl_tx_stop_e");
}

// RAT-Type ::= ENUMERATED
const char* rat_type_opts::to_string() const
{
  static const char* options[] = {"e-UTRA", "nR"};
  return convert_enum_idx(options, 2, value, "rat_type_e");
}

// UL-Configuration ::= ENUMERATED
const char* ul_cfg_opts::to_string() const
{
  static const char* options[] = {"no-data", "shared", "only"};
  return convert_enum_idx(options, 3, value, "ul_cfg_e");
}

// Cell-Group-Information-Item ::= SEQUENCE
SRSASN_CODE cell_group_info_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_cfg_present, 1));
  HANDLE_CODE(bref.pack(dl_tx_stop_present, 1));
  HANDLE_CODE(bref.pack(rat_type_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

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
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_group_info_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_cfg_present, 1));
  HANDLE_CODE(bref.unpack(dl_tx_stop_present, 1));
  HANDLE_CODE(bref.unpack(rat_type_present, 1));
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
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
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

// EUTRAN-BearerContextModificationConfirm ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_mod_confirm_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {41};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"DRB-Confirm-Modified-List-EUTRAN"};
  return convert_enum_idx(options, 1, value, "eutran_bearer_context_mod_confirm_o::value_c::types");
}

// NG-RAN-BearerContextModificationConfirm ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_mod_confirm_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {50};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"PDU-Session-Resource-Confirm-Modified-List"};
  return convert_enum_idx(options, 1, value, "ng_ran_bearer_context_mod_confirm_o::value_c::types");
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
  static const char* options[] = {
      "e-UTRAN-BearerContextModificationConfirm", "nG-RAN-BearerContextModificationConfirm", "choice-extension"};
  return convert_enum_idx(options, 3, value, "sys_bearer_context_mod_confirm_c::types");
}

// BearerContextModificationConfirmIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_confirm_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 20};
  return map_enum_number(options, 3, idx, "id");
}
bool bearer_context_mod_confirm_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 20};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_mod_confirm_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::sys_bearer_context_mod_confirm:
      c.destroy<sys_bearer_context_mod_confirm_c>();
      break;
    default:
      break;
  }
}
void bearer_context_mod_confirm_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::sys_bearer_context_mod_confirm:
      c.init<sys_bearer_context_mod_confirm_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_confirm_ies_o::value_c");
  }
}
bearer_context_mod_confirm_ies_o::value_c::value_c(const bearer_context_mod_confirm_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_confirm:
      c.init(other.c.get<sys_bearer_context_mod_confirm_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_confirm_ies_o::value_c");
  }
}
bearer_context_mod_confirm_ies_o::value_c&
bearer_context_mod_confirm_ies_o::value_c::operator=(const bearer_context_mod_confirm_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_confirm:
      c.set(other.c.get<sys_bearer_context_mod_confirm_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_confirm_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_mod_confirm_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_confirm_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_bearer_context_mod_confirm_c& bearer_context_mod_confirm_ies_o::value_c::sys_bearer_context_mod_confirm()
{
  assert_choice_type(types::sys_bearer_context_mod_confirm, type_, "Value");
  return c.get<sys_bearer_context_mod_confirm_c>();
}
const uint64_t& bearer_context_mod_confirm_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_confirm_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-BearerContextModificationConfirm"};
  return convert_enum_idx(options, 3, value, "bearer_context_mod_confirm_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_confirm_ies_o>;

bearer_context_mod_confirm_ies_container::bearer_context_mod_confirm_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  sys_bearer_context_mod_confirm(20, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_mod_confirm_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += sys_bearer_context_mod_confirm_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  if (sys_bearer_context_mod_confirm_present) {
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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 20: {
        sys_bearer_context_mod_confirm_present = true;
        sys_bearer_context_mod_confirm.id      = id;
        HANDLE_CODE(sys_bearer_context_mod_confirm.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_mod_confirm.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  if (sys_bearer_context_mod_confirm_present) {
    j.write_fieldname("");
    sys_bearer_context_mod_confirm.to_json(j);
  }
  j.end_obj();
}

// TypeOfError ::= ENUMERATED
const char* type_of_error_opts::to_string() const
{
  static const char* options[] = {"not-understood", "missing"};
  return convert_enum_idx(options, 2, value, "type_of_error_e");
}

// CauseMisc ::= ENUMERATED
const char* cause_misc_opts::to_string() const
{
  static const char* options[] = {"control-processing-overload",
                                  "not-enough-user-plane-processing-resources",
                                  "hardware-failure",
                                  "om-intervention",
                                  "unspecified"};
  return convert_enum_idx(options, 5, value, "cause_misc_e");
}

// CauseProtocol ::= ENUMERATED
const char* cause_protocol_opts::to_string() const
{
  static const char* options[] = {"transfer-syntax-error",
                                  "abstract-syntax-error-reject",
                                  "abstract-syntax-error-ignore-and-notify",
                                  "message-not-compatible-with-receiver-state",
                                  "semantic-error",
                                  "abstract-syntax-error-falsely-constructed-message",
                                  "unspecified"};
  return convert_enum_idx(options, 7, value, "cause_protocol_e");
}

// CauseRadioNetwork ::= ENUMERATED
const char* cause_radio_network_opts::to_string() const
{
  static const char* options[] = {"unspecified",
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
                                  "release-due-to-pre-emption"};
  return convert_enum_idx(options, 28, value, "cause_radio_network_e");
}

// CauseTransport ::= ENUMERATED
const char* cause_transport_opts::to_string() const
{
  static const char* options[] = {"unspecified", "transport-resource-unavailable"};
  return convert_enum_idx(options, 2, value, "cause_transport_e");
}

SRSASN_CODE crit_diagnostics_ie_list_item_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(iecrit.pack(bref));
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

  HANDLE_CODE(iecrit.unpack(bref));
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
  j.write_str("iECriticality", iecrit.to_string());
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
  static const char* options[] = {"initiating-message", "successful-outcome", "unsuccessful-outcome"};
  return convert_enum_idx(options, 3, value, "trigger_msg_e");
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
  static const char* options[] = {"radioNetwork", "transport", "protocol", "misc", "choice-extension"};
  return convert_enum_idx(options, 5, value, "cause_c::types");
}
uint8_t cause_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "cause_c::types");
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

// BearerContextModificationFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 0, 1};
  return map_enum_number(options, 4, idx, "id");
}
bool bearer_context_mod_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 0, 1};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_mod_fail_ies_o::value_c::destroy_()
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
void bearer_context_mod_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "bearer_context_mod_fail_ies_o::value_c");
  }
}
bearer_context_mod_fail_ies_o::value_c::value_c(const bearer_context_mod_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "bearer_context_mod_fail_ies_o::value_c");
  }
}
bearer_context_mod_fail_ies_o::value_c&
bearer_context_mod_fail_ies_o::value_c::operator=(const bearer_context_mod_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "bearer_context_mod_fail_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_mod_fail_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_fail_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
const uint64_t& bearer_context_mod_fail_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_fail_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "bearer_context_mod_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_fail_ies_o>;

bearer_context_mod_fail_ies_container::bearer_context_mod_fail_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  cause(0, crit_e::ignore),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_mod_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (crit_diagnostics_present) {
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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
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
      case 1: {
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
void bearer_context_mod_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
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

// Dynamic5QIDescriptor ::= SEQUENCE
SRSASN_CODE dynamic5_qi_descriptor_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(five_qi_present, 1));
  HANDLE_CODE(bref.pack(delay_crit_present, 1));
  HANDLE_CODE(bref.pack(averaging_win_present, 1));
  HANDLE_CODE(bref.pack(max_data_burst_volume_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qo_sprio_level, (uint8_t)0u, (uint8_t)127u, true, true));
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
SRSASN_CODE dynamic5_qi_descriptor_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(five_qi_present, 1));
  HANDLE_CODE(bref.unpack(delay_crit_present, 1));
  HANDLE_CODE(bref.unpack(averaging_win_present, 1));
  HANDLE_CODE(bref.unpack(max_data_burst_volume_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qo_sprio_level, bref, (uint8_t)0u, (uint8_t)127u, true, true));
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
void dynamic5_qi_descriptor_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoSPriorityLevel", qo_sprio_level);
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

const char* dynamic5_qi_descriptor_s::delay_crit_opts::to_string() const
{
  static const char* options[] = {"delay-critical", "non-delay-critical"};
  return convert_enum_idx(options, 2, value, "dynamic5_qi_descriptor_s::delay_crit_e_");
}

// Non-Dynamic5QIDescriptor ::= SEQUENCE
SRSASN_CODE non_dynamic5_qi_descriptor_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(qo_sprio_level_present, 1));
  HANDLE_CODE(bref.pack(averaging_win_present, 1));
  HANDLE_CODE(bref.pack(max_data_burst_volume_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, five_qi, (uint16_t)0u, (uint16_t)255u, true, true));
  if (qo_sprio_level_present) {
    HANDLE_CODE(pack_integer(bref, qo_sprio_level, (uint8_t)0u, (uint8_t)127u, true, true));
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
SRSASN_CODE non_dynamic5_qi_descriptor_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(qo_sprio_level_present, 1));
  HANDLE_CODE(bref.unpack(averaging_win_present, 1));
  HANDLE_CODE(bref.unpack(max_data_burst_volume_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(five_qi, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  if (qo_sprio_level_present) {
    HANDLE_CODE(unpack_integer(qo_sprio_level, bref, (uint8_t)0u, (uint8_t)127u, true, true));
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
void non_dynamic5_qi_descriptor_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("fiveQI", five_qi);
  if (qo_sprio_level_present) {
    j.write_int("qoSPriorityLevel", qo_sprio_level);
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
  static const char* options[] = {"shall-not-trigger-pre-emption", "may-trigger-pre-emption"};
  return convert_enum_idx(options, 2, value, "pre_emption_cap_e");
}

// Pre-emptionVulnerability ::= ENUMERATED
const char* pre_emption_vulnerability_opts::to_string() const
{
  static const char* options[] = {"not-pre-emptable", "pre-emptable"};
  return convert_enum_idx(options, 2, value, "pre_emption_vulnerability_e");
}

// QoS-Flow-Mapping-Indication ::= ENUMERATED
const char* qo_s_flow_map_ind_opts::to_string() const
{
  static const char* options[] = {"ul", "dl"};
  return convert_enum_idx(options, 2, value, "qo_s_flow_map_ind_e");
}

// GBR-QoSFlowInformation ::= SEQUENCE
SRSASN_CODE gbr_qosflow_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(max_packet_loss_rate_dl_present, 1));
  HANDLE_CODE(bref.pack(max_packet_loss_rate_ul_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

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
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gbr_qosflow_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(max_packet_loss_rate_dl_present, 1));
  HANDLE_CODE(bref.unpack(max_packet_loss_rate_ul_present, 1));
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
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gbr_qosflow_info_s::to_json(json_writer& j) const
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
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

// QoS-Characteristics ::= CHOICE
void qo_s_characteristics_c::destroy_()
{
  switch (type_) {
    case types::non_dynamic_minus5_qi:
      c.destroy<non_dynamic5_qi_descriptor_s>();
      break;
    case types::dynamic_minus5_qi:
      c.destroy<dynamic5_qi_descriptor_s>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void qo_s_characteristics_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::non_dynamic_minus5_qi:
      c.init<non_dynamic5_qi_descriptor_s>();
      break;
    case types::dynamic_minus5_qi:
      c.init<dynamic5_qi_descriptor_s>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qo_s_characteristics_c");
  }
}
qo_s_characteristics_c::qo_s_characteristics_c(const qo_s_characteristics_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::non_dynamic_minus5_qi:
      c.init(other.c.get<non_dynamic5_qi_descriptor_s>());
      break;
    case types::dynamic_minus5_qi:
      c.init(other.c.get<dynamic5_qi_descriptor_s>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qo_s_characteristics_c");
  }
}
qo_s_characteristics_c& qo_s_characteristics_c::operator=(const qo_s_characteristics_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::non_dynamic_minus5_qi:
      c.set(other.c.get<non_dynamic5_qi_descriptor_s>());
      break;
    case types::dynamic_minus5_qi:
      c.set(other.c.get<dynamic5_qi_descriptor_s>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qo_s_characteristics_c");
  }

  return *this;
}
non_dynamic5_qi_descriptor_s& qo_s_characteristics_c::set_non_dynamic_minus5_qi()
{
  set(types::non_dynamic_minus5_qi);
  return c.get<non_dynamic5_qi_descriptor_s>();
}
dynamic5_qi_descriptor_s& qo_s_characteristics_c::set_dynamic_minus5_qi()
{
  set(types::dynamic_minus5_qi);
  return c.get<dynamic5_qi_descriptor_s>();
}
protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>& qo_s_characteristics_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>>();
}
void qo_s_characteristics_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::non_dynamic_minus5_qi:
      j.write_fieldname("non-Dynamic-5QI");
      c.get<non_dynamic5_qi_descriptor_s>().to_json(j);
      break;
    case types::dynamic_minus5_qi:
      j.write_fieldname("dynamic-5QI");
      c.get<dynamic5_qi_descriptor_s>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "qo_s_characteristics_c");
  }
  j.end_obj();
}
SRSASN_CODE qo_s_characteristics_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::non_dynamic_minus5_qi:
      HANDLE_CODE(c.get<non_dynamic5_qi_descriptor_s>().pack(bref));
      break;
    case types::dynamic_minus5_qi:
      HANDLE_CODE(c.get<dynamic5_qi_descriptor_s>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "qo_s_characteristics_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE qo_s_characteristics_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::non_dynamic_minus5_qi:
      HANDLE_CODE(c.get<non_dynamic5_qi_descriptor_s>().unpack(bref));
      break;
    case types::dynamic_minus5_qi:
      HANDLE_CODE(c.get<dynamic5_qi_descriptor_s>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<qo_s_characteristics_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "qo_s_characteristics_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* qo_s_characteristics_c::types_opts::to_string() const
{
  static const char* options[] = {"non-Dynamic-5QI", "dynamic-5QI", "choice-extension"};
  return convert_enum_idx(options, 3, value, "qo_s_characteristics_c::types");
}

// QoS-Flow-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t qo_s_flow_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {80};
  return map_enum_number(options, 1, idx, "id");
}
bool qo_s_flow_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 80 == id;
}
crit_e qo_s_flow_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 80) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
qo_s_flow_item_ext_ies_o::ext_c qo_s_flow_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 80) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e qo_s_flow_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 80) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void qo_s_flow_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("QoS-Flow-Mapping-Indication", c.to_string());
  j.end_obj();
}
SRSASN_CODE qo_s_flow_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE qo_s_flow_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* qo_s_flow_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* options[] = {"QoS-Flow-Mapping-Indication"};
  return convert_enum_idx(options, 1, value, "qo_s_flow_item_ext_ies_o::ext_c::types");
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

// QoS-Flow-Item ::= SEQUENCE
SRSASN_CODE qo_s_flow_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, qo_s_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qo_s_flow_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qo_s_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void qo_s_flow_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qo_s_flow_id);
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// QoS-Flow-Mapping-Item ::= SEQUENCE
SRSASN_CODE qo_s_flow_map_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qo_sflow_map_ind_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qo_s_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  if (qo_sflow_map_ind_present) {
    HANDLE_CODE(qo_sflow_map_ind.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qo_s_flow_map_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(qo_sflow_map_ind_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qo_s_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  if (qo_sflow_map_ind_present) {
    HANDLE_CODE(qo_sflow_map_ind.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qo_s_flow_map_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qo_s_flow_id);
  if (qo_sflow_map_ind_present) {
    j.write_str("qoSFlowMappingIndication", qo_sflow_map_ind.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// QoSFlowLevelQoSParameters ::= SEQUENCE
SRSASN_CODE qo_sflow_level_qos_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gbr_qos_flow_info_present, 1));
  HANDLE_CODE(bref.pack(reflective_qos_attribute_present, 1));
  HANDLE_CODE(bref.pack(add_qos_info_present, 1));
  HANDLE_CODE(bref.pack(paging_policy_ind_present, 1));
  HANDLE_CODE(bref.pack(reflective_qos_ind_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(qo_s_characteristics.pack(bref));
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
SRSASN_CODE qo_sflow_level_qos_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gbr_qos_flow_info_present, 1));
  HANDLE_CODE(bref.unpack(reflective_qos_attribute_present, 1));
  HANDLE_CODE(bref.unpack(add_qos_info_present, 1));
  HANDLE_CODE(bref.unpack(paging_policy_ind_present, 1));
  HANDLE_CODE(bref.unpack(reflective_qos_ind_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(qo_s_characteristics.unpack(bref));
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
void qo_sflow_level_qos_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("qoS-Characteristics");
  qo_s_characteristics.to_json(j);
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

const char* qo_sflow_level_qos_params_s::reflective_qos_attribute_opts::to_string() const
{
  static const char* options[] = {"subject-to"};
  return convert_enum_idx(options, 1, value, "qo_sflow_level_qos_params_s::reflective_qos_attribute_e_");
}

const char* qo_sflow_level_qos_params_s::add_qos_info_opts::to_string() const
{
  static const char* options[] = {"more-likely"};
  return convert_enum_idx(options, 1, value, "qo_sflow_level_qos_params_s::add_qos_info_e_");
}

const char* qo_sflow_level_qos_params_s::reflective_qos_ind_opts::to_string() const
{
  static const char* options[] = {"enabled"};
  return convert_enum_idx(options, 1, value, "qo_sflow_level_qos_params_s::reflective_qos_ind_e_");
}

// ROHC ::= SEQUENCE
SRSASN_CODE rohc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(continue_rohc_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, max_cid, (uint16_t)0u, (uint16_t)16383u, true, true));
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

  HANDLE_CODE(unpack_integer(max_cid, bref, (uint16_t)0u, (uint16_t)16383u, true, true));
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
  j.write_int("maxCID", max_cid);
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
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "rohc_s::continue_rohc_e_");
}

// T-Reordering ::= ENUMERATED
const char* t_reordering_opts::to_string() const
{
  static const char* options[] = {"ms0",    "ms1",    "ms2",    "ms4",    "ms5",    "ms8",    "ms10",   "ms15",
                                  "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms120",
                                  "ms140",  "ms160",  "ms180",  "ms200",  "ms220",  "ms240",  "ms260",  "ms280",
                                  "ms300",  "ms500",  "ms750",  "ms1000", "ms1250", "ms1500", "ms1750", "ms2000",
                                  "ms2250", "ms2500", "ms2750", "ms3000"};
  return convert_enum_idx(options, 36, value, "t_reordering_e");
}
uint16_t t_reordering_opts::to_number() const
{
  static const uint16_t options[] = {0,   1,   2,   4,    5,    8,    10,   15,   20,   30,   40,   50,
                                     60,  80,  100, 120,  140,  160,  180,  200,  220,  240,  260,  280,
                                     300, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000};
  return map_enum_number(options, 36, value, "t_reordering_e");
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
  static const char* options[] = {"gTPTunnel", "choice-extension"};
  return convert_enum_idx(options, 2, value, "up_tnl_info_c::types");
}

// UplinkOnlyROHC ::= SEQUENCE
SRSASN_CODE ul_only_rohc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(continue_rohc_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, max_cid, (uint16_t)0u, (uint16_t)16383u, true, true));
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

  HANDLE_CODE(unpack_integer(max_cid, bref, (uint16_t)0u, (uint16_t)16383u, true, true));
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
  j.write_int("maxCID", max_cid);
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
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "ul_only_rohc_s::continue_rohc_e_");
}

// DRBBStatusTransfer ::= SEQUENCE
SRSASN_CODE drbb_status_transfer_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(receive_statusof_pdcpsdu_present, 1));
  HANDLE_CODE(bref.pack(ie_ext_present, 1));

  if (receive_statusof_pdcpsdu_present) {
    HANDLE_CODE(receive_statusof_pdcpsdu.pack(bref));
  }
  HANDLE_CODE(count_value.pack(bref));
  if (ie_ext_present) {
    HANDLE_CODE(ie_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drbb_status_transfer_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(receive_statusof_pdcpsdu_present, 1));
  HANDLE_CODE(bref.unpack(ie_ext_present, 1));

  if (receive_statusof_pdcpsdu_present) {
    HANDLE_CODE(receive_statusof_pdcpsdu.unpack(bref));
  }
  HANDLE_CODE(count_value.unpack(bref));
  if (ie_ext_present) {
    HANDLE_CODE(ie_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drbb_status_transfer_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (receive_statusof_pdcpsdu_present) {
    j.write_str("receiveStatusofPDCPSDU", receive_statusof_pdcpsdu.to_string());
  }
  j.write_fieldname("countValue");
  count_value.to_json(j);
  if (ie_ext_present) {
    j.write_fieldname("iE-Extension");
    ie_ext.to_json(j);
  }
  j.end_obj();
}

// Data-Forwarding-Request ::= ENUMERATED
const char* data_forwarding_request_opts::to_string() const
{
  static const char* options[] = {"uL", "dL", "both"};
  return convert_enum_idx(options, 3, value, "data_forwarding_request_e");
}

// DefaultDRB ::= ENUMERATED
const char* default_drb_opts::to_string() const
{
  static const char* options[] = {"true", "false"};
  return convert_enum_idx(options, 2, value, "default_drb_e");
}

// DiscardTimer ::= ENUMERATED
const char* discard_timer_opts::to_string() const
{
  static const char* options[] = {"ms10",
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
  return convert_enum_idx(options, 16, value, "discard_timer_e");
}
int16_t discard_timer_opts::to_number() const
{
  static const int16_t options[] = {10, 20, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(options, 16, value, "discard_timer_e");
}

// Duplication-Activation ::= ENUMERATED
const char* dupl_activation_opts::to_string() const
{
  static const char* options[] = {"active", "inactive"};
  return convert_enum_idx(options, 2, value, "dupl_activation_e");
}

// OutOfOrderDelivery ::= ENUMERATED
const char* out_of_order_delivery_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "out_of_order_delivery_e");
}

// PDCP-DataRecovery ::= ENUMERATED
const char* pdcp_data_recovery_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "pdcp_data_recovery_e");
}

// PDCP-Duplication ::= ENUMERATED
const char* pdcp_dupl_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "pdcp_dupl_e");
}

// PDCP-Reestablishment ::= ENUMERATED
const char* pdcp_reest_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "pdcp_reest_e");
}

// PDCP-SN-Size ::= ENUMERATED
const char* pdcp_sn_size_opts::to_string() const
{
  static const char* options[] = {"s-12", "s-18"};
  return convert_enum_idx(options, 2, value, "pdcp_sn_size_e");
}
int8_t pdcp_sn_size_opts::to_number() const
{
  static const int8_t options[] = {-12, -18};
  return map_enum_number(options, 2, value, "pdcp_sn_size_e");
}

// QoS-Flow-QoS-Parameter-Item ::= SEQUENCE
SRSASN_CODE qo_s_flow_qos_param_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qo_sflow_map_ind_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qo_s_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(qo_sflow_level_qos_params.pack(bref));
  if (qo_sflow_map_ind_present) {
    HANDLE_CODE(qo_sflow_map_ind.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qo_s_flow_qos_param_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(qo_sflow_map_ind_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qo_s_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(qo_sflow_level_qos_params.unpack(bref));
  if (qo_sflow_map_ind_present) {
    HANDLE_CODE(qo_sflow_map_ind.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qo_s_flow_qos_param_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qo_s_flow_id);
  j.write_fieldname("qoSFlowLevelQoSParameters");
  qo_sflow_level_qos_params.to_json(j);
  if (qo_sflow_map_ind_present) {
    j.write_str("qoSFlowMappingIndication", qo_sflow_map_ind.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// RLC-Mode ::= ENUMERATED
const char* rlc_mode_opts::to_string() const
{
  static const char* options[] = {
      "rlc-tm", "rlc-am", "rlc-um-bidirectional", "rlc-um-unidirectional-ul", "rlc-um-unidirectional-dl"};
  return convert_enum_idx(options, 5, value, "rlc_mode_e");
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
  static const char* options[] = {"rOHC", "uPlinkOnlyROHC", "choice-Extension"};
  return convert_enum_idx(options, 3, value, "rohc_params_c::types");
}

// SDAP-Header-DL ::= ENUMERATED
const char* sdap_hdr_dl_opts::to_string() const
{
  static const char* options[] = {"present", "absent"};
  return convert_enum_idx(options, 2, value, "sdap_hdr_dl_e");
}

// SDAP-Header-UL ::= ENUMERATED
const char* sdap_hdr_ul_opts::to_string() const
{
  static const char* options[] = {"present", "absent"};
  return convert_enum_idx(options, 2, value, "sdap_hdr_ul_e");
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
  static const char* options[] = {"b0",       "b100",     "b200",     "b400",     "b800",     "b1600",
                                  "b3200",    "b6400",    "b12800",   "b25600",   "b51200",   "b102400",
                                  "b204800",  "b409600",  "b819200",  "b1228800", "b1638400", "b2457600",
                                  "b3276800", "b4096000", "b4915200", "b5734400", "b6553600", "infinity"};
  return convert_enum_idx(options, 24, value, "ul_data_split_thres_e");
}
int32_t ul_data_split_thres_opts::to_number() const
{
  static const int32_t options[] = {0,       100,     200,     400,     800,     1600,    3200,    6400,
                                    12800,   25600,   51200,   102400,  204800,  409600,  819200,  1228800,
                                    1638400, 2457600, 3276800, 4096000, 4915200, 5734400, 6553600, -1};
  return map_enum_number(options, 24, value, "ul_data_split_thres_e");
}

// UP-Parameters-Item ::= SEQUENCE
SRSASN_CODE up_params_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(up_tnl_info.pack(bref));
  HANDLE_CODE(pack_integer(bref, cell_group_id, (uint8_t)0u, (uint8_t)3u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE up_params_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(up_tnl_info.unpack(bref));
  HANDLE_CODE(unpack_integer(cell_group_id, bref, (uint8_t)0u, (uint8_t)3u, true, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void up_params_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("uP-TNL-Information");
  up_tnl_info.to_json(j);
  j.write_int("cell-Group-ID", cell_group_id);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// DRB-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_to_modify_item_ng_ran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {71, 72};
  return map_enum_number(options, 2, idx, "id");
}
bool drb_to_modify_item_ng_ran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {71, 72};
  for (const auto& o : options) {
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
      ret.set(ext_c::types::old_qosflow_map_ulendmarkerexpected);
      break;
    case 72:
      ret.set(ext_c::types::drb_qos);
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void drb_to_modify_item_ng_ran_ext_ies_o::ext_c::destroy_()
{
  switch (type_) {
    case types::old_qosflow_map_ulendmarkerexpected:
      c.destroy<qo_s_flow_list_l>();
      break;
    case types::drb_qos:
      c.destroy<qo_sflow_level_qos_params_s>();
      break;
    default:
      break;
  }
}
void drb_to_modify_item_ng_ran_ext_ies_o::ext_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::old_qosflow_map_ulendmarkerexpected:
      c.init<qo_s_flow_list_l>();
      break;
    case types::drb_qos:
      c.init<qo_sflow_level_qos_params_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c");
  }
}
drb_to_modify_item_ng_ran_ext_ies_o::ext_c::ext_c(const drb_to_modify_item_ng_ran_ext_ies_o::ext_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::old_qosflow_map_ulendmarkerexpected:
      c.init(other.c.get<qo_s_flow_list_l>());
      break;
    case types::drb_qos:
      c.init(other.c.get<qo_sflow_level_qos_params_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c");
  }
}
drb_to_modify_item_ng_ran_ext_ies_o::ext_c&
drb_to_modify_item_ng_ran_ext_ies_o::ext_c::operator=(const drb_to_modify_item_ng_ran_ext_ies_o::ext_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::old_qosflow_map_ulendmarkerexpected:
      c.set(other.c.get<qo_s_flow_list_l>());
      break;
    case types::drb_qos:
      c.set(other.c.get<qo_sflow_level_qos_params_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c");
  }

  return *this;
}
qo_s_flow_list_l& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::old_qosflow_map_ulendmarkerexpected()
{
  assert_choice_type(types::old_qosflow_map_ulendmarkerexpected, type_, "Extension");
  return c.get<qo_s_flow_list_l>();
}
qo_sflow_level_qos_params_s& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::drb_qos()
{
  assert_choice_type(types::drb_qos, type_, "Extension");
  return c.get<qo_sflow_level_qos_params_s>();
}
const qo_s_flow_list_l& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::old_qosflow_map_ulendmarkerexpected() const
{
  assert_choice_type(types::old_qosflow_map_ulendmarkerexpected, type_, "Extension");
  return c.get<qo_s_flow_list_l>();
}
const qo_sflow_level_qos_params_s& drb_to_modify_item_ng_ran_ext_ies_o::ext_c::drb_qos() const
{
  assert_choice_type(types::drb_qos, type_, "Extension");
  return c.get<qo_sflow_level_qos_params_s>();
}
void drb_to_modify_item_ng_ran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::old_qosflow_map_ulendmarkerexpected:
      j.start_array("QoS-Flow-List");
      for (const auto& e1 : c.get<qo_s_flow_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_qos:
      j.write_fieldname("QoSFlowLevelQoSParameters");
      c.get<qo_sflow_level_qos_params_s>().to_json(j);
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
    case types::old_qosflow_map_ulendmarkerexpected:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<qo_s_flow_list_l>(), 1, 64, true));
      break;
    case types::drb_qos:
      HANDLE_CODE(c.get<qo_sflow_level_qos_params_s>().pack(bref));
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
    case types::old_qosflow_map_ulendmarkerexpected:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<qo_s_flow_list_l>(), bref, 1, 64, true));
      break;
    case types::drb_qos:
      HANDLE_CODE(c.get<qo_sflow_level_qos_params_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drb_to_modify_item_ng_ran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* options[] = {"QoS-Flow-List", "QoSFlowLevelQoSParameters"};
  return convert_enum_idx(options, 2, value, "drb_to_modify_item_ng_ran_ext_ies_o::ext_c::types");
}

// DRB-To-Setup-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_to_setup_item_ng_ran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {72};
  return map_enum_number(options, 1, idx, "id");
}
bool drb_to_setup_item_ng_ran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 72 == id;
}
crit_e drb_to_setup_item_ng_ran_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 72) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drb_to_setup_item_ng_ran_ext_ies_o::ext_c drb_to_setup_item_ng_ran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 72) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_to_setup_item_ng_ran_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 72) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void drb_to_setup_item_ng_ran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("QoSFlowLevelQoSParameters");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drb_to_setup_item_ng_ran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_item_ng_ran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* drb_to_setup_item_ng_ran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* options[] = {"QoSFlowLevelQoSParameters"};
  return convert_enum_idx(options, 1, value, "drb_to_setup_item_ng_ran_ext_ies_o::ext_c::types");
}

// DRB-To-Setup-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t drb_to_setup_mod_item_ng_ran_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {72};
  return map_enum_number(options, 1, idx, "id");
}
bool drb_to_setup_mod_item_ng_ran_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 72 == id;
}
crit_e drb_to_setup_mod_item_ng_ran_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 72) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c drb_to_setup_mod_item_ng_ran_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 72) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_to_setup_mod_item_ng_ran_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 72) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("QoSFlowLevelQoSParameters");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* options[] = {"QoSFlowLevelQoSParameters"};
  return convert_enum_idx(options, 1, value, "drb_to_setup_mod_item_ng_ran_ext_ies_o::ext_c::types");
}

// Data-Forwarding-Information ::= SEQUENCE
SRSASN_CODE data_forwarding_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_data_forwarding_present, 1));
  HANDLE_CODE(bref.pack(dl_data_forwarding_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (ul_data_forwarding_present) {
    HANDLE_CODE(ul_data_forwarding.pack(bref));
  }
  if (dl_data_forwarding_present) {
    HANDLE_CODE(dl_data_forwarding.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_forwarding_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_data_forwarding_present, 1));
  HANDLE_CODE(bref.unpack(dl_data_forwarding_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (ul_data_forwarding_present) {
    HANDLE_CODE(ul_data_forwarding.unpack(bref));
  }
  if (dl_data_forwarding_present) {
    HANDLE_CODE(dl_data_forwarding.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// Data-Forwarding-Information-Request ::= SEQUENCE
SRSASN_CODE data_forwarding_info_request_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qo_s_flows_forwarded_on_fwd_tunnels.size() > 0, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(data_forwarding_request.pack(bref));
  if (qo_s_flows_forwarded_on_fwd_tunnels.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, qo_s_flows_forwarded_on_fwd_tunnels, 1, 64, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_forwarding_info_request_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool qo_s_flows_forwarded_on_fwd_tunnels_present;
  HANDLE_CODE(bref.unpack(qo_s_flows_forwarded_on_fwd_tunnels_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(data_forwarding_request.unpack(bref));
  if (qo_s_flows_forwarded_on_fwd_tunnels_present) {
    HANDLE_CODE(unpack_dyn_seq_of(qo_s_flows_forwarded_on_fwd_tunnels, bref, 1, 64, true));
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
  if (qo_s_flows_forwarded_on_fwd_tunnels.size() > 0) {
    j.start_array("qoS-Flows-Forwarded-On-Fwd-Tunnels");
    for (const auto& e1 : qo_s_flows_forwarded_on_fwd_tunnels) {
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

// MaxIPrate ::= ENUMERATED
const char* max_iprate_opts::to_string() const
{
  static const char* options[] = {"bitrate64kbs", "max-UErate"};
  return convert_enum_idx(options, 2, value, "max_iprate_e");
}
uint8_t max_iprate_opts::to_number() const
{
  static const uint8_t options[] = {64};
  return map_enum_number(options, 1, value, "max_iprate_e");
}

// PDCP-Configuration ::= SEQUENCE
SRSASN_CODE pdcp_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rohc_params_present, 1));
  HANDLE_CODE(bref.pack(t_reordering_timer_present, 1));
  HANDLE_CODE(bref.pack(discard_timer_present, 1));
  HANDLE_CODE(bref.pack(uldata_split_thres_present, 1));
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
  if (uldata_split_thres_present) {
    HANDLE_CODE(uldata_split_thres.pack(bref));
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
  HANDLE_CODE(bref.unpack(uldata_split_thres_present, 1));
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
  if (uldata_split_thres_present) {
    HANDLE_CODE(uldata_split_thres.unpack(bref));
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
  if (uldata_split_thres_present) {
    j.write_str("uLDataSplitThreshold", uldata_split_thres.to_string());
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
  static const char* options[] = {"requested"};
  return convert_enum_idx(options, 1, value, "pdcp_sn_status_request_e");
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

// ConfidentialityProtectionIndication ::= ENUMERATED
const char* confidentiality_protection_ind_opts::to_string() const
{
  static const char* options[] = {"required", "preferred", "not-needed"};
  return convert_enum_idx(options, 3, value, "confidentiality_protection_ind_e");
}

template struct asn1::protocol_ext_field_s<drb_to_modify_item_ng_ran_ext_ies_o>;

drb_to_modify_item_ng_ran_ext_ies_container::drb_to_modify_item_ng_ran_ext_ies_container() :
  old_qosflow_map_ulendmarkerexpected(71, crit_e::reject), drb_qos(72, crit_e::ignore)
{
}
SRSASN_CODE drb_to_modify_item_ng_ran_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += old_qosflow_map_ulendmarkerexpected_present ? 1 : 0;
  nof_ies += drb_qos_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (old_qosflow_map_ulendmarkerexpected_present) {
    HANDLE_CODE(old_qosflow_map_ulendmarkerexpected.pack(bref));
  }
  if (drb_qos_present) {
    HANDLE_CODE(drb_qos.pack(bref));
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
    switch (id) {
      case 71: {
        old_qosflow_map_ulendmarkerexpected_present = true;
        old_qosflow_map_ulendmarkerexpected.id      = id;
        HANDLE_CODE(old_qosflow_map_ulendmarkerexpected.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(old_qosflow_map_ulendmarkerexpected.ext.unpack(bref));
        break;
      }
      case 72: {
        drb_qos_present = true;
        drb_qos.id      = id;
        HANDLE_CODE(drb_qos.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_qos.ext.unpack(bref));
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
  if (old_qosflow_map_ulendmarkerexpected_present) {
    j.write_fieldname("");
    old_qosflow_map_ulendmarkerexpected.to_json(j);
  }
  if (drb_qos_present) {
    j.write_fieldname("");
    drb_qos.to_json(j);
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

// DRB-To-Setup-Item-NG-RAN ::= SEQUENCE
SRSASN_CODE drb_to_setup_item_ng_ran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(pdcp_sn_status_info_present, 1));
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

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
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.unpack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_info_present, 1));
  bool ie_exts_present;
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
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
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
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
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
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

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
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_setup_mod_item_ng_ran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.unpack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_sn_status_info_present, 1));
  bool ie_exts_present;
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
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
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
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
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

  HANDLE_CODE(pack_integer(bref, erab_maximum_bitrate_dl, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(pack_integer(bref, erab_maximum_bitrate_ul, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
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

  HANDLE_CODE(unpack_integer(erab_maximum_bitrate_dl, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  HANDLE_CODE(unpack_integer(erab_maximum_bitrate_ul, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
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
  j.write_int("e-RAB-MaximumBitrateDL", erab_maximum_bitrate_dl);
  j.write_int("e-RAB-MaximumBitrateUL", erab_maximum_bitrate_ul);
  j.write_int("e-RAB-GuaranteedBitrateDL", erab_guaranteed_bitrate_dl);
  j.write_int("e-RAB-GuaranteedBitrateUL", erab_guaranteed_bitrate_ul);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// IntegrityProtectionIndication ::= ENUMERATED
const char* integrity_protection_ind_opts::to_string() const
{
  static const char* options[] = {"required", "preferred", "not-needed"};
  return convert_enum_idx(options, 3, value, "integrity_protection_ind_e");
}

// MaximumIPdatarate ::= SEQUENCE
SRSASN_CODE maximum_ipdatarate_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(max_iprate.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE maximum_ipdatarate_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(max_iprate.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void maximum_ipdatarate_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxIPrate", max_iprate.to_string());
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

// EUTRAN-QoS ::= SEQUENCE
SRSASN_CODE eutran_qos_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gbr_qos_info_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qci, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(eutra_nalloc_and_retention_prio.pack(bref));
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
  HANDLE_CODE(eutra_nalloc_and_retention_prio.unpack(bref));
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
  eutra_nalloc_and_retention_prio.to_json(j);
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
  static const uint32_t options[] = {69, 78};
  return map_enum_number(options, 2, idx, "id");
}
bool pdu_session_res_to_modify_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {69, 78};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_to_modify_item_ext_ies_o::ext_c::destroy_()
{
  switch (type_) {
    case types::snssai:
      c.destroy<snssai_s>();
      break;
    case types::common_network_instance:
      c.destroy<unbounded_octstring<true>>();
      break;
    default:
      break;
  }
}
void pdu_session_res_to_modify_item_ext_ies_o::ext_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::snssai:
      c.init<snssai_s>();
      break;
    case types::common_network_instance:
      c.init<unbounded_octstring<true>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_modify_item_ext_ies_o::ext_c");
  }
}
pdu_session_res_to_modify_item_ext_ies_o::ext_c::ext_c(const pdu_session_res_to_modify_item_ext_ies_o::ext_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::snssai:
      c.init(other.c.get<snssai_s>());
      break;
    case types::common_network_instance:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_modify_item_ext_ies_o::ext_c");
  }
}
pdu_session_res_to_modify_item_ext_ies_o::ext_c&
pdu_session_res_to_modify_item_ext_ies_o::ext_c::operator=(const pdu_session_res_to_modify_item_ext_ies_o::ext_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::snssai:
      c.set(other.c.get<snssai_s>());
      break;
    case types::common_network_instance:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_modify_item_ext_ies_o::ext_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_modify_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_to_modify_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* options[] = {"SNSSAI", "OCTET STRING"};
  return convert_enum_idx(options, 2, value, "pdu_session_res_to_modify_item_ext_ies_o::ext_c::types");
}

// PDU-Session-Resource-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_to_rem_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {0};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"Cause"};
  return convert_enum_idx(options, 1, value, "pdu_session_res_to_rem_item_ext_ies_o::ext_c::types");
}

// PDU-Session-Resource-To-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_to_setup_mod_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {79, 78};
  return map_enum_number(options, 2, idx, "id");
}
bool pdu_session_res_to_setup_mod_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {79, 78};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::destroy_()
{
  switch (type_) {
    case types::common_network_instance:
      c.destroy<unbounded_octstring<true>>();
      break;
    default:
      break;
  }
}
void pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::network_instance:
      break;
    case types::common_network_instance:
      c.init<unbounded_octstring<true>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c");
  }
}
pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::ext_c(
    const pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::network_instance:
      c.init(other.c.get<uint16_t>());
      break;
    case types::common_network_instance:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c");
  }
}
pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c& pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::operator=(
    const pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::network_instance:
      c.set(other.c.get<uint16_t>());
      break;
    case types::common_network_instance:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (1..256,...)", "OCTET STRING"};
  return convert_enum_idx(options, 2, value, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::types");
}
uint8_t pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "pdu_session_res_to_setup_mod_item_ext_ies_o::ext_c::types");
}

// PDU-Session-Type ::= ENUMERATED
const char* pdu_session_type_opts::to_string() const
{
  static const char* options[] = {"ipv4", "ipv6", "ipv4v6", "ethernet", "unstructured"};
  return convert_enum_idx(options, 5, value, "pdu_session_type_e");
}

// SecurityIndication ::= SEQUENCE
SRSASN_CODE security_ind_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(maximum_ipdatarate_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(integrity_protection_ind.pack(bref));
  HANDLE_CODE(confidentiality_protection_ind.pack(bref));
  if (maximum_ipdatarate_present) {
    HANDLE_CODE(maximum_ipdatarate.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE security_ind_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(maximum_ipdatarate_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(integrity_protection_ind.unpack(bref));
  HANDLE_CODE(confidentiality_protection_ind.unpack(bref));
  if (maximum_ipdatarate_present) {
    HANDLE_CODE(maximum_ipdatarate.unpack(bref));
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
  if (maximum_ipdatarate_present) {
    j.write_fieldname("maximumIPdatarate");
    maximum_ipdatarate.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
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
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

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
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
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
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ext_field_s<pdu_session_res_to_modify_item_ext_ies_o>;

pdu_session_res_to_modify_item_ext_ies_container::pdu_session_res_to_modify_item_ext_ies_container() :
  snssai(69, crit_e::reject), common_network_instance(78, crit_e::ignore)
{
}
SRSASN_CODE pdu_session_res_to_modify_item_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += snssai_present ? 1 : 0;
  nof_ies += common_network_instance_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (snssai_present) {
    HANDLE_CODE(snssai.pack(bref));
  }
  if (common_network_instance_present) {
    HANDLE_CODE(common_network_instance.pack(bref));
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
    switch (id) {
      case 69: {
        snssai_present = true;
        snssai.id      = id;
        HANDLE_CODE(snssai.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(snssai.ext.unpack(bref));
        break;
      }
      case 78: {
        common_network_instance_present = true;
        common_network_instance.id      = id;
        HANDLE_CODE(common_network_instance.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(common_network_instance.ext.unpack(bref));
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
    j.write_fieldname("");
    snssai.to_json(j);
  }
  if (common_network_instance_present) {
    j.write_fieldname("");
    common_network_instance.to_json(j);
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

pdu_session_res_to_setup_mod_item_ext_ies_container::pdu_session_res_to_setup_mod_item_ext_ies_container() :
  network_instance(79, crit_e::ignore), common_network_instance(78, crit_e::ignore)
{
}
SRSASN_CODE pdu_session_res_to_setup_mod_item_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += network_instance_present ? 1 : 0;
  nof_ies += common_network_instance_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (network_instance_present) {
    HANDLE_CODE(network_instance.pack(bref));
  }
  if (common_network_instance_present) {
    HANDLE_CODE(common_network_instance.pack(bref));
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
    switch (id) {
      case 79: {
        network_instance_present = true;
        network_instance.id      = id;
        HANDLE_CODE(network_instance.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(network_instance.ext.unpack(bref));
        break;
      }
      case 78: {
        common_network_instance_present = true;
        common_network_instance.id      = id;
        HANDLE_CODE(common_network_instance.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(common_network_instance.ext.unpack(bref));
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
    j.write_fieldname("");
    network_instance.to_json(j);
  }
  if (common_network_instance_present) {
    j.write_fieldname("");
    common_network_instance.to_json(j);
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
  static const char* options[] = {"nEA0", "c-128-NEA1", "c-128-NEA2", "c-128-NEA3"};
  return convert_enum_idx(options, 4, value, "ciphering_algorithm_e");
}

// IntegrityProtectionAlgorithm ::= ENUMERATED
const char* integrity_protection_algorithm_opts::to_string() const
{
  static const char* options[] = {"nIA0", "i-128-NIA1", "i-128-NIA2", "i-128-NIA3"};
  return convert_enum_idx(options, 4, value, "integrity_protection_algorithm_e");
}

// EUTRAN-BearerContextModificationRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_mod_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {51, 33, 34};
  return map_enum_number(options, 3, idx, "id");
}
bool eutran_bearer_context_mod_request_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {51, 33, 34};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_mod_request_o::value_c::destroy_()
{
  switch (type_) {
    case types::drb_to_setup_mod_list_eutran:
      c.destroy<drb_to_setup_mod_list_eutran_l>();
      break;
    case types::drb_to_modify_list_eutran:
      c.destroy<drb_to_modify_list_eutran_l>();
      break;
    case types::drb_to_rem_list_eutran:
      c.destroy<drb_to_rem_list_eutran_l>();
      break;
    default:
      break;
  }
}
void eutran_bearer_context_mod_request_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::drb_to_setup_mod_list_eutran:
      c.init<drb_to_setup_mod_list_eutran_l>();
      break;
    case types::drb_to_modify_list_eutran:
      c.init<drb_to_modify_list_eutran_l>();
      break;
    case types::drb_to_rem_list_eutran:
      c.init<drb_to_rem_list_eutran_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_request_o::value_c");
  }
}
eutran_bearer_context_mod_request_o::value_c::value_c(const eutran_bearer_context_mod_request_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::drb_to_setup_mod_list_eutran:
      c.init(other.c.get<drb_to_setup_mod_list_eutran_l>());
      break;
    case types::drb_to_modify_list_eutran:
      c.init(other.c.get<drb_to_modify_list_eutran_l>());
      break;
    case types::drb_to_rem_list_eutran:
      c.init(other.c.get<drb_to_rem_list_eutran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_request_o::value_c");
  }
}
eutran_bearer_context_mod_request_o::value_c&
eutran_bearer_context_mod_request_o::value_c::operator=(const eutran_bearer_context_mod_request_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::drb_to_setup_mod_list_eutran:
      c.set(other.c.get<drb_to_setup_mod_list_eutran_l>());
      break;
    case types::drb_to_modify_list_eutran:
      c.set(other.c.get<drb_to_modify_list_eutran_l>());
      break;
    case types::drb_to_rem_list_eutran:
      c.set(other.c.get<drb_to_rem_list_eutran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_request_o::value_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_request_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_mod_request_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "DRB-To-Setup-Mod-List-EUTRAN", "DRB-To-Modify-List-EUTRAN", "DRB-To-Remove-List-EUTRAN"};
  return convert_enum_idx(options, 3, value, "eutran_bearer_context_mod_request_o::value_c::types");
}

// NG-RAN-BearerContextModificationRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_mod_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {56, 43, 44};
  return map_enum_number(options, 3, idx, "id");
}
bool ng_ran_bearer_context_mod_request_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {56, 43, 44};
  for (const auto& o : options) {
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
void ng_ran_bearer_context_mod_request_o::value_c::destroy_()
{
  switch (type_) {
    case types::pdu_session_res_to_setup_mod_list:
      c.destroy<pdu_session_res_to_setup_mod_list_l>();
      break;
    case types::pdu_session_res_to_modify_list:
      c.destroy<pdu_session_res_to_modify_list_l>();
      break;
    case types::pdu_session_res_to_rem_list:
      c.destroy<pdu_session_res_to_rem_list_l>();
      break;
    default:
      break;
  }
}
void ng_ran_bearer_context_mod_request_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pdu_session_res_to_setup_mod_list:
      c.init<pdu_session_res_to_setup_mod_list_l>();
      break;
    case types::pdu_session_res_to_modify_list:
      c.init<pdu_session_res_to_modify_list_l>();
      break;
    case types::pdu_session_res_to_rem_list:
      c.init<pdu_session_res_to_rem_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_request_o::value_c");
  }
}
ng_ran_bearer_context_mod_request_o::value_c::value_c(const ng_ran_bearer_context_mod_request_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pdu_session_res_to_setup_mod_list:
      c.init(other.c.get<pdu_session_res_to_setup_mod_list_l>());
      break;
    case types::pdu_session_res_to_modify_list:
      c.init(other.c.get<pdu_session_res_to_modify_list_l>());
      break;
    case types::pdu_session_res_to_rem_list:
      c.init(other.c.get<pdu_session_res_to_rem_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_request_o::value_c");
  }
}
ng_ran_bearer_context_mod_request_o::value_c&
ng_ran_bearer_context_mod_request_o::value_c::operator=(const ng_ran_bearer_context_mod_request_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pdu_session_res_to_setup_mod_list:
      c.set(other.c.get<pdu_session_res_to_setup_mod_list_l>());
      break;
    case types::pdu_session_res_to_modify_list:
      c.set(other.c.get<pdu_session_res_to_modify_list_l>());
      break;
    case types::pdu_session_res_to_rem_list:
      c.set(other.c.get<pdu_session_res_to_rem_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_request_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"PDU-Session-Resource-To-Setup-Mod-List",
                                  "PDU-Session-Resource-To-Modify-List",
                                  "PDU-Session-Resource-To-Remove-List"};
  return convert_enum_idx(options, 3, value, "ng_ran_bearer_context_mod_request_o::value_c::types");
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
  static const char* options[] = {"drb", "pdu-session", "ue"};
  return convert_enum_idx(options, 3, value, "activity_notif_level_e");
}

// BearerContextStatusChange ::= ENUMERATED
const char* bearer_context_status_change_opts::to_string() const
{
  static const char* options[] = {"suspend", "resume"};
  return convert_enum_idx(options, 2, value, "bearer_context_status_change_e");
}

// DataDiscardRequired ::= ENUMERATED
const char* data_discard_required_opts::to_string() const
{
  static const char* options[] = {"required"};
  return convert_enum_idx(options, 1, value, "data_discard_required_e");
}

// New-UL-TNL-Information-Required ::= ENUMERATED
const char* new_ul_tnl_info_required_opts::to_string() const
{
  static const char* options[] = {"required"};
  return convert_enum_idx(options, 1, value, "new_ul_tnl_info_required_e");
}

// SecurityInformation ::= SEQUENCE
SRSASN_CODE security_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(security_algorithm.pack(bref));
  HANDLE_CODE(upsecuritykey.pack(bref));
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
  HANDLE_CODE(upsecuritykey.unpack(bref));
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
  upsecuritykey.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_mod_request_o>;

eutran_bearer_context_mod_request_container::eutran_bearer_context_mod_request_container() :
  drb_to_setup_mod_list_eutran(51, crit_e::reject),
  drb_to_modify_list_eutran(33, crit_e::reject),
  drb_to_rem_list_eutran(34, crit_e::reject)
{
}
SRSASN_CODE eutran_bearer_context_mod_request_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += drb_to_setup_mod_list_eutran_present ? 1 : 0;
  nof_ies += drb_to_modify_list_eutran_present ? 1 : 0;
  nof_ies += drb_to_rem_list_eutran_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (drb_to_setup_mod_list_eutran_present) {
    HANDLE_CODE(drb_to_setup_mod_list_eutran.pack(bref));
  }
  if (drb_to_modify_list_eutran_present) {
    HANDLE_CODE(drb_to_modify_list_eutran.pack(bref));
  }
  if (drb_to_rem_list_eutran_present) {
    HANDLE_CODE(drb_to_rem_list_eutran.pack(bref));
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
    switch (id) {
      case 51: {
        drb_to_setup_mod_list_eutran_present = true;
        drb_to_setup_mod_list_eutran.id      = id;
        HANDLE_CODE(drb_to_setup_mod_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_to_setup_mod_list_eutran.value.unpack(bref));
        break;
      }
      case 33: {
        drb_to_modify_list_eutran_present = true;
        drb_to_modify_list_eutran.id      = id;
        HANDLE_CODE(drb_to_modify_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_to_modify_list_eutran.value.unpack(bref));
        break;
      }
      case 34: {
        drb_to_rem_list_eutran_present = true;
        drb_to_rem_list_eutran.id      = id;
        HANDLE_CODE(drb_to_rem_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_to_rem_list_eutran.value.unpack(bref));
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
    j.write_fieldname("");
    drb_to_setup_mod_list_eutran.to_json(j);
  }
  if (drb_to_modify_list_eutran_present) {
    j.write_fieldname("");
    drb_to_modify_list_eutran.to_json(j);
  }
  if (drb_to_rem_list_eutran_present) {
    j.write_fieldname("");
    drb_to_rem_list_eutran.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ng_ran_bearer_context_mod_request_o>;

ng_ran_bearer_context_mod_request_container::ng_ran_bearer_context_mod_request_container() :
  pdu_session_res_to_setup_mod_list(56, crit_e::reject),
  pdu_session_res_to_modify_list(43, crit_e::reject),
  pdu_session_res_to_rem_list(44, crit_e::reject)
{
}
SRSASN_CODE ng_ran_bearer_context_mod_request_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += pdu_session_res_to_setup_mod_list_present ? 1 : 0;
  nof_ies += pdu_session_res_to_modify_list_present ? 1 : 0;
  nof_ies += pdu_session_res_to_rem_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (pdu_session_res_to_setup_mod_list_present) {
    HANDLE_CODE(pdu_session_res_to_setup_mod_list.pack(bref));
  }
  if (pdu_session_res_to_modify_list_present) {
    HANDLE_CODE(pdu_session_res_to_modify_list.pack(bref));
  }
  if (pdu_session_res_to_rem_list_present) {
    HANDLE_CODE(pdu_session_res_to_rem_list.pack(bref));
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
    switch (id) {
      case 56: {
        pdu_session_res_to_setup_mod_list_present = true;
        pdu_session_res_to_setup_mod_list.id      = id;
        HANDLE_CODE(pdu_session_res_to_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_to_setup_mod_list.value.unpack(bref));
        break;
      }
      case 43: {
        pdu_session_res_to_modify_list_present = true;
        pdu_session_res_to_modify_list.id      = id;
        HANDLE_CODE(pdu_session_res_to_modify_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_to_modify_list.value.unpack(bref));
        break;
      }
      case 44: {
        pdu_session_res_to_rem_list_present = true;
        pdu_session_res_to_rem_list.id      = id;
        HANDLE_CODE(pdu_session_res_to_rem_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_to_rem_list.value.unpack(bref));
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
    j.write_fieldname("");
    pdu_session_res_to_setup_mod_list.to_json(j);
  }
  if (pdu_session_res_to_modify_list_present) {
    j.write_fieldname("");
    pdu_session_res_to_modify_list.to_json(j);
  }
  if (pdu_session_res_to_rem_list_present) {
    j.write_fieldname("");
    pdu_session_res_to_rem_list.to_json(j);
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
  static const char* options[] = {
      "e-UTRAN-BearerContextModificationRequest", "nG-RAN-BearerContextModificationRequest", "choice-extension"};
  return convert_enum_idx(options, 3, value, "sys_bearer_context_mod_request_c::types");
}

// BearerContextModificationRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 13, 14, 66, 17, 26, 59, 70, 18, 76, 77, 23};
  return map_enum_number(options, 13, idx, "id");
}
bool bearer_context_mod_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 13, 14, 66, 17, 26, 59, 70, 18, 76, 77, 23};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
      break;
    case 13:
      ret.set(value_c::types::security_info);
      break;
    case 14:
      ret.set(value_c::types::uedl_aggregate_maximum_bit_rate);
      break;
    case 66:
      ret.set(value_c::types::uedl_maximum_integrity_protected_data_rate);
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
      ret.set(value_c::types::ranueid);
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
void bearer_context_mod_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::security_info:
      c.destroy<security_info_s>();
      break;
    case types::sys_bearer_context_mod_request:
      c.destroy<sys_bearer_context_mod_request_c>();
      break;
    case types::ranueid:
      c.destroy<fixed_octstring<8, true>>();
      break;
    default:
      break;
  }
}
void bearer_context_mod_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::security_info:
      c.init<security_info_s>();
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
      break;
    case types::bearer_context_status_change:
      break;
    case types::new_ul_tnl_info_required:
      break;
    case types::ue_inactivity_timer:
      break;
    case types::data_discard_required:
      break;
    case types::sys_bearer_context_mod_request:
      c.init<sys_bearer_context_mod_request_c>();
      break;
    case types::ranueid:
      c.init<fixed_octstring<8, true>>();
      break;
    case types::gnb_du_id:
      break;
    case types::activity_notif_level:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_request_ies_o::value_c");
  }
}
bearer_context_mod_request_ies_o::value_c::value_c(const bearer_context_mod_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::security_info:
      c.init(other.c.get<security_info_s>());
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      c.init(other.c.get<uint64_t>());
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
      c.init(other.c.get<uint64_t>());
      break;
    case types::bearer_context_status_change:
      c.init(other.c.get<bearer_context_status_change_e>());
      break;
    case types::new_ul_tnl_info_required:
      c.init(other.c.get<new_ul_tnl_info_required_e>());
      break;
    case types::ue_inactivity_timer:
      c.init(other.c.get<uint16_t>());
      break;
    case types::data_discard_required:
      c.init(other.c.get<data_discard_required_e>());
      break;
    case types::sys_bearer_context_mod_request:
      c.init(other.c.get<sys_bearer_context_mod_request_c>());
      break;
    case types::ranueid:
      c.init(other.c.get<fixed_octstring<8, true>>());
      break;
    case types::gnb_du_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::activity_notif_level:
      c.init(other.c.get<activity_notif_level_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_request_ies_o::value_c");
  }
}
bearer_context_mod_request_ies_o::value_c&
bearer_context_mod_request_ies_o::value_c::operator=(const bearer_context_mod_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::security_info:
      c.set(other.c.get<security_info_s>());
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      c.set(other.c.get<uint64_t>());
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
      c.set(other.c.get<uint64_t>());
      break;
    case types::bearer_context_status_change:
      c.set(other.c.get<bearer_context_status_change_e>());
      break;
    case types::new_ul_tnl_info_required:
      c.set(other.c.get<new_ul_tnl_info_required_e>());
      break;
    case types::ue_inactivity_timer:
      c.set(other.c.get<uint16_t>());
      break;
    case types::data_discard_required:
      c.set(other.c.get<data_discard_required_e>());
      break;
    case types::sys_bearer_context_mod_request:
      c.set(other.c.get<sys_bearer_context_mod_request_c>());
      break;
    case types::ranueid:
      c.set(other.c.get<fixed_octstring<8, true>>());
      break;
    case types::gnb_du_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::activity_notif_level:
      c.set(other.c.get<activity_notif_level_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_request_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
security_info_s& bearer_context_mod_request_ies_o::value_c::security_info()
{
  assert_choice_type(types::security_info, type_, "Value");
  return c.get<security_info_s>();
}
uint64_t& bearer_context_mod_request_ies_o::value_c::uedl_aggregate_maximum_bit_rate()
{
  assert_choice_type(types::uedl_aggregate_maximum_bit_rate, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_request_ies_o::value_c::uedl_maximum_integrity_protected_data_rate()
{
  assert_choice_type(types::uedl_maximum_integrity_protected_data_rate, type_, "Value");
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
fixed_octstring<8, true>& bearer_context_mod_request_ies_o::value_c::ranueid()
{
  assert_choice_type(types::ranueid, type_, "Value");
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
const uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const security_info_s& bearer_context_mod_request_ies_o::value_c::security_info() const
{
  assert_choice_type(types::security_info, type_, "Value");
  return c.get<security_info_s>();
}
const uint64_t& bearer_context_mod_request_ies_o::value_c::uedl_aggregate_maximum_bit_rate() const
{
  assert_choice_type(types::uedl_aggregate_maximum_bit_rate, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_request_ies_o::value_c::uedl_maximum_integrity_protected_data_rate() const
{
  assert_choice_type(types::uedl_maximum_integrity_protected_data_rate, type_, "Value");
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
const fixed_octstring<8, true>& bearer_context_mod_request_ies_o::value_c::ranueid() const
{
  assert_choice_type(types::ranueid, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::security_info:
      j.write_fieldname("SecurityInformation");
      c.get<security_info_s>().to_json(j);
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
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
    case types::ranueid:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::security_info:
      HANDLE_CODE(c.get<security_info_s>().pack(bref));
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
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
    case types::ranueid:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::security_info:
      HANDLE_CODE(c.get<security_info_s>().unpack(bref));
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
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
    case types::ranueid:
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
  static const char* options[] = {"INTEGER (0..4294967295)",
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
  return convert_enum_idx(options, 13, value, "bearer_context_mod_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_request_ies_o>;

bearer_context_mod_request_ies_container::bearer_context_mod_request_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  security_info(13, crit_e::reject),
  uedl_aggregate_maximum_bit_rate(14, crit_e::reject),
  uedl_maximum_integrity_protected_data_rate(66, crit_e::reject),
  bearer_context_status_change(17, crit_e::reject),
  new_ul_tnl_info_required(26, crit_e::reject),
  ue_inactivity_timer(59, crit_e::reject),
  data_discard_required(70, crit_e::ignore),
  sys_bearer_context_mod_request(18, crit_e::reject),
  ranueid(76, crit_e::ignore),
  gnb_du_id(77, crit_e::ignore),
  activity_notif_level(23, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_mod_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += security_info_present ? 1 : 0;
  nof_ies += uedl_aggregate_maximum_bit_rate_present ? 1 : 0;
  nof_ies += uedl_maximum_integrity_protected_data_rate_present ? 1 : 0;
  nof_ies += bearer_context_status_change_present ? 1 : 0;
  nof_ies += new_ul_tnl_info_required_present ? 1 : 0;
  nof_ies += ue_inactivity_timer_present ? 1 : 0;
  nof_ies += data_discard_required_present ? 1 : 0;
  nof_ies += sys_bearer_context_mod_request_present ? 1 : 0;
  nof_ies += ranueid_present ? 1 : 0;
  nof_ies += gnb_du_id_present ? 1 : 0;
  nof_ies += activity_notif_level_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  if (security_info_present) {
    HANDLE_CODE(security_info.pack(bref));
  }
  if (uedl_aggregate_maximum_bit_rate_present) {
    HANDLE_CODE(uedl_aggregate_maximum_bit_rate.pack(bref));
  }
  if (uedl_maximum_integrity_protected_data_rate_present) {
    HANDLE_CODE(uedl_maximum_integrity_protected_data_rate.pack(bref));
  }
  if (bearer_context_status_change_present) {
    HANDLE_CODE(bearer_context_status_change.pack(bref));
  }
  if (new_ul_tnl_info_required_present) {
    HANDLE_CODE(new_ul_tnl_info_required.pack(bref));
  }
  if (ue_inactivity_timer_present) {
    HANDLE_CODE(ue_inactivity_timer.pack(bref));
  }
  if (data_discard_required_present) {
    HANDLE_CODE(data_discard_required.pack(bref));
  }
  if (sys_bearer_context_mod_request_present) {
    HANDLE_CODE(sys_bearer_context_mod_request.pack(bref));
  }
  if (ranueid_present) {
    HANDLE_CODE(ranueid.pack(bref));
  }
  if (gnb_du_id_present) {
    HANDLE_CODE(gnb_du_id.pack(bref));
  }
  if (activity_notif_level_present) {
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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 13: {
        security_info_present = true;
        security_info.id      = id;
        HANDLE_CODE(security_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(security_info.value.unpack(bref));
        break;
      }
      case 14: {
        uedl_aggregate_maximum_bit_rate_present = true;
        uedl_aggregate_maximum_bit_rate.id      = id;
        HANDLE_CODE(uedl_aggregate_maximum_bit_rate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uedl_aggregate_maximum_bit_rate.value.unpack(bref));
        break;
      }
      case 66: {
        uedl_maximum_integrity_protected_data_rate_present = true;
        uedl_maximum_integrity_protected_data_rate.id      = id;
        HANDLE_CODE(uedl_maximum_integrity_protected_data_rate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uedl_maximum_integrity_protected_data_rate.value.unpack(bref));
        break;
      }
      case 17: {
        bearer_context_status_change_present = true;
        bearer_context_status_change.id      = id;
        HANDLE_CODE(bearer_context_status_change.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bearer_context_status_change.value.unpack(bref));
        break;
      }
      case 26: {
        new_ul_tnl_info_required_present = true;
        new_ul_tnl_info_required.id      = id;
        HANDLE_CODE(new_ul_tnl_info_required.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(new_ul_tnl_info_required.value.unpack(bref));
        break;
      }
      case 59: {
        ue_inactivity_timer_present = true;
        ue_inactivity_timer.id      = id;
        HANDLE_CODE(ue_inactivity_timer.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_inactivity_timer.value.unpack(bref));
        break;
      }
      case 70: {
        data_discard_required_present = true;
        data_discard_required.id      = id;
        HANDLE_CODE(data_discard_required.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(data_discard_required.value.unpack(bref));
        break;
      }
      case 18: {
        sys_bearer_context_mod_request_present = true;
        sys_bearer_context_mod_request.id      = id;
        HANDLE_CODE(sys_bearer_context_mod_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_mod_request.value.unpack(bref));
        break;
      }
      case 76: {
        ranueid_present = true;
        ranueid.id      = id;
        HANDLE_CODE(ranueid.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ranueid.value.unpack(bref));
        break;
      }
      case 77: {
        gnb_du_id_present = true;
        gnb_du_id.id      = id;
        HANDLE_CODE(gnb_du_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_id.value.unpack(bref));
        break;
      }
      case 23: {
        activity_notif_level_present = true;
        activity_notif_level.id      = id;
        HANDLE_CODE(activity_notif_level.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(activity_notif_level.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  if (security_info_present) {
    j.write_fieldname("");
    security_info.to_json(j);
  }
  if (uedl_aggregate_maximum_bit_rate_present) {
    j.write_fieldname("");
    uedl_aggregate_maximum_bit_rate.to_json(j);
  }
  if (uedl_maximum_integrity_protected_data_rate_present) {
    j.write_fieldname("");
    uedl_maximum_integrity_protected_data_rate.to_json(j);
  }
  if (bearer_context_status_change_present) {
    j.write_fieldname("");
    bearer_context_status_change.to_json(j);
  }
  if (new_ul_tnl_info_required_present) {
    j.write_fieldname("");
    new_ul_tnl_info_required.to_json(j);
  }
  if (ue_inactivity_timer_present) {
    j.write_fieldname("");
    ue_inactivity_timer.to_json(j);
  }
  if (data_discard_required_present) {
    j.write_fieldname("");
    data_discard_required.to_json(j);
  }
  if (sys_bearer_context_mod_request_present) {
    j.write_fieldname("");
    sys_bearer_context_mod_request.to_json(j);
  }
  if (ranueid_present) {
    j.write_fieldname("");
    ranueid.to_json(j);
  }
  if (gnb_du_id_present) {
    j.write_fieldname("");
    gnb_du_id.to_json(j);
  }
  if (activity_notif_level_present) {
    j.write_fieldname("");
    activity_notif_level.to_json(j);
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
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

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
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
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
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// EUTRAN-BearerContextModificationRequired ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_mod_required_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {35, 36};
  return map_enum_number(options, 2, idx, "id");
}
bool eutran_bearer_context_mod_required_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {35, 36};
  for (const auto& o : options) {
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
void eutran_bearer_context_mod_required_o::value_c::destroy_()
{
  switch (type_) {
    case types::drb_required_to_modify_list_eutran:
      c.destroy<drb_required_to_modify_list_eutran_l>();
      break;
    case types::drb_required_to_rem_list_eutran:
      c.destroy<drb_required_to_rem_list_eutran_l>();
      break;
    default:
      break;
  }
}
void eutran_bearer_context_mod_required_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::drb_required_to_modify_list_eutran:
      c.init<drb_required_to_modify_list_eutran_l>();
      break;
    case types::drb_required_to_rem_list_eutran:
      c.init<drb_required_to_rem_list_eutran_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_required_o::value_c");
  }
}
eutran_bearer_context_mod_required_o::value_c::value_c(const eutran_bearer_context_mod_required_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::drb_required_to_modify_list_eutran:
      c.init(other.c.get<drb_required_to_modify_list_eutran_l>());
      break;
    case types::drb_required_to_rem_list_eutran:
      c.init(other.c.get<drb_required_to_rem_list_eutran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_required_o::value_c");
  }
}
eutran_bearer_context_mod_required_o::value_c&
eutran_bearer_context_mod_required_o::value_c::operator=(const eutran_bearer_context_mod_required_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::drb_required_to_modify_list_eutran:
      c.set(other.c.get<drb_required_to_modify_list_eutran_l>());
      break;
    case types::drb_required_to_rem_list_eutran:
      c.set(other.c.get<drb_required_to_rem_list_eutran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_required_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"DRB-Required-To-Modify-List-EUTRAN", "DRB-Required-To-Remove-List-EUTRAN"};
  return convert_enum_idx(options, 2, value, "eutran_bearer_context_mod_required_o::value_c::types");
}

// NG-RAN-BearerContextModificationRequired ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_mod_required_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {45, 44};
  return map_enum_number(options, 2, idx, "id");
}
bool ng_ran_bearer_context_mod_required_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {45, 44};
  for (const auto& o : options) {
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
void ng_ran_bearer_context_mod_required_o::value_c::destroy_()
{
  switch (type_) {
    case types::pdu_session_res_required_to_modify_list:
      c.destroy<pdu_session_res_required_to_modify_list_l>();
      break;
    case types::pdu_session_res_to_rem_list:
      c.destroy<pdu_session_res_to_rem_list_l>();
      break;
    default:
      break;
  }
}
void ng_ran_bearer_context_mod_required_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pdu_session_res_required_to_modify_list:
      c.init<pdu_session_res_required_to_modify_list_l>();
      break;
    case types::pdu_session_res_to_rem_list:
      c.init<pdu_session_res_to_rem_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_required_o::value_c");
  }
}
ng_ran_bearer_context_mod_required_o::value_c::value_c(const ng_ran_bearer_context_mod_required_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pdu_session_res_required_to_modify_list:
      c.init(other.c.get<pdu_session_res_required_to_modify_list_l>());
      break;
    case types::pdu_session_res_to_rem_list:
      c.init(other.c.get<pdu_session_res_to_rem_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_required_o::value_c");
  }
}
ng_ran_bearer_context_mod_required_o::value_c&
ng_ran_bearer_context_mod_required_o::value_c::operator=(const ng_ran_bearer_context_mod_required_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pdu_session_res_required_to_modify_list:
      c.set(other.c.get<pdu_session_res_required_to_modify_list_l>());
      break;
    case types::pdu_session_res_to_rem_list:
      c.set(other.c.get<pdu_session_res_to_rem_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_required_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"PDU-Session-Resource-Required-To-Modify-List",
                                  "PDU-Session-Resource-To-Remove-List"};
  return convert_enum_idx(options, 2, value, "ng_ran_bearer_context_mod_required_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_mod_required_o>;

eutran_bearer_context_mod_required_container::eutran_bearer_context_mod_required_container() :
  drb_required_to_modify_list_eutran(35, crit_e::reject), drb_required_to_rem_list_eutran(36, crit_e::reject)
{
}
SRSASN_CODE eutran_bearer_context_mod_required_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += drb_required_to_modify_list_eutran_present ? 1 : 0;
  nof_ies += drb_required_to_rem_list_eutran_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (drb_required_to_modify_list_eutran_present) {
    HANDLE_CODE(drb_required_to_modify_list_eutran.pack(bref));
  }
  if (drb_required_to_rem_list_eutran_present) {
    HANDLE_CODE(drb_required_to_rem_list_eutran.pack(bref));
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
    switch (id) {
      case 35: {
        drb_required_to_modify_list_eutran_present = true;
        drb_required_to_modify_list_eutran.id      = id;
        HANDLE_CODE(drb_required_to_modify_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_required_to_modify_list_eutran.value.unpack(bref));
        break;
      }
      case 36: {
        drb_required_to_rem_list_eutran_present = true;
        drb_required_to_rem_list_eutran.id      = id;
        HANDLE_CODE(drb_required_to_rem_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_required_to_rem_list_eutran.value.unpack(bref));
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
    j.write_fieldname("");
    drb_required_to_modify_list_eutran.to_json(j);
  }
  if (drb_required_to_rem_list_eutran_present) {
    j.write_fieldname("");
    drb_required_to_rem_list_eutran.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ng_ran_bearer_context_mod_required_o>;

ng_ran_bearer_context_mod_required_container::ng_ran_bearer_context_mod_required_container() :
  pdu_session_res_required_to_modify_list(45, crit_e::reject), pdu_session_res_to_rem_list(44, crit_e::reject)
{
}
SRSASN_CODE ng_ran_bearer_context_mod_required_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += pdu_session_res_required_to_modify_list_present ? 1 : 0;
  nof_ies += pdu_session_res_to_rem_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (pdu_session_res_required_to_modify_list_present) {
    HANDLE_CODE(pdu_session_res_required_to_modify_list.pack(bref));
  }
  if (pdu_session_res_to_rem_list_present) {
    HANDLE_CODE(pdu_session_res_to_rem_list.pack(bref));
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
    switch (id) {
      case 45: {
        pdu_session_res_required_to_modify_list_present = true;
        pdu_session_res_required_to_modify_list.id      = id;
        HANDLE_CODE(pdu_session_res_required_to_modify_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_required_to_modify_list.value.unpack(bref));
        break;
      }
      case 44: {
        pdu_session_res_to_rem_list_present = true;
        pdu_session_res_to_rem_list.id      = id;
        HANDLE_CODE(pdu_session_res_to_rem_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_to_rem_list.value.unpack(bref));
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
    j.write_fieldname("");
    pdu_session_res_required_to_modify_list.to_json(j);
  }
  if (pdu_session_res_to_rem_list_present) {
    j.write_fieldname("");
    pdu_session_res_to_rem_list.to_json(j);
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
  static const char* options[] = {
      "e-UTRAN-BearerContextModificationRequired", "nG-RAN-BearerContextModificationRequired", "choice-extension"};
  return convert_enum_idx(options, 3, value, "sys_bearer_context_mod_required_c::types");
}

// BearerContextModificationRequiredIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_required_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 21};
  return map_enum_number(options, 3, idx, "id");
}
bool bearer_context_mod_required_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 21};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_mod_required_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::sys_bearer_context_mod_required:
      c.destroy<sys_bearer_context_mod_required_c>();
      break;
    default:
      break;
  }
}
void bearer_context_mod_required_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::sys_bearer_context_mod_required:
      c.init<sys_bearer_context_mod_required_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_required_ies_o::value_c");
  }
}
bearer_context_mod_required_ies_o::value_c::value_c(const bearer_context_mod_required_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_required:
      c.init(other.c.get<sys_bearer_context_mod_required_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_required_ies_o::value_c");
  }
}
bearer_context_mod_required_ies_o::value_c&
bearer_context_mod_required_ies_o::value_c::operator=(const bearer_context_mod_required_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_required:
      c.set(other.c.get<sys_bearer_context_mod_required_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_required_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_mod_required_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_required_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_bearer_context_mod_required_c& bearer_context_mod_required_ies_o::value_c::sys_bearer_context_mod_required()
{
  assert_choice_type(types::sys_bearer_context_mod_required, type_, "Value");
  return c.get<sys_bearer_context_mod_required_c>();
}
const uint64_t& bearer_context_mod_required_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_required_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-BearerContextModificationRequired"};
  return convert_enum_idx(options, 3, value, "bearer_context_mod_required_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_required_ies_o>;

bearer_context_mod_required_ies_container::bearer_context_mod_required_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  sys_bearer_context_mod_required(21, crit_e::reject)
{
}
SRSASN_CODE bearer_context_mod_required_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(sys_bearer_context_mod_required.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 21: {
        nof_mandatory_ies--;
        sys_bearer_context_mod_required.id = id;
        HANDLE_CODE(sys_bearer_context_mod_required.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_mod_required.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  sys_bearer_context_mod_required.to_json(j);
  j.end_obj();
}

// QoS-Flow-Failed-Item ::= SEQUENCE
SRSASN_CODE qo_s_flow_failed_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qo_s_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE qo_s_flow_failed_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qo_s_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void qo_s_flow_failed_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qo_s_flow_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// ConfidentialityProtectionResult ::= ENUMERATED
const char* confidentiality_protection_result_opts::to_string() const
{
  static const char* options[] = {"performed", "not-performed"};
  return convert_enum_idx(options, 2, value, "confidentiality_protection_result_e");
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
  static const char* options[] = {"performed", "not-performed"};
  return convert_enum_idx(options, 2, value, "integrity_protection_result_e");
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

// DRB-Setup-Mod-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_setup_mod_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(s1_dl_up_tnl_info.pack(bref));
  if (data_forwarding_info_resp_present) {
    HANDLE_CODE(data_forwarding_info_resp.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_transport_params, 1, 8, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_setup_mod_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(s1_dl_up_tnl_info.unpack(bref));
  if (data_forwarding_info_resp_present) {
    HANDLE_CODE(data_forwarding_info_resp.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(ul_up_transport_params, bref, 1, 8, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
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
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

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
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
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
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
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
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

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
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
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
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// EUTRAN-BearerContextModificationResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_mod_resp_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {52, 53, 39, 40};
  return map_enum_number(options, 4, idx, "id");
}
bool eutran_bearer_context_mod_resp_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {52, 53, 39, 40};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_mod_resp_o::value_c::destroy_()
{
  switch (type_) {
    case types::drb_setup_mod_list_eutran:
      c.destroy<drb_setup_mod_list_eutran_l>();
      break;
    case types::drb_failed_mod_list_eutran:
      c.destroy<drb_failed_mod_list_eutran_l>();
      break;
    case types::drb_modified_list_eutran:
      c.destroy<drb_modified_list_eutran_l>();
      break;
    case types::drb_failed_to_modify_list_eutran:
      c.destroy<drb_failed_to_modify_list_eutran_l>();
      break;
    default:
      break;
  }
}
void eutran_bearer_context_mod_resp_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::drb_setup_mod_list_eutran:
      c.init<drb_setup_mod_list_eutran_l>();
      break;
    case types::drb_failed_mod_list_eutran:
      c.init<drb_failed_mod_list_eutran_l>();
      break;
    case types::drb_modified_list_eutran:
      c.init<drb_modified_list_eutran_l>();
      break;
    case types::drb_failed_to_modify_list_eutran:
      c.init<drb_failed_to_modify_list_eutran_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_resp_o::value_c");
  }
}
eutran_bearer_context_mod_resp_o::value_c::value_c(const eutran_bearer_context_mod_resp_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::drb_setup_mod_list_eutran:
      c.init(other.c.get<drb_setup_mod_list_eutran_l>());
      break;
    case types::drb_failed_mod_list_eutran:
      c.init(other.c.get<drb_failed_mod_list_eutran_l>());
      break;
    case types::drb_modified_list_eutran:
      c.init(other.c.get<drb_modified_list_eutran_l>());
      break;
    case types::drb_failed_to_modify_list_eutran:
      c.init(other.c.get<drb_failed_to_modify_list_eutran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_resp_o::value_c");
  }
}
eutran_bearer_context_mod_resp_o::value_c&
eutran_bearer_context_mod_resp_o::value_c::operator=(const eutran_bearer_context_mod_resp_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::drb_setup_mod_list_eutran:
      c.set(other.c.get<drb_setup_mod_list_eutran_l>());
      break;
    case types::drb_failed_mod_list_eutran:
      c.set(other.c.get<drb_failed_mod_list_eutran_l>());
      break;
    case types::drb_modified_list_eutran:
      c.set(other.c.get<drb_modified_list_eutran_l>());
      break;
    case types::drb_failed_to_modify_list_eutran:
      c.set(other.c.get<drb_failed_to_modify_list_eutran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_resp_o::value_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_resp_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_mod_resp_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"DRB-Setup-Mod-List-EUTRAN",
                                  "DRB-Failed-Mod-List-EUTRAN",
                                  "DRB-Modified-List-EUTRAN",
                                  "DRB-Failed-To-Modify-List-EUTRAN"};
  return convert_enum_idx(options, 4, value, "eutran_bearer_context_mod_resp_o::value_c::types");
}

// NG-RAN-BearerContextModificationResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_mod_resp_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {54, 55, 48, 49};
  return map_enum_number(options, 4, idx, "id");
}
bool ng_ran_bearer_context_mod_resp_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {54, 55, 48, 49};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ng_ran_bearer_context_mod_resp_o::value_c::destroy_()
{
  switch (type_) {
    case types::pdu_session_res_setup_mod_list:
      c.destroy<pdu_session_res_setup_mod_list_l>();
      break;
    case types::pdu_session_res_failed_mod_list:
      c.destroy<pdu_session_res_failed_mod_list_l>();
      break;
    case types::pdu_session_res_modified_list:
      c.destroy<pdu_session_res_modified_list_l>();
      break;
    case types::pdu_session_res_failed_to_modify_list:
      c.destroy<pdu_session_res_failed_to_modify_list_l>();
      break;
    default:
      break;
  }
}
void ng_ran_bearer_context_mod_resp_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pdu_session_res_setup_mod_list:
      c.init<pdu_session_res_setup_mod_list_l>();
      break;
    case types::pdu_session_res_failed_mod_list:
      c.init<pdu_session_res_failed_mod_list_l>();
      break;
    case types::pdu_session_res_modified_list:
      c.init<pdu_session_res_modified_list_l>();
      break;
    case types::pdu_session_res_failed_to_modify_list:
      c.init<pdu_session_res_failed_to_modify_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_resp_o::value_c");
  }
}
ng_ran_bearer_context_mod_resp_o::value_c::value_c(const ng_ran_bearer_context_mod_resp_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pdu_session_res_setup_mod_list:
      c.init(other.c.get<pdu_session_res_setup_mod_list_l>());
      break;
    case types::pdu_session_res_failed_mod_list:
      c.init(other.c.get<pdu_session_res_failed_mod_list_l>());
      break;
    case types::pdu_session_res_modified_list:
      c.init(other.c.get<pdu_session_res_modified_list_l>());
      break;
    case types::pdu_session_res_failed_to_modify_list:
      c.init(other.c.get<pdu_session_res_failed_to_modify_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_resp_o::value_c");
  }
}
ng_ran_bearer_context_mod_resp_o::value_c&
ng_ran_bearer_context_mod_resp_o::value_c::operator=(const ng_ran_bearer_context_mod_resp_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pdu_session_res_setup_mod_list:
      c.set(other.c.get<pdu_session_res_setup_mod_list_l>());
      break;
    case types::pdu_session_res_failed_mod_list:
      c.set(other.c.get<pdu_session_res_failed_mod_list_l>());
      break;
    case types::pdu_session_res_modified_list:
      c.set(other.c.get<pdu_session_res_modified_list_l>());
      break;
    case types::pdu_session_res_failed_to_modify_list:
      c.set(other.c.get<pdu_session_res_failed_to_modify_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_resp_o::value_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_resp_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ng_ran_bearer_context_mod_resp_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"PDU-Session-Resource-Setup-Mod-List",
                                  "PDU-Session-Resource-Failed-Mod-List",
                                  "PDU-Session-Resource-Modified-List",
                                  "PDU-Session-Resource-Failed-To-Modify-List"};
  return convert_enum_idx(options, 4, value, "ng_ran_bearer_context_mod_resp_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_mod_resp_o>;

eutran_bearer_context_mod_resp_container::eutran_bearer_context_mod_resp_container() :
  drb_setup_mod_list_eutran(52, crit_e::ignore),
  drb_failed_mod_list_eutran(53, crit_e::ignore),
  drb_modified_list_eutran(39, crit_e::ignore),
  drb_failed_to_modify_list_eutran(40, crit_e::ignore)
{
}
SRSASN_CODE eutran_bearer_context_mod_resp_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += drb_setup_mod_list_eutran_present ? 1 : 0;
  nof_ies += drb_failed_mod_list_eutran_present ? 1 : 0;
  nof_ies += drb_modified_list_eutran_present ? 1 : 0;
  nof_ies += drb_failed_to_modify_list_eutran_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (drb_setup_mod_list_eutran_present) {
    HANDLE_CODE(drb_setup_mod_list_eutran.pack(bref));
  }
  if (drb_failed_mod_list_eutran_present) {
    HANDLE_CODE(drb_failed_mod_list_eutran.pack(bref));
  }
  if (drb_modified_list_eutran_present) {
    HANDLE_CODE(drb_modified_list_eutran.pack(bref));
  }
  if (drb_failed_to_modify_list_eutran_present) {
    HANDLE_CODE(drb_failed_to_modify_list_eutran.pack(bref));
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
    switch (id) {
      case 52: {
        drb_setup_mod_list_eutran_present = true;
        drb_setup_mod_list_eutran.id      = id;
        HANDLE_CODE(drb_setup_mod_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_setup_mod_list_eutran.value.unpack(bref));
        break;
      }
      case 53: {
        drb_failed_mod_list_eutran_present = true;
        drb_failed_mod_list_eutran.id      = id;
        HANDLE_CODE(drb_failed_mod_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_failed_mod_list_eutran.value.unpack(bref));
        break;
      }
      case 39: {
        drb_modified_list_eutran_present = true;
        drb_modified_list_eutran.id      = id;
        HANDLE_CODE(drb_modified_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_modified_list_eutran.value.unpack(bref));
        break;
      }
      case 40: {
        drb_failed_to_modify_list_eutran_present = true;
        drb_failed_to_modify_list_eutran.id      = id;
        HANDLE_CODE(drb_failed_to_modify_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_failed_to_modify_list_eutran.value.unpack(bref));
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
    j.write_fieldname("");
    drb_setup_mod_list_eutran.to_json(j);
  }
  if (drb_failed_mod_list_eutran_present) {
    j.write_fieldname("");
    drb_failed_mod_list_eutran.to_json(j);
  }
  if (drb_modified_list_eutran_present) {
    j.write_fieldname("");
    drb_modified_list_eutran.to_json(j);
  }
  if (drb_failed_to_modify_list_eutran_present) {
    j.write_fieldname("");
    drb_failed_to_modify_list_eutran.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ng_ran_bearer_context_mod_resp_o>;

ng_ran_bearer_context_mod_resp_container::ng_ran_bearer_context_mod_resp_container() :
  pdu_session_res_setup_mod_list(54, crit_e::reject),
  pdu_session_res_failed_mod_list(55, crit_e::reject),
  pdu_session_res_modified_list(48, crit_e::reject),
  pdu_session_res_failed_to_modify_list(49, crit_e::reject)
{
}
SRSASN_CODE ng_ran_bearer_context_mod_resp_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += pdu_session_res_setup_mod_list_present ? 1 : 0;
  nof_ies += pdu_session_res_failed_mod_list_present ? 1 : 0;
  nof_ies += pdu_session_res_modified_list_present ? 1 : 0;
  nof_ies += pdu_session_res_failed_to_modify_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (pdu_session_res_setup_mod_list_present) {
    HANDLE_CODE(pdu_session_res_setup_mod_list.pack(bref));
  }
  if (pdu_session_res_failed_mod_list_present) {
    HANDLE_CODE(pdu_session_res_failed_mod_list.pack(bref));
  }
  if (pdu_session_res_modified_list_present) {
    HANDLE_CODE(pdu_session_res_modified_list.pack(bref));
  }
  if (pdu_session_res_failed_to_modify_list_present) {
    HANDLE_CODE(pdu_session_res_failed_to_modify_list.pack(bref));
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
    switch (id) {
      case 54: {
        pdu_session_res_setup_mod_list_present = true;
        pdu_session_res_setup_mod_list.id      = id;
        HANDLE_CODE(pdu_session_res_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_setup_mod_list.value.unpack(bref));
        break;
      }
      case 55: {
        pdu_session_res_failed_mod_list_present = true;
        pdu_session_res_failed_mod_list.id      = id;
        HANDLE_CODE(pdu_session_res_failed_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_failed_mod_list.value.unpack(bref));
        break;
      }
      case 48: {
        pdu_session_res_modified_list_present = true;
        pdu_session_res_modified_list.id      = id;
        HANDLE_CODE(pdu_session_res_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_modified_list.value.unpack(bref));
        break;
      }
      case 49: {
        pdu_session_res_failed_to_modify_list_present = true;
        pdu_session_res_failed_to_modify_list.id      = id;
        HANDLE_CODE(pdu_session_res_failed_to_modify_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_failed_to_modify_list.value.unpack(bref));
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
    j.write_fieldname("");
    pdu_session_res_setup_mod_list.to_json(j);
  }
  if (pdu_session_res_failed_mod_list_present) {
    j.write_fieldname("");
    pdu_session_res_failed_mod_list.to_json(j);
  }
  if (pdu_session_res_modified_list_present) {
    j.write_fieldname("");
    pdu_session_res_modified_list.to_json(j);
  }
  if (pdu_session_res_failed_to_modify_list_present) {
    j.write_fieldname("");
    pdu_session_res_failed_to_modify_list.to_json(j);
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
  static const char* options[] = {
      "e-UTRAN-BearerContextModificationResponse", "nG-RAN-BearerContextModificationResponse", "choice-extension"};
  return convert_enum_idx(options, 3, value, "sys_bearer_context_mod_resp_c::types");
}

// BearerContextModificationResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 19};
  return map_enum_number(options, 3, idx, "id");
}
bool bearer_context_mod_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 19};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_mod_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::sys_bearer_context_mod_resp:
      c.destroy<sys_bearer_context_mod_resp_c>();
      break;
    default:
      break;
  }
}
void bearer_context_mod_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::sys_bearer_context_mod_resp:
      c.init<sys_bearer_context_mod_resp_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_resp_ies_o::value_c");
  }
}
bearer_context_mod_resp_ies_o::value_c::value_c(const bearer_context_mod_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_resp:
      c.init(other.c.get<sys_bearer_context_mod_resp_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_resp_ies_o::value_c");
  }
}
bearer_context_mod_resp_ies_o::value_c&
bearer_context_mod_resp_ies_o::value_c::operator=(const bearer_context_mod_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_resp:
      c.set(other.c.get<sys_bearer_context_mod_resp_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_resp_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_mod_resp_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_resp_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_bearer_context_mod_resp_c& bearer_context_mod_resp_ies_o::value_c::sys_bearer_context_mod_resp()
{
  assert_choice_type(types::sys_bearer_context_mod_resp, type_, "Value");
  return c.get<sys_bearer_context_mod_resp_c>();
}
const uint64_t& bearer_context_mod_resp_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_resp_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-BearerContextModificationResponse"};
  return convert_enum_idx(options, 3, value, "bearer_context_mod_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_resp_ies_o>;

bearer_context_mod_resp_ies_container::bearer_context_mod_resp_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  sys_bearer_context_mod_resp(19, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_mod_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += sys_bearer_context_mod_resp_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  if (sys_bearer_context_mod_resp_present) {
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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 19: {
        sys_bearer_context_mod_resp_present = true;
        sys_bearer_context_mod_resp.id      = id;
        HANDLE_CODE(sys_bearer_context_mod_resp.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_mod_resp.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  if (sys_bearer_context_mod_resp_present) {
    j.write_fieldname("");
    sys_bearer_context_mod_resp.to_json(j);
  }
  j.end_obj();
}

// BearerContextReleaseCommandIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_release_cmd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 0};
  return map_enum_number(options, 3, idx, "id");
}
bool bearer_context_release_cmd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 0};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_release_cmd_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    default:
      break;
  }
}
void bearer_context_release_cmd_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_cmd_ies_o::value_c");
  }
}
bearer_context_release_cmd_ies_o::value_c::value_c(const bearer_context_release_cmd_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_cmd_ies_o::value_c");
  }
}
bearer_context_release_cmd_ies_o::value_c&
bearer_context_release_cmd_ies_o::value_c::operator=(const bearer_context_release_cmd_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_cmd_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_release_cmd_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_release_cmd_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& bearer_context_release_cmd_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const uint64_t& bearer_context_release_cmd_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_release_cmd_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause"};
  return convert_enum_idx(options, 3, value, "bearer_context_release_cmd_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_release_cmd_ies_o>;

bearer_context_release_cmd_ies_container::bearer_context_release_cmd_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject), gnb_cu_up_ue_e1_ap_id(3, crit_e::reject), cause(0, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_release_cmd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  j.end_obj();
}

// BearerContextReleaseCompleteIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_release_complete_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 1};
  return map_enum_number(options, 3, idx, "id");
}
bool bearer_context_release_complete_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 1};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_release_complete_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void bearer_context_release_complete_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_complete_ies_o::value_c");
  }
}
bearer_context_release_complete_ies_o::value_c::value_c(const bearer_context_release_complete_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_complete_ies_o::value_c");
  }
}
bearer_context_release_complete_ies_o::value_c&
bearer_context_release_complete_ies_o::value_c::operator=(const bearer_context_release_complete_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_complete_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_release_complete_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_release_complete_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
crit_diagnostics_s& bearer_context_release_complete_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint64_t& bearer_context_release_complete_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_release_complete_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const crit_diagnostics_s& bearer_context_release_complete_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void bearer_context_release_complete_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_complete_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_release_complete_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 3, value, "bearer_context_release_complete_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_release_complete_ies_o>;

bearer_context_release_complete_ies_container::bearer_context_release_complete_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_release_complete_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 1: {
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
void bearer_context_release_complete_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
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

// BearerContextReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_release_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 22, 0};
  return map_enum_number(options, 4, idx, "id");
}
bool bearer_context_release_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 22, 0};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_release_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::drb_status_list:
      c.destroy<drb_status_list_l>();
      break;
    case types::cause:
      c.destroy<cause_c>();
      break;
    default:
      break;
  }
}
void bearer_context_release_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::drb_status_list:
      c.init<drb_status_list_l>();
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_request_ies_o::value_c");
  }
}
bearer_context_release_request_ies_o::value_c::value_c(const bearer_context_release_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::drb_status_list:
      c.init(other.c.get<drb_status_list_l>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_request_ies_o::value_c");
  }
}
bearer_context_release_request_ies_o::value_c&
bearer_context_release_request_ies_o::value_c::operator=(const bearer_context_release_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::drb_status_list:
      c.set(other.c.get<drb_status_list_l>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_request_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_release_request_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_release_request_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
const uint64_t& bearer_context_release_request_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_release_request_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "DRB-Status-List", "Cause"};
  return convert_enum_idx(options, 4, value, "bearer_context_release_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_release_request_ies_o>;

bearer_context_release_request_ies_container::bearer_context_release_request_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  drb_status_list(22, crit_e::ignore),
  cause(0, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_release_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += drb_status_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  if (drb_status_list_present) {
    HANDLE_CODE(drb_status_list.pack(bref));
  }
  HANDLE_CODE(cause.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 22: {
        drb_status_list_present = true;
        drb_status_list.id      = id;
        HANDLE_CODE(drb_status_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_status_list.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  if (drb_status_list_present) {
    j.write_fieldname("");
    drb_status_list.to_json(j);
  }
  j.write_fieldname("");
  cause.to_json(j);
  j.end_obj();
}

// BearerContextSetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 0, 1};
  return map_enum_number(options, 4, idx, "id");
}
bool bearer_context_setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 0, 1};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_setup_fail_ies_o::value_c::destroy_()
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
void bearer_context_setup_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "bearer_context_setup_fail_ies_o::value_c");
  }
}
bearer_context_setup_fail_ies_o::value_c::value_c(const bearer_context_setup_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "bearer_context_setup_fail_ies_o::value_c");
  }
}
bearer_context_setup_fail_ies_o::value_c&
bearer_context_setup_fail_ies_o::value_c::operator=(const bearer_context_setup_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "bearer_context_setup_fail_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_setup_fail_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_setup_fail_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
const uint64_t& bearer_context_setup_fail_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_setup_fail_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "bearer_context_setup_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_setup_fail_ies_o>;

bearer_context_setup_fail_ies_container::bearer_context_setup_fail_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::ignore),
  cause(0, crit_e::ignore),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += gnb_cu_up_ue_e1_ap_id_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  if (gnb_cu_up_ue_e1_ap_id_present) {
    HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  }
  HANDLE_CODE(cause.pack(bref));
  if (crit_diagnostics_present) {
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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        gnb_cu_up_ue_e1_ap_id_present = true;
        gnb_cu_up_ue_e1_ap_id.id      = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
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
      case 1: {
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
void bearer_context_setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  if (gnb_cu_up_ue_e1_ap_id_present) {
    j.write_fieldname("");
    gnb_cu_up_ue_e1_ap_id.to_json(j);
  }
  j.write_fieldname("");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

// PDU-Session-Resource-To-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t pdu_session_res_to_setup_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {78};
  return map_enum_number(options, 1, idx, "id");
}
bool pdu_session_res_to_setup_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 78 == id;
}
crit_e pdu_session_res_to_setup_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 78) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
pdu_session_res_to_setup_item_ext_ies_o::ext_c pdu_session_res_to_setup_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 78) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pdu_session_res_to_setup_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 78) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void pdu_session_res_to_setup_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("OCTET STRING", c.to_string());
  j.end_obj();
}
SRSASN_CODE pdu_session_res_to_setup_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdu_session_res_to_setup_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* pdu_session_res_to_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* options[] = {"OCTET STRING"};
  return convert_enum_idx(options, 1, value, "pdu_session_res_to_setup_item_ext_ies_o::ext_c::types");
}

// DRB-To-Setup-Item-EUTRAN ::= SEQUENCE
SRSASN_CODE drb_to_setup_item_eutran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_forwarding_info_request_present, 1));
  HANDLE_CODE(bref.pack(dl_up_params.size() > 0, 1));
  HANDLE_CODE(bref.pack(drb_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(existing_allocated_s1_dl_up_tnl_info_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

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
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
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
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
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
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

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
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
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
  bool ie_exts_present;
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
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
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
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// EUTRAN-BearerContextSetupRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_setup_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {32};
  return map_enum_number(options, 1, idx, "id");
}
bool eutran_bearer_context_setup_request_o::is_id_valid(const uint32_t& id)
{
  return 32 == id;
}
crit_e eutran_bearer_context_setup_request_o::get_crit(const uint32_t& id)
{
  if (id == 32) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
eutran_bearer_context_setup_request_o::value_c eutran_bearer_context_setup_request_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 32) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e eutran_bearer_context_setup_request_o::get_presence(const uint32_t& id)
{
  if (id == 32) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_setup_request_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("DRB-To-Setup-List-EUTRAN");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE eutran_bearer_context_setup_request_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 32, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_setup_request_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 32, true));
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_setup_request_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"DRB-To-Setup-List-EUTRAN"};
  return convert_enum_idx(options, 1, value, "eutran_bearer_context_setup_request_o::value_c::types");
}

// NG-RAN-BearerContextSetupRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_setup_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {42};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"PDU-Session-Resource-To-Setup-List"};
  return convert_enum_idx(options, 1, value, "ng_ran_bearer_context_setup_request_o::value_c::types");
}

// System-BearerContextSetupRequest ::= CHOICE
void sys_bearer_context_setup_request_c::destroy_()
{
  switch (type_) {
    case types::e_utran_bearer_context_setup_request:
      c.destroy<protocol_ie_container_l<eutran_bearer_context_setup_request_o>>();
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
      c.init<protocol_ie_container_l<eutran_bearer_context_setup_request_o>>();
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
      c.init(other.c.get<protocol_ie_container_l<eutran_bearer_context_setup_request_o>>());
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
      c.set(other.c.get<protocol_ie_container_l<eutran_bearer_context_setup_request_o>>());
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
protocol_ie_container_l<eutran_bearer_context_setup_request_o>&
sys_bearer_context_setup_request_c::set_e_utran_bearer_context_setup_request()
{
  set(types::e_utran_bearer_context_setup_request);
  return c.get<protocol_ie_container_l<eutran_bearer_context_setup_request_o>>();
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
      HANDLE_CODE(pack_dyn_seq_of(
          bref, c.get<protocol_ie_container_l<eutran_bearer_context_setup_request_o>>(), 0, 65535, true));
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
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<protocol_ie_container_l<eutran_bearer_context_setup_request_o>>(), bref, 0, 65535, true));
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
  static const char* options[] = {
      "e-UTRAN-BearerContextSetupRequest", "nG-RAN-BearerContextSetupRequest", "choice-extension"};
  return convert_enum_idx(options, 3, value, "sys_bearer_context_setup_request_c::types");
}

// BearerContextSetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 13, 14, 66, 58, 23, 59, 17, 15, 76, 77};
  return map_enum_number(options, 11, idx, "id");
}
bool bearer_context_setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 13, 14, 66, 58, 23, 59, 17, 15, 76, 77};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 13:
      ret.set(value_c::types::security_info);
      break;
    case 14:
      ret.set(value_c::types::uedl_aggregate_maximum_bit_rate);
      break;
    case 66:
      ret.set(value_c::types::uedl_maximum_integrity_protected_data_rate);
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
      ret.set(value_c::types::ranueid);
      break;
    case 77:
      ret.set(value_c::types::gnb_du_id);
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_setup_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::security_info:
      c.destroy<security_info_s>();
      break;
    case types::serving_plmn:
      c.destroy<fixed_octstring<3, true>>();
      break;
    case types::sys_bearer_context_setup_request:
      c.destroy<sys_bearer_context_setup_request_c>();
      break;
    case types::ranueid:
      c.destroy<fixed_octstring<8, true>>();
      break;
    default:
      break;
  }
}
void bearer_context_setup_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::security_info:
      c.init<security_info_s>();
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
      break;
    case types::serving_plmn:
      c.init<fixed_octstring<3, true>>();
      break;
    case types::activity_notif_level:
      break;
    case types::ue_inactivity_timer:
      break;
    case types::bearer_context_status_change:
      break;
    case types::sys_bearer_context_setup_request:
      c.init<sys_bearer_context_setup_request_c>();
      break;
    case types::ranueid:
      c.init<fixed_octstring<8, true>>();
      break;
    case types::gnb_du_id:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_request_ies_o::value_c");
  }
}
bearer_context_setup_request_ies_o::value_c::value_c(const bearer_context_setup_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::security_info:
      c.init(other.c.get<security_info_s>());
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      c.init(other.c.get<uint64_t>());
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
      c.init(other.c.get<uint64_t>());
      break;
    case types::serving_plmn:
      c.init(other.c.get<fixed_octstring<3, true>>());
      break;
    case types::activity_notif_level:
      c.init(other.c.get<activity_notif_level_e>());
      break;
    case types::ue_inactivity_timer:
      c.init(other.c.get<uint16_t>());
      break;
    case types::bearer_context_status_change:
      c.init(other.c.get<bearer_context_status_change_e>());
      break;
    case types::sys_bearer_context_setup_request:
      c.init(other.c.get<sys_bearer_context_setup_request_c>());
      break;
    case types::ranueid:
      c.init(other.c.get<fixed_octstring<8, true>>());
      break;
    case types::gnb_du_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_request_ies_o::value_c");
  }
}
bearer_context_setup_request_ies_o::value_c&
bearer_context_setup_request_ies_o::value_c::operator=(const bearer_context_setup_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::security_info:
      c.set(other.c.get<security_info_s>());
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      c.set(other.c.get<uint64_t>());
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
      c.set(other.c.get<uint64_t>());
      break;
    case types::serving_plmn:
      c.set(other.c.get<fixed_octstring<3, true>>());
      break;
    case types::activity_notif_level:
      c.set(other.c.get<activity_notif_level_e>());
      break;
    case types::ue_inactivity_timer:
      c.set(other.c.get<uint16_t>());
      break;
    case types::bearer_context_status_change:
      c.set(other.c.get<bearer_context_status_change_e>());
      break;
    case types::sys_bearer_context_setup_request:
      c.set(other.c.get<sys_bearer_context_setup_request_c>());
      break;
    case types::ranueid:
      c.set(other.c.get<fixed_octstring<8, true>>());
      break;
    case types::gnb_du_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_request_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
security_info_s& bearer_context_setup_request_ies_o::value_c::security_info()
{
  assert_choice_type(types::security_info, type_, "Value");
  return c.get<security_info_s>();
}
uint64_t& bearer_context_setup_request_ies_o::value_c::uedl_aggregate_maximum_bit_rate()
{
  assert_choice_type(types::uedl_aggregate_maximum_bit_rate, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_setup_request_ies_o::value_c::uedl_maximum_integrity_protected_data_rate()
{
  assert_choice_type(types::uedl_maximum_integrity_protected_data_rate, type_, "Value");
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
fixed_octstring<8, true>& bearer_context_setup_request_ies_o::value_c::ranueid()
{
  assert_choice_type(types::ranueid, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_du_id()
{
  assert_choice_type(types::gnb_du_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const security_info_s& bearer_context_setup_request_ies_o::value_c::security_info() const
{
  assert_choice_type(types::security_info, type_, "Value");
  return c.get<security_info_s>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::uedl_aggregate_maximum_bit_rate() const
{
  assert_choice_type(types::uedl_aggregate_maximum_bit_rate, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::uedl_maximum_integrity_protected_data_rate() const
{
  assert_choice_type(types::uedl_maximum_integrity_protected_data_rate, type_, "Value");
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
const fixed_octstring<8, true>& bearer_context_setup_request_ies_o::value_c::ranueid() const
{
  assert_choice_type(types::ranueid, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_du_id() const
{
  assert_choice_type(types::gnb_du_id, type_, "Value");
  return c.get<uint64_t>();
}
void bearer_context_setup_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::security_info:
      j.write_fieldname("SecurityInformation");
      c.get<security_info_s>().to_json(j);
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
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
    case types::ranueid:
      j.write_str("OCTET STRING", c.get<fixed_octstring<8, true>>().to_string());
      break;
    case types::gnb_du_id:
      j.write_int("INTEGER (0..68719476735)", c.get<uint64_t>());
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::security_info:
      HANDLE_CODE(c.get<security_info_s>().pack(bref));
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
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
    case types::ranueid:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().pack(bref)));
      break;
    case types::gnb_du_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)68719476735u, false, true));
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::security_info:
      HANDLE_CODE(c.get<security_info_s>().unpack(bref));
      break;
    case types::uedl_aggregate_maximum_bit_rate:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::uedl_maximum_integrity_protected_data_rate:
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
    case types::ranueid:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().unpack(bref)));
      break;
    case types::gnb_du_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..4294967295)",
                                  "SecurityInformation",
                                  "INTEGER (0..4000000000000,...)",
                                  "INTEGER (0..4000000000000,...)",
                                  "OCTET STRING",
                                  "ActivityNotificationLevel",
                                  "INTEGER (1..7200,...)",
                                  "BearerContextStatusChange",
                                  "System-BearerContextSetupRequest",
                                  "OCTET STRING",
                                  "INTEGER (0..68719476735)"};
  return convert_enum_idx(options, 11, value, "bearer_context_setup_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_setup_request_ies_o>;

bearer_context_setup_request_ies_container::bearer_context_setup_request_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  security_info(13, crit_e::reject),
  uedl_aggregate_maximum_bit_rate(14, crit_e::reject),
  uedl_maximum_integrity_protected_data_rate(66, crit_e::reject),
  serving_plmn(58, crit_e::ignore),
  activity_notif_level(23, crit_e::reject),
  ue_inactivity_timer(59, crit_e::reject),
  bearer_context_status_change(17, crit_e::reject),
  sys_bearer_context_setup_request(15, crit_e::reject),
  ranueid(76, crit_e::ignore),
  gnb_du_id(77, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 6;
  nof_ies += uedl_maximum_integrity_protected_data_rate_present ? 1 : 0;
  nof_ies += ue_inactivity_timer_present ? 1 : 0;
  nof_ies += bearer_context_status_change_present ? 1 : 0;
  nof_ies += ranueid_present ? 1 : 0;
  nof_ies += gnb_du_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(security_info.pack(bref));
  HANDLE_CODE(uedl_aggregate_maximum_bit_rate.pack(bref));
  if (uedl_maximum_integrity_protected_data_rate_present) {
    HANDLE_CODE(uedl_maximum_integrity_protected_data_rate.pack(bref));
  }
  HANDLE_CODE(serving_plmn.pack(bref));
  HANDLE_CODE(activity_notif_level.pack(bref));
  if (ue_inactivity_timer_present) {
    HANDLE_CODE(ue_inactivity_timer.pack(bref));
  }
  if (bearer_context_status_change_present) {
    HANDLE_CODE(bearer_context_status_change.pack(bref));
  }
  HANDLE_CODE(sys_bearer_context_setup_request.pack(bref));
  if (ranueid_present) {
    HANDLE_CODE(ranueid.pack(bref));
  }
  if (gnb_du_id_present) {
    HANDLE_CODE(gnb_du_id.pack(bref));
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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 13: {
        nof_mandatory_ies--;
        security_info.id = id;
        HANDLE_CODE(security_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(security_info.value.unpack(bref));
        break;
      }
      case 14: {
        nof_mandatory_ies--;
        uedl_aggregate_maximum_bit_rate.id = id;
        HANDLE_CODE(uedl_aggregate_maximum_bit_rate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uedl_aggregate_maximum_bit_rate.value.unpack(bref));
        break;
      }
      case 66: {
        uedl_maximum_integrity_protected_data_rate_present = true;
        uedl_maximum_integrity_protected_data_rate.id      = id;
        HANDLE_CODE(uedl_maximum_integrity_protected_data_rate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uedl_maximum_integrity_protected_data_rate.value.unpack(bref));
        break;
      }
      case 58: {
        nof_mandatory_ies--;
        serving_plmn.id = id;
        HANDLE_CODE(serving_plmn.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_plmn.value.unpack(bref));
        break;
      }
      case 23: {
        nof_mandatory_ies--;
        activity_notif_level.id = id;
        HANDLE_CODE(activity_notif_level.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(activity_notif_level.value.unpack(bref));
        break;
      }
      case 59: {
        ue_inactivity_timer_present = true;
        ue_inactivity_timer.id      = id;
        HANDLE_CODE(ue_inactivity_timer.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_inactivity_timer.value.unpack(bref));
        break;
      }
      case 17: {
        bearer_context_status_change_present = true;
        bearer_context_status_change.id      = id;
        HANDLE_CODE(bearer_context_status_change.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bearer_context_status_change.value.unpack(bref));
        break;
      }
      case 15: {
        nof_mandatory_ies--;
        sys_bearer_context_setup_request.id = id;
        HANDLE_CODE(sys_bearer_context_setup_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_setup_request.value.unpack(bref));
        break;
      }
      case 76: {
        ranueid_present = true;
        ranueid.id      = id;
        HANDLE_CODE(ranueid.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ranueid.value.unpack(bref));
        break;
      }
      case 77: {
        gnb_du_id_present = true;
        gnb_du_id.id      = id;
        HANDLE_CODE(gnb_du_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_id.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  security_info.to_json(j);
  j.write_fieldname("");
  uedl_aggregate_maximum_bit_rate.to_json(j);
  if (uedl_maximum_integrity_protected_data_rate_present) {
    j.write_fieldname("");
    uedl_maximum_integrity_protected_data_rate.to_json(j);
  }
  j.write_fieldname("");
  serving_plmn.to_json(j);
  j.write_fieldname("");
  activity_notif_level.to_json(j);
  if (ue_inactivity_timer_present) {
    j.write_fieldname("");
    ue_inactivity_timer.to_json(j);
  }
  if (bearer_context_status_change_present) {
    j.write_fieldname("");
    bearer_context_status_change.to_json(j);
  }
  j.write_fieldname("");
  sys_bearer_context_setup_request.to_json(j);
  if (ranueid_present) {
    j.write_fieldname("");
    ranueid.to_json(j);
  }
  if (gnb_du_id_present) {
    j.write_fieldname("");
    gnb_du_id.to_json(j);
  }
  j.end_obj();
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
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u, true, true));
  HANDLE_CODE(s1_dl_up_tnl_info.pack(bref));
  if (data_forwarding_info_resp_present) {
    HANDLE_CODE(data_forwarding_info_resp.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_transport_params, 1, 8, true));
  if (s1_dl_up_unchanged_present) {
    HANDLE_CODE(s1_dl_up_unchanged.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_setup_item_eutran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_forwarding_info_resp_present, 1));
  HANDLE_CODE(bref.unpack(s1_dl_up_unchanged_present, 1));
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
    HANDLE_CODE(ie_exts.unpack(bref));
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
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

const char* drb_setup_item_eutran_s::s1_dl_up_unchanged_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "drb_setup_item_eutran_s::s1_dl_up_unchanged_e_");
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
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "pdu_session_res_setup_item_s::ng_dl_up_unchanged_e_");
}

// EUTRAN-BearerContextSetupResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_setup_resp_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {37, 38};
  return map_enum_number(options, 2, idx, "id");
}
bool eutran_bearer_context_setup_resp_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {37, 38};
  for (const auto& o : options) {
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
void eutran_bearer_context_setup_resp_o::value_c::destroy_()
{
  switch (type_) {
    case types::drb_setup_list_eutran:
      c.destroy<drb_setup_list_eutran_l>();
      break;
    case types::drb_failed_list_eutran:
      c.destroy<drb_failed_list_eutran_l>();
      break;
    default:
      break;
  }
}
void eutran_bearer_context_setup_resp_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::drb_setup_list_eutran:
      c.init<drb_setup_list_eutran_l>();
      break;
    case types::drb_failed_list_eutran:
      c.init<drb_failed_list_eutran_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_resp_o::value_c");
  }
}
eutran_bearer_context_setup_resp_o::value_c::value_c(const eutran_bearer_context_setup_resp_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::drb_setup_list_eutran:
      c.init(other.c.get<drb_setup_list_eutran_l>());
      break;
    case types::drb_failed_list_eutran:
      c.init(other.c.get<drb_failed_list_eutran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_resp_o::value_c");
  }
}
eutran_bearer_context_setup_resp_o::value_c&
eutran_bearer_context_setup_resp_o::value_c::operator=(const eutran_bearer_context_setup_resp_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::drb_setup_list_eutran:
      c.set(other.c.get<drb_setup_list_eutran_l>());
      break;
    case types::drb_failed_list_eutran:
      c.set(other.c.get<drb_failed_list_eutran_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_resp_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"DRB-Setup-List-EUTRAN", "DRB-Failed-List-EUTRAN"};
  return convert_enum_idx(options, 2, value, "eutran_bearer_context_setup_resp_o::value_c::types");
}

// NG-RAN-BearerContextSetupResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_setup_resp_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {46, 47};
  return map_enum_number(options, 2, idx, "id");
}
bool ng_ran_bearer_context_setup_resp_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {46, 47};
  for (const auto& o : options) {
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
void ng_ran_bearer_context_setup_resp_o::value_c::destroy_()
{
  switch (type_) {
    case types::pdu_session_res_setup_list:
      c.destroy<pdu_session_res_setup_list_l>();
      break;
    case types::pdu_session_res_failed_list:
      c.destroy<pdu_session_res_failed_list_l>();
      break;
    default:
      break;
  }
}
void ng_ran_bearer_context_setup_resp_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pdu_session_res_setup_list:
      c.init<pdu_session_res_setup_list_l>();
      break;
    case types::pdu_session_res_failed_list:
      c.init<pdu_session_res_failed_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_setup_resp_o::value_c");
  }
}
ng_ran_bearer_context_setup_resp_o::value_c::value_c(const ng_ran_bearer_context_setup_resp_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pdu_session_res_setup_list:
      c.init(other.c.get<pdu_session_res_setup_list_l>());
      break;
    case types::pdu_session_res_failed_list:
      c.init(other.c.get<pdu_session_res_failed_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_setup_resp_o::value_c");
  }
}
ng_ran_bearer_context_setup_resp_o::value_c&
ng_ran_bearer_context_setup_resp_o::value_c::operator=(const ng_ran_bearer_context_setup_resp_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pdu_session_res_setup_list:
      c.set(other.c.get<pdu_session_res_setup_list_l>());
      break;
    case types::pdu_session_res_failed_list:
      c.set(other.c.get<pdu_session_res_failed_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_setup_resp_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"PDU-Session-Resource-Setup-List", "PDU-Session-Resource-Failed-List"};
  return convert_enum_idx(options, 2, value, "ng_ran_bearer_context_setup_resp_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_setup_resp_o>;

eutran_bearer_context_setup_resp_container::eutran_bearer_context_setup_resp_container() :
  drb_setup_list_eutran(37, crit_e::ignore), drb_failed_list_eutran(38, crit_e::ignore)
{
}
SRSASN_CODE eutran_bearer_context_setup_resp_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += drb_failed_list_eutran_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(drb_setup_list_eutran.pack(bref));
  if (drb_failed_list_eutran_present) {
    HANDLE_CODE(drb_failed_list_eutran.pack(bref));
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
    switch (id) {
      case 37: {
        nof_mandatory_ies--;
        drb_setup_list_eutran.id = id;
        HANDLE_CODE(drb_setup_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_setup_list_eutran.value.unpack(bref));
        break;
      }
      case 38: {
        drb_failed_list_eutran_present = true;
        drb_failed_list_eutran.id      = id;
        HANDLE_CODE(drb_failed_list_eutran.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_failed_list_eutran.value.unpack(bref));
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
  j.write_fieldname("");
  drb_setup_list_eutran.to_json(j);
  if (drb_failed_list_eutran_present) {
    j.write_fieldname("");
    drb_failed_list_eutran.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ng_ran_bearer_context_setup_resp_o>;

ng_ran_bearer_context_setup_resp_container::ng_ran_bearer_context_setup_resp_container() :
  pdu_session_res_setup_list(46, crit_e::ignore), pdu_session_res_failed_list(47, crit_e::ignore)
{
}
SRSASN_CODE ng_ran_bearer_context_setup_resp_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += pdu_session_res_failed_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(pdu_session_res_setup_list.pack(bref));
  if (pdu_session_res_failed_list_present) {
    HANDLE_CODE(pdu_session_res_failed_list.pack(bref));
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
    switch (id) {
      case 46: {
        nof_mandatory_ies--;
        pdu_session_res_setup_list.id = id;
        HANDLE_CODE(pdu_session_res_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_setup_list.value.unpack(bref));
        break;
      }
      case 47: {
        pdu_session_res_failed_list_present = true;
        pdu_session_res_failed_list.id      = id;
        HANDLE_CODE(pdu_session_res_failed_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_failed_list.value.unpack(bref));
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
  j.write_fieldname("");
  pdu_session_res_setup_list.to_json(j);
  if (pdu_session_res_failed_list_present) {
    j.write_fieldname("");
    pdu_session_res_failed_list.to_json(j);
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
  static const char* options[] = {
      "e-UTRAN-BearerContextSetupResponse", "nG-RAN-BearerContextSetupResponse", "choice-extension"};
  return convert_enum_idx(options, 3, value, "sys_bearer_context_setup_resp_c::types");
}

// BearerContextSetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 16};
  return map_enum_number(options, 3, idx, "id");
}
bool bearer_context_setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 16};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void bearer_context_setup_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::sys_bearer_context_setup_resp:
      c.destroy<sys_bearer_context_setup_resp_c>();
      break;
    default:
      break;
  }
}
void bearer_context_setup_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::sys_bearer_context_setup_resp:
      c.init<sys_bearer_context_setup_resp_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_resp_ies_o::value_c");
  }
}
bearer_context_setup_resp_ies_o::value_c::value_c(const bearer_context_setup_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sys_bearer_context_setup_resp:
      c.init(other.c.get<sys_bearer_context_setup_resp_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_resp_ies_o::value_c");
  }
}
bearer_context_setup_resp_ies_o::value_c&
bearer_context_setup_resp_ies_o::value_c::operator=(const bearer_context_setup_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sys_bearer_context_setup_resp:
      c.set(other.c.get<sys_bearer_context_setup_resp_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_resp_ies_o::value_c");
  }

  return *this;
}
uint64_t& bearer_context_setup_resp_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_setup_resp_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_bearer_context_setup_resp_c& bearer_context_setup_resp_ies_o::value_c::sys_bearer_context_setup_resp()
{
  assert_choice_type(types::sys_bearer_context_setup_resp, type_, "Value");
  return c.get<sys_bearer_context_setup_resp_c>();
}
const uint64_t& bearer_context_setup_resp_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_setup_resp_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-BearerContextSetupResponse"};
  return convert_enum_idx(options, 3, value, "bearer_context_setup_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_setup_resp_ies_o>;

bearer_context_setup_resp_ies_container::bearer_context_setup_resp_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  sys_bearer_context_setup_resp(16, crit_e::ignore)
{
}
SRSASN_CODE bearer_context_setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(sys_bearer_context_setup_resp.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 16: {
        nof_mandatory_ies--;
        sys_bearer_context_setup_resp.id = id;
        HANDLE_CODE(sys_bearer_context_setup_resp.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_setup_resp.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  sys_bearer_context_setup_resp.to_json(j);
  j.end_obj();
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
  static const uint32_t options[] = {74};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"Endpoint-IP-address-and-port"};
  return convert_enum_idx(options, 1, value, "cp_tnl_info_ext_ies_o::value_c::types");
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
  static const char* options[] = {"endpoint-IP-Address", "choice-extension"};
  return convert_enum_idx(options, 2, value, "cp_tnl_info_c::types");
}

// DLDataNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t dl_data_notif_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 63};
  return map_enum_number(options, 3, idx, "id");
}
bool dl_data_notif_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 63};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
      break;
    case 63:
      ret.set(value_c::types::ppi);
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void dl_data_notif_ies_o::value_c::destroy_() {}
void dl_data_notif_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
dl_data_notif_ies_o::value_c::value_c(const dl_data_notif_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::ppi:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_data_notif_ies_o::value_c");
  }
}
dl_data_notif_ies_o::value_c& dl_data_notif_ies_o::value_c::operator=(const dl_data_notif_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::ppi:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_data_notif_ies_o::value_c");
  }

  return *this;
}
uint64_t& dl_data_notif_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& dl_data_notif_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint8_t& dl_data_notif_ies_o::value_c::ppi()
{
  assert_choice_type(types::ppi, type_, "Value");
  return c.get<uint8_t>();
}
const uint64_t& dl_data_notif_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& dl_data_notif_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint8_t& dl_data_notif_ies_o::value_c::ppi() const
{
  assert_choice_type(types::ppi, type_, "Value");
  return c.get<uint8_t>();
}
void dl_data_notif_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::ppi:
      j.write_int("INTEGER (0..7,...)", c.get<uint8_t>());
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::ppi:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u, true, true));
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::ppi:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "dl_data_notif_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_data_notif_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "INTEGER (0..7,...)"};
  return convert_enum_idx(options, 3, value, "dl_data_notif_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_data_notif_ies_o>;

dl_data_notif_ies_container::dl_data_notif_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject), gnb_cu_up_ue_e1_ap_id(3, crit_e::reject), ppi(63, crit_e::ignore)
{
}
SRSASN_CODE dl_data_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += ppi_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  if (ppi_present) {
    HANDLE_CODE(ppi.pack(bref));
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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 63: {
        ppi_present = true;
        ppi.id      = id;
        HANDLE_CODE(ppi.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ppi.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  if (ppi_present) {
    j.write_fieldname("");
    ppi.to_json(j);
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
  static const char* options[] = {"nR", "e-UTRA"};
  return convert_enum_idx(options, 2, value, "data_usage_per_pdu_session_report_s::secondary_rat_type_e_");
}

// Data-Usage-per-QoS-Flow-Item ::= SEQUENCE
SRSASN_CODE data_usage_per_qos_flow_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, qo_s_flow_id, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(secondary_rat_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, qo_s_flow_timed_report_list, 1, 2, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_usage_per_qos_flow_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(unpack_integer(qo_s_flow_id, bref, (uint8_t)0u, (uint8_t)63u, false, true));
  HANDLE_CODE(secondary_rat_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(qo_s_flow_timed_report_list, bref, 1, 2, true));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void data_usage_per_qos_flow_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("qoS-Flow-Identifier", qo_s_flow_id);
  j.write_str("secondaryRATType", secondary_rat_type.to_string());
  j.start_array("qoS-Flow-Timed-Report-List");
  for (const auto& e1 : qo_s_flow_timed_report_list) {
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
  static const char* options[] = {"nR", "e-UTRA"};
  return convert_enum_idx(options, 2, value, "data_usage_per_qos_flow_item_s::secondary_rat_type_e_");
}

// DataUsageReportIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t data_usage_report_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 25};
  return map_enum_number(options, 3, idx, "id");
}
bool data_usage_report_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 25};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void data_usage_report_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::data_usage_report_list:
      c.destroy<data_usage_report_list_l>();
      break;
    default:
      break;
  }
}
void data_usage_report_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::data_usage_report_list:
      c.init<data_usage_report_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "data_usage_report_ies_o::value_c");
  }
}
data_usage_report_ies_o::value_c::value_c(const data_usage_report_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::data_usage_report_list:
      c.init(other.c.get<data_usage_report_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "data_usage_report_ies_o::value_c");
  }
}
data_usage_report_ies_o::value_c&
data_usage_report_ies_o::value_c::operator=(const data_usage_report_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::data_usage_report_list:
      c.set(other.c.get<data_usage_report_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "data_usage_report_ies_o::value_c");
  }

  return *this;
}
uint64_t& data_usage_report_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& data_usage_report_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
data_usage_report_list_l& data_usage_report_ies_o::value_c::data_usage_report_list()
{
  assert_choice_type(types::data_usage_report_list, type_, "Value");
  return c.get<data_usage_report_list_l>();
}
const uint64_t& data_usage_report_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& data_usage_report_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Data-Usage-Report-List"};
  return convert_enum_idx(options, 3, value, "data_usage_report_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<data_usage_report_ies_o>;

data_usage_report_ies_container::data_usage_report_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  data_usage_report_list(25, crit_e::ignore)
{
}
SRSASN_CODE data_usage_report_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(data_usage_report_list.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 25: {
        nof_mandatory_ies--;
        data_usage_report_list.id = id;
        HANDLE_CODE(data_usage_report_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(data_usage_report_list.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  data_usage_report_list.to_json(j);
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

// NG-RAN-QoS-Support-Item ::= SEQUENCE
SRSASN_CODE ng_ran_qos_support_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(non_dynamic5_qi_descriptor.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_qos_support_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(non_dynamic5_qi_descriptor.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ng_ran_qos_support_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("non-Dynamic5QIDescriptor");
  non_dynamic5_qi_descriptor.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
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

// UE-associatedLogicalE1-ConnectionItem ::= SEQUENCE
SRSASN_CODE ue_associated_lc_e1_conn_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gnb_cu_cp_ue_e1_ap_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_up_ue_e1_ap_id_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  if (gnb_cu_cp_ue_e1_ap_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_cu_up_ue_e1_ap_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_associated_lc_e1_conn_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gnb_cu_cp_ue_e1_ap_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_up_ue_e1_ap_id_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  if (gnb_cu_cp_ue_e1_ap_id_present) {
    HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_cu_up_ue_e1_ap_id_present) {
    HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_associated_lc_e1_conn_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gnb_cu_cp_ue_e1_ap_id_present) {
    j.write_int("gNB-CU-CP-UE-E1AP-ID", gnb_cu_cp_ue_e1_ap_id);
  }
  if (gnb_cu_up_ue_e1_ap_id_present) {
    j.write_int("gNB-CU-UP-UE-E1AP-ID", gnb_cu_up_ue_e1_ap_id);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t gnb_cu_cp_tnla_to_rem_item_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {75};
  return map_enum_number(options, 1, idx, "id");
}
bool gnb_cu_cp_tnla_to_rem_item_ext_ies_o::is_id_valid(const uint32_t& id)
{
  return 75 == id;
}
crit_e gnb_cu_cp_tnla_to_rem_item_ext_ies_o::get_crit(const uint32_t& id)
{
  if (id == 75) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_cu_cp_tnla_to_rem_item_ext_ies_o::ext_c gnb_cu_cp_tnla_to_rem_item_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  if (id != 75) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_tnla_to_rem_item_ext_ies_o::get_presence(const uint32_t& id)
{
  if (id == 75) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Extension ::= OPEN TYPE
void gnb_cu_cp_tnla_to_rem_item_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("CP-TNL-Information");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_tnla_to_rem_item_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnla_to_rem_item_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_tnla_to_rem_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* options[] = {"CP-TNL-Information"};
  return convert_enum_idx(options, 1, value, "gnb_cu_cp_tnla_to_rem_item_ext_ies_o::ext_c::types");
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

// PrivateIE-ID ::= CHOICE
void private_ie_id_c::set(types::options e)
{
  type_ = e;
}
uint32_t& private_ie_id_c::set_local()
{
  set(types::local);
  return c;
}
void private_ie_id_c::set_global()
{
  set(types::global);
}
void private_ie_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::local:
      j.write_int("local", c);
      break;
    case types::global:
      break;
    default:
      log_invalid_choice_id(type_, "private_ie_id_c");
  }
  j.end_obj();
}
SRSASN_CODE private_ie_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::local:
      HANDLE_CODE(pack_integer(bref, c, (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    case types::global:
      break;
    default:
      log_invalid_choice_id(type_, "private_ie_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE private_ie_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::local:
      HANDLE_CODE(unpack_integer(c, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    case types::global:
      break;
    default:
      log_invalid_choice_id(type_, "private_ie_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* private_ie_id_c::types_opts::to_string() const
{
  static const char* options[] = {"local", "global"};
  return convert_enum_idx(options, 2, value, "private_ie_id_c::types");
}

// QoS-Parameters-Support-List ::= SEQUENCE
SRSASN_CODE qo_s_params_support_list_s::pack(bit_ref& bref) const
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
SRSASN_CODE qo_s_params_support_list_s::unpack(cbit_ref& bref)
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
void qo_s_params_support_list_s::to_json(json_writer& j) const
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
const char* tnlassoc_usage_opts::to_string() const
{
  static const char* options[] = {"ue", "non-ue", "both"};
  return convert_enum_idx(options, 3, value, "tnlassoc_usage_e");
}

// UE-associatedLogicalE1-ConnectionItemRes ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ue_associated_lc_e1_conn_item_res_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {5};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"UE-associatedLogicalE1-ConnectionItem"};
  return convert_enum_idx(options, 1, value, "ue_associated_lc_e1_conn_item_res_o::value_c::types");
}
uint8_t ue_associated_lc_e1_conn_item_res_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ue_associated_lc_e1_conn_item_res_o::value_c::types");
}

// EUTRAN-GNB-CU-UP-CounterCheckRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_gnb_cu_up_counter_check_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {61};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"DRBs-Subject-To-Counter-Check-List-EUTRAN"};
  return convert_enum_idx(options, 1, value, "eutran_gnb_cu_up_counter_check_request_o::value_c::types");
}

// GNB-CU-CP-TNLA-Failed-To-Setup-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnla_failed_to_setup_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnla_failed_to_setup_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.unpack(bref));
  HANDLE_CODE(cause.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnla_failed_to_setup_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnlassoc_transport_layer_address.to_json(j);
  j.write_fieldname("cause");
  cause.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-Setup-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnla_setup_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnla_setup_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnla_setup_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnlassoc_transport_layer_address.to_json(j);
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-To-Add-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnla_to_add_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.pack(bref));
  HANDLE_CODE(tnlassoc_usage.pack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnla_to_add_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.unpack(bref));
  HANDLE_CODE(tnlassoc_usage.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnla_to_add_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnlassoc_transport_layer_address.to_json(j);
  j.write_str("tNLAssociationUsage", tnlassoc_usage.to_string());
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-To-Remove-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnla_to_rem_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ie_exts.size() > 0, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.pack(bref));
  if (ie_exts.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ie_exts, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnla_to_rem_item_s::unpack(cbit_ref& bref)
{
  bool ie_exts_present;
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.unpack(bref));
  if (ie_exts_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ie_exts, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnla_to_rem_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnlassoc_transport_layer_address.to_json(j);
  if (ie_exts.size() > 0) {
    j.write_fieldname("iE-Extensions");
  }
  j.end_obj();
}

// GNB-CU-CP-TNLA-To-Update-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_cp_tnla_to_upd_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tnlassoc_usage_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.pack(bref));
  if (tnlassoc_usage_present) {
    HANDLE_CODE(tnlassoc_usage.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_tnla_to_upd_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tnlassoc_usage_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.unpack(bref));
  if (tnlassoc_usage_present) {
    HANDLE_CODE(tnlassoc_usage.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_cp_tnla_to_upd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnlassoc_transport_layer_address.to_json(j);
  if (tnlassoc_usage_present) {
    j.write_str("tNLAssociationUsage", tnlassoc_usage.to_string());
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// GNB-CU-UP-TNLA-To-Remove-Item ::= SEQUENCE
SRSASN_CODE gnb_cu_up_tnla_to_rem_item_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tnlassoc_transport_layer_addressg_nbcucp_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.pack(bref));
  if (tnlassoc_transport_layer_addressg_nbcucp_present) {
    HANDLE_CODE(tnlassoc_transport_layer_addressg_nbcucp.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_tnla_to_rem_item_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tnlassoc_transport_layer_addressg_nbcucp_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(tnlassoc_transport_layer_address.unpack(bref));
  if (tnlassoc_transport_layer_addressg_nbcucp_present) {
    HANDLE_CODE(tnlassoc_transport_layer_addressg_nbcucp.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gnb_cu_up_tnla_to_rem_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tNLAssociationTransportLayerAddress");
  tnlassoc_transport_layer_address.to_json(j);
  if (tnlassoc_transport_layer_addressg_nbcucp_present) {
    j.write_fieldname("tNLAssociationTransportLayerAddressgNBCUCP");
    tnlassoc_transport_layer_addressg_nbcucp.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// NG-RAN-GNB-CU-UP-CounterCheckRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_gnb_cu_up_counter_check_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {62};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"DRBs-Subject-To-Counter-Check-List-NG-RAN"};
  return convert_enum_idx(options, 1, value, "ng_ran_gnb_cu_up_counter_check_request_o::value_c::types");
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

// PDU-Session-To-Notify-Item ::= SEQUENCE
SRSASN_CODE pdu_session_to_notify_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(pack_integer(bref, pdu_session_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, qo_s_flow_list, 1, 64, true));
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
  HANDLE_CODE(unpack_dyn_seq_of(qo_s_flow_list, bref, 1, 64, true));
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
  for (const auto& e1 : qo_s_flow_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// ResetAll ::= ENUMERATED
const char* reset_all_opts::to_string() const
{
  static const char* options[] = {"reset-all"};
  return convert_enum_idx(options, 1, value, "reset_all_e");
}

// SupportedPLMNs-Item ::= SEQUENCE
SRSASN_CODE supported_plmns_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slice_support_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(nr_cgi_support_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(qo_s_params_support_list_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(plmn_id.pack(bref));
  if (slice_support_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slice_support_list, 1, 1024, true));
  }
  if (nr_cgi_support_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nr_cgi_support_list, 1, 512, true));
  }
  if (qo_s_params_support_list_present) {
    HANDLE_CODE(qo_s_params_support_list.pack(bref));
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
  HANDLE_CODE(bref.unpack(qo_s_params_support_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(plmn_id.unpack(bref));
  if (slice_support_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slice_support_list, bref, 1, 1024, true));
  }
  if (nr_cgi_support_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nr_cgi_support_list, bref, 1, 512, true));
  }
  if (qo_s_params_support_list_present) {
    HANDLE_CODE(qo_s_params_support_list.unpack(bref));
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
  if (qo_s_params_support_list_present) {
    j.write_fieldname("qoS-Parameters-Support-List");
    qo_s_params_support_list.to_json(j);
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
  static const uint32_t options[] = {5};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"UE-associatedLogicalE1-ConnectionItem"};
  return convert_enum_idx(options, 1, value, "ue_associated_lc_e1_conn_item_res_ack_o::value_c::types");
}
uint8_t ue_associated_lc_e1_conn_item_res_ack_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ue_associated_lc_e1_conn_item_res_ack_o::value_c::types");
}

template struct asn1::protocol_ie_single_container_s<ue_associated_lc_e1_conn_item_res_o>;

// CNSupport ::= ENUMERATED
const char* cn_support_opts::to_string() const
{
  static const char* options[] = {"c-epc", "c-5gc", "both"};
  return convert_enum_idx(options, 3, value, "cn_support_e");
}
int8_t cn_support_opts::to_number() const
{
  if (value == c_minus5gc) {
    return -5;
  }
  invalid_enum_number(value, "cn_support_e");
  return 0;
}

// GNB-CU-UP-OverloadInformation ::= ENUMERATED
const char* gnb_cu_up_overload_info_opts::to_string() const
{
  static const char* options[] = {"overloaded", "not-overloaded"};
  return convert_enum_idx(options, 2, value, "gnb_cu_up_overload_info_e");
}

// PrivateIE-Field{E1AP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE{{E1AP-PRIVATE-IES}}
template <class ies_set_paramT_>
SRSASN_CODE private_ie_field_s<ies_set_paramT_>::pack(bit_ref& bref) const
{
  HANDLE_CODE(id.pack(bref));
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
SRSASN_CODE private_ie_field_s<ies_set_paramT_>::unpack(cbit_ref& bref)
{
  HANDLE_CODE(id.unpack(bref));
  HANDLE_CODE(crit.unpack(bref));
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
void private_ie_field_s<ies_set_paramT_>::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("id");
  id.to_json(j);
  j.write_str("criticality", crit.to_string());
  j.end_obj();
}

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
  static const char* options[] = {"e1-Interface", "partOfE1-Interface", "choice-extension"};
  return convert_enum_idx(options, 3, value, "reset_type_c::types");
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
  static const char* options[] = {
      "e-UTRAN-GNB-CU-UP-CounterCheckRequest", "nG-RAN-GNB-CU-UP-CounterCheckRequest", "choice-extension"};
  return convert_enum_idx(options, 3, value, "sys_gnb_cu_up_counter_check_request_c::types");
}

// TimeToWait ::= ENUMERATED
const char* time_to_wait_opts::to_string() const
{
  static const char* options[] = {"v1s", "v2s", "v5s", "v10s", "v20s", "v60s"};
  return convert_enum_idx(options, 6, value, "time_to_wait_e");
}
uint8_t time_to_wait_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 5, 10, 20, 60};
  return map_enum_number(options, 6, value, "time_to_wait_e");
}

template struct asn1::protocol_ie_single_container_s<ue_associated_lc_e1_conn_item_res_ack_o>;

// E1ReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t e1_release_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 0};
  return map_enum_number(options, 2, idx, "id");
}
bool e1_release_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 0};
  for (const auto& o : options) {
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
void e1_release_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    default:
      break;
  }
}
void e1_release_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_release_request_ies_o::value_c");
  }
}
e1_release_request_ies_o::value_c::value_c(const e1_release_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_release_request_ies_o::value_c");
  }
}
e1_release_request_ies_o::value_c&
e1_release_request_ies_o::value_c::operator=(const e1_release_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_release_request_ies_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"INTEGER (0..255,...)", "Cause"};
  return convert_enum_idx(options, 2, value, "e1_release_request_ies_o::value_c::types");
}
uint8_t e1_release_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "e1_release_request_ies_o::value_c::types");
}

// E1ReleaseResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t e1_release_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57};
  return map_enum_number(options, 1, idx, "id");
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
  static const char* options[] = {"INTEGER (0..255,...)"};
  return convert_enum_idx(options, 1, value, "e1_release_resp_ies_o::value_c::types");
}
uint8_t e1_release_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "e1_release_resp_ies_o::value_c::types");
}

// ErrorIndication-IEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t error_ind_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 2, 3, 0, 1};
  return map_enum_number(options, 5, idx, "id");
}
bool error_ind_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 2, 3, 0, 1};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void error_ind_ies_o::value_c::destroy_()
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
void error_ind_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }
}
error_ind_ies_o::value_c::value_c(const error_ind_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }
}
error_ind_ies_o::value_c& error_ind_ies_o::value_c::operator=(const error_ind_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }

  return *this;
}
uint16_t& error_ind_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint64_t& error_ind_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& error_ind_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
const uint64_t& error_ind_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& error_ind_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..255,...)", "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 5, value, "error_ind_ies_o::value_c::types");
}

// GNB-CU-CP-ConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_cfg_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 1, 30, 31};
  return map_enum_number(options, 4, idx, "id");
}
bool gnb_cu_cp_cfg_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 1, 30, 31};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_tnla_setup_list);
      break;
    case 31:
      ret.set(value_c::types::gnb_cu_cp_tnla_failed_to_setup_list);
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_cfg_upd_ack_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    case types::gnb_cu_cp_tnla_setup_list:
      c.destroy<gnb_cu_cp_tnla_setup_list_l>();
      break;
    case types::gnb_cu_cp_tnla_failed_to_setup_list:
      c.destroy<gnb_cu_cp_tnla_failed_to_setup_list_l>();
      break;
    default:
      break;
  }
}
void gnb_cu_cp_cfg_upd_ack_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::gnb_cu_cp_tnla_setup_list:
      c.init<gnb_cu_cp_tnla_setup_list_l>();
      break;
    case types::gnb_cu_cp_tnla_failed_to_setup_list:
      c.init<gnb_cu_cp_tnla_failed_to_setup_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c");
  }
}
gnb_cu_cp_cfg_upd_ack_ies_o::value_c::value_c(const gnb_cu_cp_cfg_upd_ack_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::gnb_cu_cp_tnla_setup_list:
      c.init(other.c.get<gnb_cu_cp_tnla_setup_list_l>());
      break;
    case types::gnb_cu_cp_tnla_failed_to_setup_list:
      c.init(other.c.get<gnb_cu_cp_tnla_failed_to_setup_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c");
  }
}
gnb_cu_cp_cfg_upd_ack_ies_o::value_c&
gnb_cu_cp_cfg_upd_ack_ies_o::value_c::operator=(const gnb_cu_cp_cfg_upd_ack_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::gnb_cu_cp_tnla_setup_list:
      c.set(other.c.get<gnb_cu_cp_tnla_setup_list_l>());
      break;
    case types::gnb_cu_cp_tnla_failed_to_setup_list:
      c.set(other.c.get<gnb_cu_cp_tnla_failed_to_setup_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c");
  }

  return *this;
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
gnb_cu_cp_tnla_setup_list_l& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::gnb_cu_cp_tnla_setup_list()
{
  assert_choice_type(types::gnb_cu_cp_tnla_setup_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_setup_list_l>();
}
gnb_cu_cp_tnla_failed_to_setup_list_l& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::gnb_cu_cp_tnla_failed_to_setup_list()
{
  assert_choice_type(types::gnb_cu_cp_tnla_failed_to_setup_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_failed_to_setup_list_l>();
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
const gnb_cu_cp_tnla_setup_list_l& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::gnb_cu_cp_tnla_setup_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnla_setup_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_setup_list_l>();
}
const gnb_cu_cp_tnla_failed_to_setup_list_l&
gnb_cu_cp_cfg_upd_ack_ies_o::value_c::gnb_cu_cp_tnla_failed_to_setup_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnla_failed_to_setup_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_failed_to_setup_list_l>();
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
    case types::gnb_cu_cp_tnla_setup_list:
      j.start_array("GNB-CU-CP-TNLA-Setup-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnla_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_cp_tnla_failed_to_setup_list:
      j.start_array("GNB-CU-CP-TNLA-Failed-To-Setup-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnla_failed_to_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
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
    case types::gnb_cu_cp_tnla_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnla_setup_list_l>(), 1, 32, true));
      break;
    case types::gnb_cu_cp_tnla_failed_to_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnla_failed_to_setup_list_l>(), 1, 32, true));
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
    case types::gnb_cu_cp_tnla_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnla_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::gnb_cu_cp_tnla_failed_to_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnla_failed_to_setup_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)",
                                  "CriticalityDiagnostics",
                                  "GNB-CU-CP-TNLA-Setup-List",
                                  "GNB-CU-CP-TNLA-Failed-To-Setup-List"};
  return convert_enum_idx(options, 4, value, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_cfg_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c::types");
}

// GNB-CU-CP-ConfigurationUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_cfg_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 0, 12, 1};
  return map_enum_number(options, 4, idx, "id");
}
bool gnb_cu_cp_cfg_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 0, 12, 1};
  for (const auto& o : options) {
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
void gnb_cu_cp_cfg_upd_fail_ies_o::value_c::destroy_()
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
void gnb_cu_cp_cfg_upd_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::time_to_wait:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c");
  }
}
gnb_cu_cp_cfg_upd_fail_ies_o::value_c::value_c(const gnb_cu_cp_cfg_upd_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.init(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c");
  }
}
gnb_cu_cp_cfg_upd_fail_ies_o::value_c&
gnb_cu_cp_cfg_upd_fail_ies_o::value_c::operator=(const gnb_cu_cp_cfg_upd_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.set(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c::types");
}

// GNB-CU-CP-ConfigurationUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_cfg_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 9, 27, 28, 29};
  return map_enum_number(options, 5, idx, "id");
}
bool gnb_cu_cp_cfg_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 9, 27, 28, 29};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_tnla_to_add_list);
      break;
    case 28:
      ret.set(value_c::types::gnb_cu_cp_tnla_to_rem_list);
      break;
    case 29:
      ret.set(value_c::types::gnb_cu_cp_tnla_to_upd_list);
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_cfg_upd_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::gnb_cu_cp_name:
      c.destroy<printable_string<1, 150, true, true>>();
      break;
    case types::gnb_cu_cp_tnla_to_add_list:
      c.destroy<gnb_cu_cp_tnla_to_add_list_l>();
      break;
    case types::gnb_cu_cp_tnla_to_rem_list:
      c.destroy<gnb_cu_cp_tnla_to_rem_list_l>();
      break;
    case types::gnb_cu_cp_tnla_to_upd_list:
      c.destroy<gnb_cu_cp_tnla_to_upd_list_l>();
      break;
    default:
      break;
  }
}
void gnb_cu_cp_cfg_upd_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::gnb_cu_cp_name:
      c.init<printable_string<1, 150, true, true>>();
      break;
    case types::gnb_cu_cp_tnla_to_add_list:
      c.init<gnb_cu_cp_tnla_to_add_list_l>();
      break;
    case types::gnb_cu_cp_tnla_to_rem_list:
      c.init<gnb_cu_cp_tnla_to_rem_list_l>();
      break;
    case types::gnb_cu_cp_tnla_to_upd_list:
      c.init<gnb_cu_cp_tnla_to_upd_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ies_o::value_c");
  }
}
gnb_cu_cp_cfg_upd_ies_o::value_c::value_c(const gnb_cu_cp_cfg_upd_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      c.init(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::gnb_cu_cp_tnla_to_add_list:
      c.init(other.c.get<gnb_cu_cp_tnla_to_add_list_l>());
      break;
    case types::gnb_cu_cp_tnla_to_rem_list:
      c.init(other.c.get<gnb_cu_cp_tnla_to_rem_list_l>());
      break;
    case types::gnb_cu_cp_tnla_to_upd_list:
      c.init(other.c.get<gnb_cu_cp_tnla_to_upd_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ies_o::value_c");
  }
}
gnb_cu_cp_cfg_upd_ies_o::value_c&
gnb_cu_cp_cfg_upd_ies_o::value_c::operator=(const gnb_cu_cp_cfg_upd_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      c.set(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::gnb_cu_cp_tnla_to_add_list:
      c.set(other.c.get<gnb_cu_cp_tnla_to_add_list_l>());
      break;
    case types::gnb_cu_cp_tnla_to_rem_list:
      c.set(other.c.get<gnb_cu_cp_tnla_to_rem_list_l>());
      break;
    case types::gnb_cu_cp_tnla_to_upd_list:
      c.set(other.c.get<gnb_cu_cp_tnla_to_upd_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ies_o::value_c");
  }

  return *this;
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
gnb_cu_cp_tnla_to_add_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnla_to_add_list()
{
  assert_choice_type(types::gnb_cu_cp_tnla_to_add_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_to_add_list_l>();
}
gnb_cu_cp_tnla_to_rem_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnla_to_rem_list()
{
  assert_choice_type(types::gnb_cu_cp_tnla_to_rem_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_to_rem_list_l>();
}
gnb_cu_cp_tnla_to_upd_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnla_to_upd_list()
{
  assert_choice_type(types::gnb_cu_cp_tnla_to_upd_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_to_upd_list_l>();
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
const gnb_cu_cp_tnla_to_add_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnla_to_add_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnla_to_add_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_to_add_list_l>();
}
const gnb_cu_cp_tnla_to_rem_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnla_to_rem_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnla_to_rem_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_to_rem_list_l>();
}
const gnb_cu_cp_tnla_to_upd_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnla_to_upd_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnla_to_upd_list, type_, "Value");
  return c.get<gnb_cu_cp_tnla_to_upd_list_l>();
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
    case types::gnb_cu_cp_tnla_to_add_list:
      j.start_array("GNB-CU-CP-TNLA-To-Add-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnla_to_add_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_cp_tnla_to_rem_list:
      j.start_array("GNB-CU-CP-TNLA-To-Remove-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnla_to_rem_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_cp_tnla_to_upd_list:
      j.start_array("GNB-CU-CP-TNLA-To-Update-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnla_to_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
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
    case types::gnb_cu_cp_tnla_to_add_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnla_to_add_list_l>(), 1, 32, true));
      break;
    case types::gnb_cu_cp_tnla_to_rem_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnla_to_rem_list_l>(), 1, 32, true));
      break;
    case types::gnb_cu_cp_tnla_to_upd_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnla_to_upd_list_l>(), 1, 32, true));
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
    case types::gnb_cu_cp_tnla_to_add_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnla_to_add_list_l>(), bref, 1, 32, true));
      break;
    case types::gnb_cu_cp_tnla_to_rem_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnla_to_rem_list_l>(), bref, 1, 32, true));
      break;
    case types::gnb_cu_cp_tnla_to_upd_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnla_to_upd_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)",
                                  "PrintableString",
                                  "GNB-CU-CP-TNLA-To-Add-List",
                                  "GNB-CU-CP-TNLA-To-Remove-List",
                                  "GNB-CU-CP-TNLA-To-Update-List"};
  return convert_enum_idx(options, 5, value, "gnb_cu_cp_cfg_upd_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_cfg_upd_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_cp_cfg_upd_ies_o::value_c::types");
}

// GNB-CU-CP-E1SetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_e1_setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 0, 12, 1};
  return map_enum_number(options, 4, idx, "id");
}
bool gnb_cu_cp_e1_setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 0, 12, 1};
  for (const auto& o : options) {
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
void gnb_cu_cp_e1_setup_fail_ies_o::value_c::destroy_()
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
void gnb_cu_cp_e1_setup_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::time_to_wait:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_fail_ies_o::value_c");
  }
}
gnb_cu_cp_e1_setup_fail_ies_o::value_c::value_c(const gnb_cu_cp_e1_setup_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.init(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_fail_ies_o::value_c");
  }
}
gnb_cu_cp_e1_setup_fail_ies_o::value_c&
gnb_cu_cp_e1_setup_fail_ies_o::value_c::operator=(const gnb_cu_cp_e1_setup_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.set(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_fail_ies_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "gnb_cu_cp_e1_setup_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_e1_setup_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_cp_e1_setup_fail_ies_o::value_c::types");
}

// GNB-CU-CP-E1SetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_e1_setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 9};
  return map_enum_number(options, 2, idx, "id");
}
bool gnb_cu_cp_e1_setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 9};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_e1_setup_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::gnb_cu_cp_name:
      c.destroy<printable_string<1, 150, true, true>>();
      break;
    default:
      break;
  }
}
void gnb_cu_cp_e1_setup_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::gnb_cu_cp_name:
      c.init<printable_string<1, 150, true, true>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_request_ies_o::value_c");
  }
}
gnb_cu_cp_e1_setup_request_ies_o::value_c::value_c(const gnb_cu_cp_e1_setup_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      c.init(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_request_ies_o::value_c");
  }
}
gnb_cu_cp_e1_setup_request_ies_o::value_c&
gnb_cu_cp_e1_setup_request_ies_o::value_c::operator=(const gnb_cu_cp_e1_setup_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      c.set(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_request_ies_o::value_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_e1_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "PrintableString"};
  return convert_enum_idx(options, 2, value, "gnb_cu_cp_e1_setup_request_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_e1_setup_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_cp_e1_setup_request_ies_o::value_c::types");
}

// GNB-CU-CP-E1SetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_e1_setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 7, 8, 10, 11, 64};
  return map_enum_number(options, 6, idx, "id");
}
bool gnb_cu_cp_e1_setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 7, 8, 10, 11, 64};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_e1_setup_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::gnb_cu_up_name:
      c.destroy<printable_string<1, 150, true, true>>();
      break;
    case types::supported_plmns:
      c.destroy<supported_plmns_list_l>();
      break;
    default:
      break;
  }
}
void gnb_cu_cp_e1_setup_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::gnb_cu_up_id:
      break;
    case types::gnb_cu_up_name:
      c.init<printable_string<1, 150, true, true>>();
      break;
    case types::cn_support:
      break;
    case types::supported_plmns:
      c.init<supported_plmns_list_l>();
      break;
    case types::gnb_cu_up_capacity:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_resp_ies_o::value_c");
  }
}
gnb_cu_cp_e1_setup_resp_ies_o::value_c::value_c(const gnb_cu_cp_e1_setup_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      c.init(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::cn_support:
      c.init(other.c.get<cn_support_e>());
      break;
    case types::supported_plmns:
      c.init(other.c.get<supported_plmns_list_l>());
      break;
    case types::gnb_cu_up_capacity:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_resp_ies_o::value_c");
  }
}
gnb_cu_cp_e1_setup_resp_ies_o::value_c&
gnb_cu_cp_e1_setup_resp_ies_o::value_c::operator=(const gnb_cu_cp_e1_setup_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      c.set(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::cn_support:
      c.set(other.c.get<cn_support_e>());
      break;
    case types::supported_plmns:
      c.set(other.c.get<supported_plmns_list_l>());
      break;
    case types::gnb_cu_up_capacity:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_resp_ies_o::value_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_e1_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)",
                                  "INTEGER (0..68719476735)",
                                  "PrintableString",
                                  "CNSupport",
                                  "SupportedPLMNs-List",
                                  "INTEGER (0..255)"};
  return convert_enum_idx(options, 6, value, "gnb_cu_cp_e1_setup_resp_ies_o::value_c::types");
}

// GNB-CU-UP-ConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_cfg_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 1};
  return map_enum_number(options, 2, idx, "id");
}
bool gnb_cu_up_cfg_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 1};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_cfg_upd_ack_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void gnb_cu_up_cfg_upd_ack_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ack_ies_o::value_c");
  }
}
gnb_cu_up_cfg_upd_ack_ies_o::value_c::value_c(const gnb_cu_up_cfg_upd_ack_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ack_ies_o::value_c");
  }
}
gnb_cu_up_cfg_upd_ack_ies_o::value_c&
gnb_cu_up_cfg_upd_ack_ies_o::value_c::operator=(const gnb_cu_up_cfg_upd_ack_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ack_ies_o::value_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 2, value, "gnb_cu_up_cfg_upd_ack_ies_o::value_c::types");
}
uint8_t gnb_cu_up_cfg_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_up_cfg_upd_ack_ies_o::value_c::types");
}

// GNB-CU-UP-ConfigurationUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_cfg_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 0, 12, 1};
  return map_enum_number(options, 4, idx, "id");
}
bool gnb_cu_up_cfg_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 0, 12, 1};
  for (const auto& o : options) {
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
void gnb_cu_up_cfg_upd_fail_ies_o::value_c::destroy_()
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
void gnb_cu_up_cfg_upd_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::time_to_wait:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_fail_ies_o::value_c");
  }
}
gnb_cu_up_cfg_upd_fail_ies_o::value_c::value_c(const gnb_cu_up_cfg_upd_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.init(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_fail_ies_o::value_c");
  }
}
gnb_cu_up_cfg_upd_fail_ies_o::value_c&
gnb_cu_up_cfg_upd_fail_ies_o::value_c::operator=(const gnb_cu_up_cfg_upd_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.set(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_fail_ies_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "gnb_cu_up_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_up_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_up_cfg_upd_fail_ies_o::value_c::types");
}

// GNB-CU-UP-ConfigurationUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_cfg_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 7, 8, 11, 64, 73};
  return map_enum_number(options, 6, idx, "id");
}
bool gnb_cu_up_cfg_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 7, 8, 11, 64, 73};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_up_tnla_to_rem_list);
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_cfg_upd_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::gnb_cu_up_name:
      c.destroy<printable_string<1, 150, true, true>>();
      break;
    case types::supported_plmns:
      c.destroy<supported_plmns_list_l>();
      break;
    case types::gnb_cu_up_tnla_to_rem_list:
      c.destroy<gnb_cu_up_tnla_to_rem_list_l>();
      break;
    default:
      break;
  }
}
void gnb_cu_up_cfg_upd_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::gnb_cu_up_id:
      break;
    case types::gnb_cu_up_name:
      c.init<printable_string<1, 150, true, true>>();
      break;
    case types::supported_plmns:
      c.init<supported_plmns_list_l>();
      break;
    case types::gnb_cu_up_capacity:
      break;
    case types::gnb_cu_up_tnla_to_rem_list:
      c.init<gnb_cu_up_tnla_to_rem_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ies_o::value_c");
  }
}
gnb_cu_up_cfg_upd_ies_o::value_c::value_c(const gnb_cu_up_cfg_upd_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      c.init(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::supported_plmns:
      c.init(other.c.get<supported_plmns_list_l>());
      break;
    case types::gnb_cu_up_capacity:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_tnla_to_rem_list:
      c.init(other.c.get<gnb_cu_up_tnla_to_rem_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ies_o::value_c");
  }
}
gnb_cu_up_cfg_upd_ies_o::value_c&
gnb_cu_up_cfg_upd_ies_o::value_c::operator=(const gnb_cu_up_cfg_upd_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      c.set(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::supported_plmns:
      c.set(other.c.get<supported_plmns_list_l>());
      break;
    case types::gnb_cu_up_capacity:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_tnla_to_rem_list:
      c.set(other.c.get<gnb_cu_up_tnla_to_rem_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ies_o::value_c");
  }

  return *this;
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
gnb_cu_up_tnla_to_rem_list_l& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_tnla_to_rem_list()
{
  assert_choice_type(types::gnb_cu_up_tnla_to_rem_list, type_, "Value");
  return c.get<gnb_cu_up_tnla_to_rem_list_l>();
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
const gnb_cu_up_tnla_to_rem_list_l& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_tnla_to_rem_list() const
{
  assert_choice_type(types::gnb_cu_up_tnla_to_rem_list, type_, "Value");
  return c.get<gnb_cu_up_tnla_to_rem_list_l>();
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
    case types::gnb_cu_up_tnla_to_rem_list:
      j.start_array("GNB-CU-UP-TNLA-To-Remove-List");
      for (const auto& e1 : c.get<gnb_cu_up_tnla_to_rem_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
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
    case types::gnb_cu_up_tnla_to_rem_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_up_tnla_to_rem_list_l>(), 1, 32, true));
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
    case types::gnb_cu_up_tnla_to_rem_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_up_tnla_to_rem_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)",
                                  "INTEGER (0..68719476735)",
                                  "PrintableString",
                                  "SupportedPLMNs-List",
                                  "INTEGER (0..255)",
                                  "GNB-CU-UP-TNLA-To-Remove-List"};
  return convert_enum_idx(options, 6, value, "gnb_cu_up_cfg_upd_ies_o::value_c::types");
}

// GNB-CU-UP-CounterCheckRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_counter_check_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 60};
  return map_enum_number(options, 3, idx, "id");
}
bool gnb_cu_up_counter_check_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 60};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void gnb_cu_up_counter_check_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::sys_gnb_cu_up_counter_check_request:
      c.destroy<sys_gnb_cu_up_counter_check_request_c>();
      break;
    default:
      break;
  }
}
void gnb_cu_up_counter_check_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::sys_gnb_cu_up_counter_check_request:
      c.init<sys_gnb_cu_up_counter_check_request_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_counter_check_request_ies_o::value_c");
  }
}
gnb_cu_up_counter_check_request_ies_o::value_c::value_c(const gnb_cu_up_counter_check_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sys_gnb_cu_up_counter_check_request:
      c.init(other.c.get<sys_gnb_cu_up_counter_check_request_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_counter_check_request_ies_o::value_c");
  }
}
gnb_cu_up_counter_check_request_ies_o::value_c&
gnb_cu_up_counter_check_request_ies_o::value_c::operator=(const gnb_cu_up_counter_check_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sys_gnb_cu_up_counter_check_request:
      c.set(other.c.get<sys_gnb_cu_up_counter_check_request_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_counter_check_request_ies_o::value_c");
  }

  return *this;
}
uint64_t& gnb_cu_up_counter_check_request_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& gnb_cu_up_counter_check_request_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_gnb_cu_up_counter_check_request_c&
gnb_cu_up_counter_check_request_ies_o::value_c::sys_gnb_cu_up_counter_check_request()
{
  assert_choice_type(types::sys_gnb_cu_up_counter_check_request, type_, "Value");
  return c.get<sys_gnb_cu_up_counter_check_request_c>();
}
const uint64_t& gnb_cu_up_counter_check_request_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& gnb_cu_up_counter_check_request_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-GNB-CU-UP-CounterCheckRequest"};
  return convert_enum_idx(options, 3, value, "gnb_cu_up_counter_check_request_ies_o::value_c::types");
}

// GNB-CU-UP-E1SetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_e1_setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 0, 12, 1};
  return map_enum_number(options, 4, idx, "id");
}
bool gnb_cu_up_e1_setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 0, 12, 1};
  for (const auto& o : options) {
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
void gnb_cu_up_e1_setup_fail_ies_o::value_c::destroy_()
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
void gnb_cu_up_e1_setup_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::time_to_wait:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_fail_ies_o::value_c");
  }
}
gnb_cu_up_e1_setup_fail_ies_o::value_c::value_c(const gnb_cu_up_e1_setup_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.init(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_fail_ies_o::value_c");
  }
}
gnb_cu_up_e1_setup_fail_ies_o::value_c&
gnb_cu_up_e1_setup_fail_ies_o::value_c::operator=(const gnb_cu_up_e1_setup_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.set(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_fail_ies_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "gnb_cu_up_e1_setup_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_up_e1_setup_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_up_e1_setup_fail_ies_o::value_c::types");
}

// GNB-CU-UP-E1SetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_e1_setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 7, 8, 10, 11, 64};
  return map_enum_number(options, 6, idx, "id");
}
bool gnb_cu_up_e1_setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 7, 8, 10, 11, 64};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_e1_setup_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::gnb_cu_up_name:
      c.destroy<printable_string<1, 150, true, true>>();
      break;
    case types::supported_plmns:
      c.destroy<supported_plmns_list_l>();
      break;
    default:
      break;
  }
}
void gnb_cu_up_e1_setup_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::gnb_cu_up_id:
      break;
    case types::gnb_cu_up_name:
      c.init<printable_string<1, 150, true, true>>();
      break;
    case types::cn_support:
      break;
    case types::supported_plmns:
      c.init<supported_plmns_list_l>();
      break;
    case types::gnb_cu_up_capacity:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_request_ies_o::value_c");
  }
}
gnb_cu_up_e1_setup_request_ies_o::value_c::value_c(const gnb_cu_up_e1_setup_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      c.init(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::cn_support:
      c.init(other.c.get<cn_support_e>());
      break;
    case types::supported_plmns:
      c.init(other.c.get<supported_plmns_list_l>());
      break;
    case types::gnb_cu_up_capacity:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_request_ies_o::value_c");
  }
}
gnb_cu_up_e1_setup_request_ies_o::value_c&
gnb_cu_up_e1_setup_request_ies_o::value_c::operator=(const gnb_cu_up_e1_setup_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      c.set(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::cn_support:
      c.set(other.c.get<cn_support_e>());
      break;
    case types::supported_plmns:
      c.set(other.c.get<supported_plmns_list_l>());
      break;
    case types::gnb_cu_up_capacity:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_request_ies_o::value_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_e1_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)",
                                  "INTEGER (0..68719476735)",
                                  "PrintableString",
                                  "CNSupport",
                                  "SupportedPLMNs-List",
                                  "INTEGER (0..255)"};
  return convert_enum_idx(options, 6, value, "gnb_cu_up_e1_setup_request_ies_o::value_c::types");
}

// GNB-CU-UP-E1SetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_e1_setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 9};
  return map_enum_number(options, 2, idx, "id");
}
bool gnb_cu_up_e1_setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 9};
  for (const auto& o : options) {
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
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_e1_setup_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::gnb_cu_cp_name:
      c.destroy<printable_string<1, 150, true, true>>();
      break;
    default:
      break;
  }
}
void gnb_cu_up_e1_setup_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::gnb_cu_cp_name:
      c.init<printable_string<1, 150, true, true>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_resp_ies_o::value_c");
  }
}
gnb_cu_up_e1_setup_resp_ies_o::value_c::value_c(const gnb_cu_up_e1_setup_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      c.init(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_resp_ies_o::value_c");
  }
}
gnb_cu_up_e1_setup_resp_ies_o::value_c&
gnb_cu_up_e1_setup_resp_ies_o::value_c::operator=(const gnb_cu_up_e1_setup_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      c.set(other.c.get<printable_string<1, 150, true, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_resp_ies_o::value_c");
  }

  return *this;
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
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_e1_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "PrintableString"};
  return convert_enum_idx(options, 2, value, "gnb_cu_up_e1_setup_resp_ies_o::value_c::types");
}
uint8_t gnb_cu_up_e1_setup_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_up_e1_setup_resp_ies_o::value_c::types");
}

// GNB-CU-UP-StatusIndicationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_status_ind_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 65};
  return map_enum_number(options, 2, idx, "id");
}
bool gnb_cu_up_status_ind_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 65};
  for (const auto& o : options) {
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
void gnb_cu_up_status_ind_ies_o::value_c::destroy_() {}
void gnb_cu_up_status_ind_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
gnb_cu_up_status_ind_ies_o::value_c::value_c(const gnb_cu_up_status_ind_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_overload_info:
      c.init(other.c.get<gnb_cu_up_overload_info_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_status_ind_ies_o::value_c");
  }
}
gnb_cu_up_status_ind_ies_o::value_c&
gnb_cu_up_status_ind_ies_o::value_c::operator=(const gnb_cu_up_status_ind_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::gnb_cu_up_overload_info:
      c.set(other.c.get<gnb_cu_up_overload_info_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_status_ind_ies_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"INTEGER (0..255,...)", "GNB-CU-UP-OverloadInformation"};
  return convert_enum_idx(options, 2, value, "gnb_cu_up_status_ind_ies_o::value_c::types");
}
uint8_t gnb_cu_up_status_ind_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "gnb_cu_up_status_ind_ies_o::value_c::types");
}

// MRDC-DataUsageReportIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t mrdc_data_usage_report_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 68};
  return map_enum_number(options, 3, idx, "id");
}
bool mrdc_data_usage_report_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 68};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void mrdc_data_usage_report_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::pdu_session_res_data_usage_list:
      c.destroy<pdu_session_res_data_usage_list_l>();
      break;
    default:
      break;
  }
}
void mrdc_data_usage_report_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::pdu_session_res_data_usage_list:
      c.init<pdu_session_res_data_usage_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_data_usage_report_ies_o::value_c");
  }
}
mrdc_data_usage_report_ies_o::value_c::value_c(const mrdc_data_usage_report_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::pdu_session_res_data_usage_list:
      c.init(other.c.get<pdu_session_res_data_usage_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_data_usage_report_ies_o::value_c");
  }
}
mrdc_data_usage_report_ies_o::value_c&
mrdc_data_usage_report_ies_o::value_c::operator=(const mrdc_data_usage_report_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::pdu_session_res_data_usage_list:
      c.set(other.c.get<pdu_session_res_data_usage_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_data_usage_report_ies_o::value_c");
  }

  return *this;
}
uint64_t& mrdc_data_usage_report_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& mrdc_data_usage_report_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
pdu_session_res_data_usage_list_l& mrdc_data_usage_report_ies_o::value_c::pdu_session_res_data_usage_list()
{
  assert_choice_type(types::pdu_session_res_data_usage_list, type_, "Value");
  return c.get<pdu_session_res_data_usage_list_l>();
}
const uint64_t& mrdc_data_usage_report_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& mrdc_data_usage_report_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "PDU-Session-Resource-Data-Usage-List"};
  return convert_enum_idx(options, 3, value, "mrdc_data_usage_report_ies_o::value_c::types");
}

// Value ::= OPEN TYPE
void e1_ap_private_ies_empty_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE e1_ap_private_ies_empty_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1_ap_private_ies_empty_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}

const char* e1_ap_private_ies_empty_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {};
  return convert_enum_idx(options, 0, value, "e1_ap_private_ies_empty_o::value_c::types");
}

// ResetAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t reset_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 6, 1};
  return map_enum_number(options, 3, idx, "id");
}
bool reset_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 6, 1};
  for (const auto& o : options) {
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
void reset_ack_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ue_associated_lc_e1_conn_list_res_ack:
      c.destroy<ue_associated_lc_e1_conn_list_res_ack_l>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void reset_ack_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::ue_associated_lc_e1_conn_list_res_ack:
      c.init<ue_associated_lc_e1_conn_list_res_ack_l>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_ack_ies_o::value_c");
  }
}
reset_ack_ies_o::value_c::value_c(const reset_ack_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ue_associated_lc_e1_conn_list_res_ack:
      c.init(other.c.get<ue_associated_lc_e1_conn_list_res_ack_l>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_ack_ies_o::value_c");
  }
}
reset_ack_ies_o::value_c& reset_ack_ies_o::value_c::operator=(const reset_ack_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ue_associated_lc_e1_conn_list_res_ack:
      c.set(other.c.get<ue_associated_lc_e1_conn_list_res_ack_l>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_ack_ies_o::value_c");
  }

  return *this;
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
  static const char* options[] = {
      "INTEGER (0..255,...)", "UE-associatedLogicalE1-ConnectionListResAck", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 3, value, "reset_ack_ies_o::value_c::types");
}
uint8_t reset_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "reset_ack_ies_o::value_c::types");
}

// ResetIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t reset_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57, 0, 4};
  return map_enum_number(options, 3, idx, "id");
}
bool reset_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {57, 0, 4};
  for (const auto& o : options) {
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
void reset_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::reset_type:
      c.destroy<reset_type_c>();
      break;
    default:
      break;
  }
}
void reset_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::reset_type:
      c.init<reset_type_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_ies_o::value_c");
  }
}
reset_ies_o::value_c::value_c(const reset_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::reset_type:
      c.init(other.c.get<reset_type_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_ies_o::value_c");
  }
}
reset_ies_o::value_c& reset_ies_o::value_c::operator=(const reset_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::reset_type:
      c.set(other.c.get<reset_type_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_ies_o::value_c");
  }

  return *this;
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
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "ResetType"};
  return convert_enum_idx(options, 3, value, "reset_ies_o::value_c::types");
}
uint8_t reset_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "reset_ies_o::value_c::types");
}

// ULDataNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ul_data_notif_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {2, 3, 67};
  return map_enum_number(options, 3, idx, "id");
}
bool ul_data_notif_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {2, 3, 67};
  for (const auto& o : options) {
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
      ret.set(value_c::types::gnb_cu_cp_ue_e1_ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1_ap_id);
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
void ul_data_notif_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::pdu_session_to_notify_list:
      c.destroy<pdu_session_to_notify_list_l>();
      break;
    default:
      break;
  }
}
void ul_data_notif_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      break;
    case types::pdu_session_to_notify_list:
      c.init<pdu_session_to_notify_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_notif_ies_o::value_c");
  }
}
ul_data_notif_ies_o::value_c::value_c(const ul_data_notif_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::pdu_session_to_notify_list:
      c.init(other.c.get<pdu_session_to_notify_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_notif_ies_o::value_c");
  }
}
ul_data_notif_ies_o::value_c& ul_data_notif_ies_o::value_c::operator=(const ul_data_notif_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_cp_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::pdu_session_to_notify_list:
      c.set(other.c.get<pdu_session_to_notify_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_notif_ies_o::value_c");
  }

  return *this;
}
uint64_t& ul_data_notif_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ul_data_notif_ies_o::value_c::gnb_cu_up_ue_e1_ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
pdu_session_to_notify_list_l& ul_data_notif_ies_o::value_c::pdu_session_to_notify_list()
{
  assert_choice_type(types::pdu_session_to_notify_list, type_, "Value");
  return c.get<pdu_session_to_notify_list_l>();
}
const uint64_t& ul_data_notif_ies_o::value_c::gnb_cu_cp_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1_ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ul_data_notif_ies_o::value_c::gnb_cu_up_ue_e1_ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1_ap_id, type_, "Value");
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
    case types::gnb_cu_cp_ue_e1_ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1_ap_id:
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
  static const char* options[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "PDU-Session-To-Notify-List"};
  return convert_enum_idx(options, 3, value, "ul_data_notif_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e1_release_request_ies_o>;

e1_release_request_ies_container::e1_release_request_ies_container() :
  transaction_id(57, crit_e::reject), cause(0, crit_e::ignore)
{
}
SRSASN_CODE e1_release_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));

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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<error_ind_ies_o>;

error_ind_ies_container::error_ind_ies_container() :
  transaction_id(57, crit_e::reject),
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::ignore),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::ignore),
  cause(0, crit_e::ignore),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE error_ind_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += gnb_cu_cp_ue_e1_ap_id_present ? 1 : 0;
  nof_ies += gnb_cu_up_ue_e1_ap_id_present ? 1 : 0;
  nof_ies += cause_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (gnb_cu_cp_ue_e1_ap_id_present) {
    HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  }
  if (gnb_cu_up_ue_e1_ap_id_present) {
    HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  }
  if (cause_present) {
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 2: {
        gnb_cu_cp_ue_e1_ap_id_present = true;
        gnb_cu_cp_ue_e1_ap_id.id      = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        gnb_cu_up_ue_e1_ap_id_present = true;
        gnb_cu_up_ue_e1_ap_id.id      = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 0: {
        cause_present = true;
        cause.id      = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 1: {
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
void error_ind_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (gnb_cu_cp_ue_e1_ap_id_present) {
    j.write_fieldname("");
    gnb_cu_cp_ue_e1_ap_id.to_json(j);
  }
  if (gnb_cu_up_ue_e1_ap_id_present) {
    j.write_fieldname("");
    gnb_cu_up_ue_e1_ap_id.to_json(j);
  }
  if (cause_present) {
    j.write_fieldname("");
    cause.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_cfg_upd_ies_o>;

gnb_cu_cp_cfg_upd_ies_container::gnb_cu_cp_cfg_upd_ies_container() :
  transaction_id(57, crit_e::reject),
  gnb_cu_cp_name(9, crit_e::ignore),
  gnb_cu_cp_tnla_to_add_list(27, crit_e::ignore),
  gnb_cu_cp_tnla_to_rem_list(28, crit_e::ignore),
  gnb_cu_cp_tnla_to_upd_list(29, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_cp_cfg_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += gnb_cu_cp_name_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnla_to_add_list_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnla_to_rem_list_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnla_to_upd_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (gnb_cu_cp_name_present) {
    HANDLE_CODE(gnb_cu_cp_name.pack(bref));
  }
  if (gnb_cu_cp_tnla_to_add_list_present) {
    HANDLE_CODE(gnb_cu_cp_tnla_to_add_list.pack(bref));
  }
  if (gnb_cu_cp_tnla_to_rem_list_present) {
    HANDLE_CODE(gnb_cu_cp_tnla_to_rem_list.pack(bref));
  }
  if (gnb_cu_cp_tnla_to_upd_list_present) {
    HANDLE_CODE(gnb_cu_cp_tnla_to_upd_list.pack(bref));
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 9: {
        gnb_cu_cp_name_present = true;
        gnb_cu_cp_name.id      = id;
        HANDLE_CODE(gnb_cu_cp_name.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_name.value.unpack(bref));
        break;
      }
      case 27: {
        gnb_cu_cp_tnla_to_add_list_present = true;
        gnb_cu_cp_tnla_to_add_list.id      = id;
        HANDLE_CODE(gnb_cu_cp_tnla_to_add_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_tnla_to_add_list.value.unpack(bref));
        break;
      }
      case 28: {
        gnb_cu_cp_tnla_to_rem_list_present = true;
        gnb_cu_cp_tnla_to_rem_list.id      = id;
        HANDLE_CODE(gnb_cu_cp_tnla_to_rem_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_tnla_to_rem_list.value.unpack(bref));
        break;
      }
      case 29: {
        gnb_cu_cp_tnla_to_upd_list_present = true;
        gnb_cu_cp_tnla_to_upd_list.id      = id;
        HANDLE_CODE(gnb_cu_cp_tnla_to_upd_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_tnla_to_upd_list.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (gnb_cu_cp_name_present) {
    j.write_fieldname("");
    gnb_cu_cp_name.to_json(j);
  }
  if (gnb_cu_cp_tnla_to_add_list_present) {
    j.write_fieldname("");
    gnb_cu_cp_tnla_to_add_list.to_json(j);
  }
  if (gnb_cu_cp_tnla_to_rem_list_present) {
    j.write_fieldname("");
    gnb_cu_cp_tnla_to_rem_list.to_json(j);
  }
  if (gnb_cu_cp_tnla_to_upd_list_present) {
    j.write_fieldname("");
    gnb_cu_cp_tnla_to_upd_list.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_cfg_upd_ack_ies_o>;

gnb_cu_cp_cfg_upd_ack_ies_container::gnb_cu_cp_cfg_upd_ack_ies_container() :
  transaction_id(57, crit_e::reject),
  crit_diagnostics(1, crit_e::ignore),
  gnb_cu_cp_tnla_setup_list(30, crit_e::ignore),
  gnb_cu_cp_tnla_failed_to_setup_list(31, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_cp_cfg_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnla_setup_list_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnla_failed_to_setup_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (gnb_cu_cp_tnla_setup_list_present) {
    HANDLE_CODE(gnb_cu_cp_tnla_setup_list.pack(bref));
  }
  if (gnb_cu_cp_tnla_failed_to_setup_list_present) {
    HANDLE_CODE(gnb_cu_cp_tnla_failed_to_setup_list.pack(bref));
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 1: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      case 30: {
        gnb_cu_cp_tnla_setup_list_present = true;
        gnb_cu_cp_tnla_setup_list.id      = id;
        HANDLE_CODE(gnb_cu_cp_tnla_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_tnla_setup_list.value.unpack(bref));
        break;
      }
      case 31: {
        gnb_cu_cp_tnla_failed_to_setup_list_present = true;
        gnb_cu_cp_tnla_failed_to_setup_list.id      = id;
        HANDLE_CODE(gnb_cu_cp_tnla_failed_to_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_tnla_failed_to_setup_list.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  if (gnb_cu_cp_tnla_setup_list_present) {
    j.write_fieldname("");
    gnb_cu_cp_tnla_setup_list.to_json(j);
  }
  if (gnb_cu_cp_tnla_failed_to_setup_list_present) {
    j.write_fieldname("");
    gnb_cu_cp_tnla_failed_to_setup_list.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_cfg_upd_fail_ies_o>;

gnb_cu_cp_cfg_upd_fail_ies_container::gnb_cu_cp_cfg_upd_fail_ies_container() :
  transaction_id(57, crit_e::reject),
  cause(0, crit_e::ignore),
  time_to_wait(12, crit_e::ignore),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_cp_cfg_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (time_to_wait_present) {
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
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
      case 12: {
        time_to_wait_present = true;
        time_to_wait.id      = id;
        HANDLE_CODE(time_to_wait.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.value.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_cp_cfg_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_fieldname("");
    time_to_wait.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_e1_setup_fail_ies_o>;

gnb_cu_cp_e1_setup_fail_ies_container::gnb_cu_cp_e1_setup_fail_ies_container() :
  transaction_id(57, crit_e::reject),
  cause(0, crit_e::ignore),
  time_to_wait(12, crit_e::ignore),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_cp_e1_setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (time_to_wait_present) {
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
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
      case 12: {
        time_to_wait_present = true;
        time_to_wait.id      = id;
        HANDLE_CODE(time_to_wait.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.value.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_cp_e1_setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_fieldname("");
    time_to_wait.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_e1_setup_request_ies_o>;

gnb_cu_cp_e1_setup_request_ies_container::gnb_cu_cp_e1_setup_request_ies_container() :
  transaction_id(57, crit_e::reject), gnb_cu_cp_name(9, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_cp_e1_setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += gnb_cu_cp_name_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (gnb_cu_cp_name_present) {
    HANDLE_CODE(gnb_cu_cp_name.pack(bref));
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 9: {
        gnb_cu_cp_name_present = true;
        gnb_cu_cp_name.id      = id;
        HANDLE_CODE(gnb_cu_cp_name.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_name.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (gnb_cu_cp_name_present) {
    j.write_fieldname("");
    gnb_cu_cp_name.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_e1_setup_resp_ies_o>;

gnb_cu_cp_e1_setup_resp_ies_container::gnb_cu_cp_e1_setup_resp_ies_container() :
  transaction_id(57, crit_e::reject),
  gnb_cu_up_id(7, crit_e::reject),
  gnb_cu_up_name(8, crit_e::ignore),
  cn_support(10, crit_e::reject),
  supported_plmns(11, crit_e::reject),
  gnb_cu_up_capacity(64, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_cp_e1_setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += gnb_cu_up_name_present ? 1 : 0;
  nof_ies += gnb_cu_up_capacity_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_id.pack(bref));
  if (gnb_cu_up_name_present) {
    HANDLE_CODE(gnb_cu_up_name.pack(bref));
  }
  HANDLE_CODE(cn_support.pack(bref));
  HANDLE_CODE(supported_plmns.pack(bref));
  if (gnb_cu_up_capacity_present) {
    HANDLE_CODE(gnb_cu_up_capacity.pack(bref));
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 7: {
        nof_mandatory_ies--;
        gnb_cu_up_id.id = id;
        HANDLE_CODE(gnb_cu_up_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_id.value.unpack(bref));
        break;
      }
      case 8: {
        gnb_cu_up_name_present = true;
        gnb_cu_up_name.id      = id;
        HANDLE_CODE(gnb_cu_up_name.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_name.value.unpack(bref));
        break;
      }
      case 10: {
        nof_mandatory_ies--;
        cn_support.id = id;
        HANDLE_CODE(cn_support.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cn_support.value.unpack(bref));
        break;
      }
      case 11: {
        nof_mandatory_ies--;
        supported_plmns.id = id;
        HANDLE_CODE(supported_plmns.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(supported_plmns.value.unpack(bref));
        break;
      }
      case 64: {
        gnb_cu_up_capacity_present = true;
        gnb_cu_up_capacity.id      = id;
        HANDLE_CODE(gnb_cu_up_capacity.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_capacity.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_id.to_json(j);
  if (gnb_cu_up_name_present) {
    j.write_fieldname("");
    gnb_cu_up_name.to_json(j);
  }
  j.write_fieldname("");
  cn_support.to_json(j);
  j.write_fieldname("");
  supported_plmns.to_json(j);
  if (gnb_cu_up_capacity_present) {
    j.write_fieldname("");
    gnb_cu_up_capacity.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_cfg_upd_ies_o>;

gnb_cu_up_cfg_upd_ies_container::gnb_cu_up_cfg_upd_ies_container() :
  transaction_id(57, crit_e::reject),
  gnb_cu_up_id(7, crit_e::reject),
  gnb_cu_up_name(8, crit_e::ignore),
  supported_plmns(11, crit_e::reject),
  gnb_cu_up_capacity(64, crit_e::ignore),
  gnb_cu_up_tnla_to_rem_list(73, crit_e::reject)
{
}
SRSASN_CODE gnb_cu_up_cfg_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += gnb_cu_up_name_present ? 1 : 0;
  nof_ies += supported_plmns_present ? 1 : 0;
  nof_ies += gnb_cu_up_capacity_present ? 1 : 0;
  nof_ies += gnb_cu_up_tnla_to_rem_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_id.pack(bref));
  if (gnb_cu_up_name_present) {
    HANDLE_CODE(gnb_cu_up_name.pack(bref));
  }
  if (supported_plmns_present) {
    HANDLE_CODE(supported_plmns.pack(bref));
  }
  if (gnb_cu_up_capacity_present) {
    HANDLE_CODE(gnb_cu_up_capacity.pack(bref));
  }
  if (gnb_cu_up_tnla_to_rem_list_present) {
    HANDLE_CODE(gnb_cu_up_tnla_to_rem_list.pack(bref));
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 7: {
        nof_mandatory_ies--;
        gnb_cu_up_id.id = id;
        HANDLE_CODE(gnb_cu_up_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_id.value.unpack(bref));
        break;
      }
      case 8: {
        gnb_cu_up_name_present = true;
        gnb_cu_up_name.id      = id;
        HANDLE_CODE(gnb_cu_up_name.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_name.value.unpack(bref));
        break;
      }
      case 11: {
        supported_plmns_present = true;
        supported_plmns.id      = id;
        HANDLE_CODE(supported_plmns.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(supported_plmns.value.unpack(bref));
        break;
      }
      case 64: {
        gnb_cu_up_capacity_present = true;
        gnb_cu_up_capacity.id      = id;
        HANDLE_CODE(gnb_cu_up_capacity.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_capacity.value.unpack(bref));
        break;
      }
      case 73: {
        gnb_cu_up_tnla_to_rem_list_present = true;
        gnb_cu_up_tnla_to_rem_list.id      = id;
        HANDLE_CODE(gnb_cu_up_tnla_to_rem_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_tnla_to_rem_list.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_id.to_json(j);
  if (gnb_cu_up_name_present) {
    j.write_fieldname("");
    gnb_cu_up_name.to_json(j);
  }
  if (supported_plmns_present) {
    j.write_fieldname("");
    supported_plmns.to_json(j);
  }
  if (gnb_cu_up_capacity_present) {
    j.write_fieldname("");
    gnb_cu_up_capacity.to_json(j);
  }
  if (gnb_cu_up_tnla_to_rem_list_present) {
    j.write_fieldname("");
    gnb_cu_up_tnla_to_rem_list.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_cfg_upd_ack_ies_o>;

gnb_cu_up_cfg_upd_ack_ies_container::gnb_cu_up_cfg_upd_ack_ies_container() :
  transaction_id(57, crit_e::reject), crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_up_cfg_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_up_cfg_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_cfg_upd_fail_ies_o>;

gnb_cu_up_cfg_upd_fail_ies_container::gnb_cu_up_cfg_upd_fail_ies_container() :
  transaction_id(57, crit_e::reject),
  cause(0, crit_e::ignore),
  time_to_wait(12, crit_e::ignore),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_up_cfg_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (time_to_wait_present) {
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
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
      case 12: {
        time_to_wait_present = true;
        time_to_wait.id      = id;
        HANDLE_CODE(time_to_wait.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.value.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_up_cfg_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_fieldname("");
    time_to_wait.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_counter_check_request_ies_o>;

gnb_cu_up_counter_check_request_ies_container::gnb_cu_up_counter_check_request_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  sys_gnb_cu_up_counter_check_request(60, crit_e::reject)
{
}
SRSASN_CODE gnb_cu_up_counter_check_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(sys_gnb_cu_up_counter_check_request.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 60: {
        nof_mandatory_ies--;
        sys_gnb_cu_up_counter_check_request.id = id;
        HANDLE_CODE(sys_gnb_cu_up_counter_check_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_gnb_cu_up_counter_check_request.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  sys_gnb_cu_up_counter_check_request.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_e1_setup_fail_ies_o>;

gnb_cu_up_e1_setup_fail_ies_container::gnb_cu_up_e1_setup_fail_ies_container() :
  transaction_id(57, crit_e::reject),
  cause(0, crit_e::ignore),
  time_to_wait(12, crit_e::ignore),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_up_e1_setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (time_to_wait_present) {
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
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
      case 12: {
        time_to_wait_present = true;
        time_to_wait.id      = id;
        HANDLE_CODE(time_to_wait.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.value.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_up_e1_setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_fieldname("");
    time_to_wait.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_e1_setup_request_ies_o>;

gnb_cu_up_e1_setup_request_ies_container::gnb_cu_up_e1_setup_request_ies_container() :
  transaction_id(57, crit_e::reject),
  gnb_cu_up_id(7, crit_e::reject),
  gnb_cu_up_name(8, crit_e::ignore),
  cn_support(10, crit_e::reject),
  supported_plmns(11, crit_e::reject),
  gnb_cu_up_capacity(64, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_up_e1_setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += gnb_cu_up_name_present ? 1 : 0;
  nof_ies += gnb_cu_up_capacity_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_id.pack(bref));
  if (gnb_cu_up_name_present) {
    HANDLE_CODE(gnb_cu_up_name.pack(bref));
  }
  HANDLE_CODE(cn_support.pack(bref));
  HANDLE_CODE(supported_plmns.pack(bref));
  if (gnb_cu_up_capacity_present) {
    HANDLE_CODE(gnb_cu_up_capacity.pack(bref));
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 7: {
        nof_mandatory_ies--;
        gnb_cu_up_id.id = id;
        HANDLE_CODE(gnb_cu_up_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_id.value.unpack(bref));
        break;
      }
      case 8: {
        gnb_cu_up_name_present = true;
        gnb_cu_up_name.id      = id;
        HANDLE_CODE(gnb_cu_up_name.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_name.value.unpack(bref));
        break;
      }
      case 10: {
        nof_mandatory_ies--;
        cn_support.id = id;
        HANDLE_CODE(cn_support.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cn_support.value.unpack(bref));
        break;
      }
      case 11: {
        nof_mandatory_ies--;
        supported_plmns.id = id;
        HANDLE_CODE(supported_plmns.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(supported_plmns.value.unpack(bref));
        break;
      }
      case 64: {
        gnb_cu_up_capacity_present = true;
        gnb_cu_up_capacity.id      = id;
        HANDLE_CODE(gnb_cu_up_capacity.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_capacity.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_id.to_json(j);
  if (gnb_cu_up_name_present) {
    j.write_fieldname("");
    gnb_cu_up_name.to_json(j);
  }
  j.write_fieldname("");
  cn_support.to_json(j);
  j.write_fieldname("");
  supported_plmns.to_json(j);
  if (gnb_cu_up_capacity_present) {
    j.write_fieldname("");
    gnb_cu_up_capacity.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_e1_setup_resp_ies_o>;

gnb_cu_up_e1_setup_resp_ies_container::gnb_cu_up_e1_setup_resp_ies_container() :
  transaction_id(57, crit_e::reject), gnb_cu_cp_name(9, crit_e::ignore)
{
}
SRSASN_CODE gnb_cu_up_e1_setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += gnb_cu_cp_name_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (gnb_cu_cp_name_present) {
    HANDLE_CODE(gnb_cu_cp_name.pack(bref));
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 9: {
        gnb_cu_cp_name_present = true;
        gnb_cu_cp_name.id      = id;
        HANDLE_CODE(gnb_cu_cp_name.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_name.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (gnb_cu_cp_name_present) {
    j.write_fieldname("");
    gnb_cu_cp_name.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_status_ind_ies_o>;

gnb_cu_up_status_ind_ies_container::gnb_cu_up_status_ind_ies_container() :
  transaction_id(57, crit_e::reject), gnb_cu_up_overload_info(65, crit_e::reject)
{
}
SRSASN_CODE gnb_cu_up_status_ind_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_overload_info.pack(bref));

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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 65: {
        nof_mandatory_ies--;
        gnb_cu_up_overload_info.id = id;
        HANDLE_CODE(gnb_cu_up_overload_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_overload_info.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_overload_info.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<mrdc_data_usage_report_ies_o>;

mrdc_data_usage_report_ies_container::mrdc_data_usage_report_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  pdu_session_res_data_usage_list(68, crit_e::ignore)
{
}
SRSASN_CODE mrdc_data_usage_report_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(pdu_session_res_data_usage_list.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 68: {
        nof_mandatory_ies--;
        pdu_session_res_data_usage_list.id = id;
        HANDLE_CODE(pdu_session_res_data_usage_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_res_data_usage_list.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  pdu_session_res_data_usage_list.to_json(j);
  j.end_obj();
}

template <class valueT_>
private_ie_container_item_s<valueT_>::private_ie_container_item_s(private_ie_id_c id_, crit_e crit_) :
  id(id_), crit(crit_)

{
}
template <class valueT_>
SRSASN_CODE private_ie_container_item_s<valueT_>::pack(bit_ref& bref) const
{
  HANDLE_CODE(id.pack(bref));
  HANDLE_CODE(crit.pack(bref));
  {
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(value.pack(bref));
  }
  return SRSASN_SUCCESS;
}
template <class valueT_>
SRSASN_CODE private_ie_container_item_s<valueT_>::unpack(cbit_ref& bref)
{
  HANDLE_CODE(id.unpack(bref));
  HANDLE_CODE(crit.unpack(bref));
  {
    varlength_field_unpack_guard varlen_scope(bref, true);
    HANDLE_CODE(value.unpack(bref));
  }
  return SRSASN_SUCCESS;
}
template <class valueT_>
void private_ie_container_item_s<valueT_>::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("id");
  id.to_json(j);
  j.write_str("criticality", crit.to_string());
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

reset_ies_container::reset_ies_container() :
  transaction_id(57, crit_e::reject), cause(0, crit_e::ignore), reset_type(4, crit_e::reject)
{
}
SRSASN_CODE reset_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  HANDLE_CODE(reset_type.pack(bref));

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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
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
      case 4: {
        nof_mandatory_ies--;
        reset_type.id = id;
        HANDLE_CODE(reset_type.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(reset_type.value.unpack(bref));
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
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  j.write_fieldname("");
  reset_type.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<reset_ack_ies_o>;

reset_ack_ies_container::reset_ack_ies_container() :
  transaction_id(57, crit_e::reject),
  ue_associated_lc_e1_conn_list_res_ack(6, crit_e::ignore),
  crit_diagnostics(1, crit_e::ignore)
{
}
SRSASN_CODE reset_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += ue_associated_lc_e1_conn_list_res_ack_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (ue_associated_lc_e1_conn_list_res_ack_present) {
    HANDLE_CODE(ue_associated_lc_e1_conn_list_res_ack.pack(bref));
  }
  if (crit_diagnostics_present) {
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
    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 6: {
        ue_associated_lc_e1_conn_list_res_ack_present = true;
        ue_associated_lc_e1_conn_list_res_ack.id      = id;
        HANDLE_CODE(ue_associated_lc_e1_conn_list_res_ack.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_associated_lc_e1_conn_list_res_ack.value.unpack(bref));
        break;
      }
      case 1: {
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
void reset_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (ue_associated_lc_e1_conn_list_res_ack_present) {
    j.write_fieldname("");
    ue_associated_lc_e1_conn_list_res_ack.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ul_data_notif_ies_o>;

ul_data_notif_ies_container::ul_data_notif_ies_container() :
  gnb_cu_cp_ue_e1_ap_id(2, crit_e::reject),
  gnb_cu_up_ue_e1_ap_id(3, crit_e::reject),
  pdu_session_to_notify_list(67, crit_e::reject)
{
}
SRSASN_CODE ul_data_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.pack(bref));
  HANDLE_CODE(pdu_session_to_notify_list.pack(bref));

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
    switch (id) {
      case 2: {
        nof_mandatory_ies--;
        gnb_cu_cp_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        gnb_cu_up_ue_e1_ap_id.id = id;
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_ue_e1_ap_id.value.unpack(bref));
        break;
      }
      case 67: {
        nof_mandatory_ies--;
        pdu_session_to_notify_list.id = id;
        HANDLE_CODE(pdu_session_to_notify_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pdu_session_to_notify_list.value.unpack(bref));
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
  j.write_fieldname("");
  gnb_cu_cp_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  gnb_cu_up_ue_e1_ap_id.to_json(j);
  j.write_fieldname("");
  pdu_session_to_notify_list.to_json(j);
  j.end_obj();
}

// E1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E1AP-ELEMENTARY-PROCEDURE
uint16_t e1_ap_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t options[] = {0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1, 12, 13, 14, 18, 15, 16, 17, 19, 2};
  return map_enum_number(options, 20, idx, "proc_code");
}
bool e1_ap_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t options[] = {0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1, 12, 13, 14, 18, 15, 16, 17, 19, 2};
  for (const auto& o : options) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
e1_ap_elem_procs_o::init_msg_c e1_ap_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(init_msg_c::types::reset);
      break;
    case 3:
      ret.set(init_msg_c::types::gnb_cu_up_e1_setup_request);
      break;
    case 4:
      ret.set(init_msg_c::types::gnb_cu_cp_e1_setup_request);
      break;
    case 5:
      ret.set(init_msg_c::types::gnb_cu_up_cfg_upd);
      break;
    case 6:
      ret.set(init_msg_c::types::gnb_cu_cp_cfg_upd);
      break;
    case 7:
      ret.set(init_msg_c::types::e1_release_request);
      break;
    case 8:
      ret.set(init_msg_c::types::bearer_context_setup_request);
      break;
    case 9:
      ret.set(init_msg_c::types::bearer_context_mod_request);
      break;
    case 10:
      ret.set(init_msg_c::types::bearer_context_mod_required);
      break;
    case 11:
      ret.set(init_msg_c::types::bearer_context_release_cmd);
      break;
    case 1:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 12:
      ret.set(init_msg_c::types::bearer_context_release_request);
      break;
    case 13:
      ret.set(init_msg_c::types::bearer_context_inactivity_notif);
      break;
    case 14:
      ret.set(init_msg_c::types::dl_data_notif);
      break;
    case 18:
      ret.set(init_msg_c::types::ul_data_notif);
      break;
    case 15:
      ret.set(init_msg_c::types::data_usage_report);
      break;
    case 16:
      ret.set(init_msg_c::types::gnb_cu_up_counter_check_request);
      break;
    case 17:
      ret.set(init_msg_c::types::gnb_cu_up_status_ind);
      break;
    case 19:
      ret.set(init_msg_c::types::mrdc_data_usage_report);
      break;
    case 2:
      ret.set(init_msg_c::types::private_msg);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e1_ap_elem_procs_o::successful_outcome_c e1_ap_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(successful_outcome_c::types::reset_ack);
      break;
    case 3:
      ret.set(successful_outcome_c::types::gnb_cu_up_e1_setup_resp);
      break;
    case 4:
      ret.set(successful_outcome_c::types::gnb_cu_cp_e1_setup_resp);
      break;
    case 5:
      ret.set(successful_outcome_c::types::gnb_cu_up_cfg_upd_ack);
      break;
    case 6:
      ret.set(successful_outcome_c::types::gnb_cu_cp_cfg_upd_ack);
      break;
    case 7:
      ret.set(successful_outcome_c::types::e1_release_resp);
      break;
    case 8:
      ret.set(successful_outcome_c::types::bearer_context_setup_resp);
      break;
    case 9:
      ret.set(successful_outcome_c::types::bearer_context_mod_resp);
      break;
    case 10:
      ret.set(successful_outcome_c::types::bearer_context_mod_confirm);
      break;
    case 11:
      ret.set(successful_outcome_c::types::bearer_context_release_complete);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e1_ap_elem_procs_o::unsuccessful_outcome_c e1_ap_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 3:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_up_e1_setup_fail);
      break;
    case 4:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_cp_e1_setup_fail);
      break;
    case 5:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_up_cfg_upd_fail);
      break;
    case 6:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_cp_cfg_upd_fail);
      break;
    case 8:
      ret.set(unsuccessful_outcome_c::types::bearer_context_setup_fail);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::bearer_context_mod_fail);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e e1_ap_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 0:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 4:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 6:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 8:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 13:
      return crit_e::ignore;
    case 14:
      return crit_e::ignore;
    case 18:
      return crit_e::ignore;
    case 15:
      return crit_e::ignore;
    case 16:
      return crit_e::ignore;
    case 17:
      return crit_e::ignore;
    case 19:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void e1_ap_elem_procs_o::init_msg_c::destroy_()
{
  switch (type_) {
    case types::reset:
      c.destroy<reset_s>();
      break;
    case types::gnb_cu_up_e1_setup_request:
      c.destroy<gnb_cu_up_e1_setup_request_s>();
      break;
    case types::gnb_cu_cp_e1_setup_request:
      c.destroy<gnb_cu_cp_e1_setup_request_s>();
      break;
    case types::gnb_cu_up_cfg_upd:
      c.destroy<gnb_cu_up_cfg_upd_s>();
      break;
    case types::gnb_cu_cp_cfg_upd:
      c.destroy<gnb_cu_cp_cfg_upd_s>();
      break;
    case types::e1_release_request:
      c.destroy<e1_release_request_s>();
      break;
    case types::bearer_context_setup_request:
      c.destroy<bearer_context_setup_request_s>();
      break;
    case types::bearer_context_mod_request:
      c.destroy<bearer_context_mod_request_s>();
      break;
    case types::bearer_context_mod_required:
      c.destroy<bearer_context_mod_required_s>();
      break;
    case types::bearer_context_release_cmd:
      c.destroy<bearer_context_release_cmd_s>();
      break;
    case types::error_ind:
      c.destroy<error_ind_s>();
      break;
    case types::bearer_context_release_request:
      c.destroy<bearer_context_release_request_s>();
      break;
    case types::bearer_context_inactivity_notif:
      c.destroy<bearer_context_inactivity_notif_s>();
      break;
    case types::dl_data_notif:
      c.destroy<dl_data_notif_s>();
      break;
    case types::ul_data_notif:
      c.destroy<ul_data_notif_s>();
      break;
    case types::data_usage_report:
      c.destroy<data_usage_report_s>();
      break;
    case types::gnb_cu_up_counter_check_request:
      c.destroy<gnb_cu_up_counter_check_request_s>();
      break;
    case types::gnb_cu_up_status_ind:
      c.destroy<gnb_cu_up_status_ind_s>();
      break;
    case types::mrdc_data_usage_report:
      c.destroy<mrdc_data_usage_report_s>();
      break;
    case types::private_msg:
      c.destroy<private_msg_s>();
      break;
    default:
      break;
  }
}
void e1_ap_elem_procs_o::init_msg_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::reset:
      c.init<reset_s>();
      break;
    case types::gnb_cu_up_e1_setup_request:
      c.init<gnb_cu_up_e1_setup_request_s>();
      break;
    case types::gnb_cu_cp_e1_setup_request:
      c.init<gnb_cu_cp_e1_setup_request_s>();
      break;
    case types::gnb_cu_up_cfg_upd:
      c.init<gnb_cu_up_cfg_upd_s>();
      break;
    case types::gnb_cu_cp_cfg_upd:
      c.init<gnb_cu_cp_cfg_upd_s>();
      break;
    case types::e1_release_request:
      c.init<e1_release_request_s>();
      break;
    case types::bearer_context_setup_request:
      c.init<bearer_context_setup_request_s>();
      break;
    case types::bearer_context_mod_request:
      c.init<bearer_context_mod_request_s>();
      break;
    case types::bearer_context_mod_required:
      c.init<bearer_context_mod_required_s>();
      break;
    case types::bearer_context_release_cmd:
      c.init<bearer_context_release_cmd_s>();
      break;
    case types::error_ind:
      c.init<error_ind_s>();
      break;
    case types::bearer_context_release_request:
      c.init<bearer_context_release_request_s>();
      break;
    case types::bearer_context_inactivity_notif:
      c.init<bearer_context_inactivity_notif_s>();
      break;
    case types::dl_data_notif:
      c.init<dl_data_notif_s>();
      break;
    case types::ul_data_notif:
      c.init<ul_data_notif_s>();
      break;
    case types::data_usage_report:
      c.init<data_usage_report_s>();
      break;
    case types::gnb_cu_up_counter_check_request:
      c.init<gnb_cu_up_counter_check_request_s>();
      break;
    case types::gnb_cu_up_status_ind:
      c.init<gnb_cu_up_status_ind_s>();
      break;
    case types::mrdc_data_usage_report:
      c.init<mrdc_data_usage_report_s>();
      break;
    case types::private_msg:
      c.init<private_msg_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::init_msg_c");
  }
}
e1_ap_elem_procs_o::init_msg_c::init_msg_c(const e1_ap_elem_procs_o::init_msg_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::reset:
      c.init(other.c.get<reset_s>());
      break;
    case types::gnb_cu_up_e1_setup_request:
      c.init(other.c.get<gnb_cu_up_e1_setup_request_s>());
      break;
    case types::gnb_cu_cp_e1_setup_request:
      c.init(other.c.get<gnb_cu_cp_e1_setup_request_s>());
      break;
    case types::gnb_cu_up_cfg_upd:
      c.init(other.c.get<gnb_cu_up_cfg_upd_s>());
      break;
    case types::gnb_cu_cp_cfg_upd:
      c.init(other.c.get<gnb_cu_cp_cfg_upd_s>());
      break;
    case types::e1_release_request:
      c.init(other.c.get<e1_release_request_s>());
      break;
    case types::bearer_context_setup_request:
      c.init(other.c.get<bearer_context_setup_request_s>());
      break;
    case types::bearer_context_mod_request:
      c.init(other.c.get<bearer_context_mod_request_s>());
      break;
    case types::bearer_context_mod_required:
      c.init(other.c.get<bearer_context_mod_required_s>());
      break;
    case types::bearer_context_release_cmd:
      c.init(other.c.get<bearer_context_release_cmd_s>());
      break;
    case types::error_ind:
      c.init(other.c.get<error_ind_s>());
      break;
    case types::bearer_context_release_request:
      c.init(other.c.get<bearer_context_release_request_s>());
      break;
    case types::bearer_context_inactivity_notif:
      c.init(other.c.get<bearer_context_inactivity_notif_s>());
      break;
    case types::dl_data_notif:
      c.init(other.c.get<dl_data_notif_s>());
      break;
    case types::ul_data_notif:
      c.init(other.c.get<ul_data_notif_s>());
      break;
    case types::data_usage_report:
      c.init(other.c.get<data_usage_report_s>());
      break;
    case types::gnb_cu_up_counter_check_request:
      c.init(other.c.get<gnb_cu_up_counter_check_request_s>());
      break;
    case types::gnb_cu_up_status_ind:
      c.init(other.c.get<gnb_cu_up_status_ind_s>());
      break;
    case types::mrdc_data_usage_report:
      c.init(other.c.get<mrdc_data_usage_report_s>());
      break;
    case types::private_msg:
      c.init(other.c.get<private_msg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::init_msg_c");
  }
}
e1_ap_elem_procs_o::init_msg_c& e1_ap_elem_procs_o::init_msg_c::operator=(const e1_ap_elem_procs_o::init_msg_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::reset:
      c.set(other.c.get<reset_s>());
      break;
    case types::gnb_cu_up_e1_setup_request:
      c.set(other.c.get<gnb_cu_up_e1_setup_request_s>());
      break;
    case types::gnb_cu_cp_e1_setup_request:
      c.set(other.c.get<gnb_cu_cp_e1_setup_request_s>());
      break;
    case types::gnb_cu_up_cfg_upd:
      c.set(other.c.get<gnb_cu_up_cfg_upd_s>());
      break;
    case types::gnb_cu_cp_cfg_upd:
      c.set(other.c.get<gnb_cu_cp_cfg_upd_s>());
      break;
    case types::e1_release_request:
      c.set(other.c.get<e1_release_request_s>());
      break;
    case types::bearer_context_setup_request:
      c.set(other.c.get<bearer_context_setup_request_s>());
      break;
    case types::bearer_context_mod_request:
      c.set(other.c.get<bearer_context_mod_request_s>());
      break;
    case types::bearer_context_mod_required:
      c.set(other.c.get<bearer_context_mod_required_s>());
      break;
    case types::bearer_context_release_cmd:
      c.set(other.c.get<bearer_context_release_cmd_s>());
      break;
    case types::error_ind:
      c.set(other.c.get<error_ind_s>());
      break;
    case types::bearer_context_release_request:
      c.set(other.c.get<bearer_context_release_request_s>());
      break;
    case types::bearer_context_inactivity_notif:
      c.set(other.c.get<bearer_context_inactivity_notif_s>());
      break;
    case types::dl_data_notif:
      c.set(other.c.get<dl_data_notif_s>());
      break;
    case types::ul_data_notif:
      c.set(other.c.get<ul_data_notif_s>());
      break;
    case types::data_usage_report:
      c.set(other.c.get<data_usage_report_s>());
      break;
    case types::gnb_cu_up_counter_check_request:
      c.set(other.c.get<gnb_cu_up_counter_check_request_s>());
      break;
    case types::gnb_cu_up_status_ind:
      c.set(other.c.get<gnb_cu_up_status_ind_s>());
      break;
    case types::mrdc_data_usage_report:
      c.set(other.c.get<mrdc_data_usage_report_s>());
      break;
    case types::private_msg:
      c.set(other.c.get<private_msg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::init_msg_c");
  }

  return *this;
}
reset_s& e1_ap_elem_procs_o::init_msg_c::reset()
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
gnb_cu_up_e1_setup_request_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_up_e1_setup_request()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_e1_setup_request_s>();
}
gnb_cu_cp_e1_setup_request_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_cp_e1_setup_request()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_e1_setup_request_s>();
}
gnb_cu_up_cfg_upd_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_up_cfg_upd()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_cfg_upd_s>();
}
gnb_cu_cp_cfg_upd_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_cp_cfg_upd()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_cfg_upd_s>();
}
e1_release_request_s& e1_ap_elem_procs_o::init_msg_c::e1_release_request()
{
  assert_choice_type(types::e1_release_request, type_, "InitiatingMessage");
  return c.get<e1_release_request_s>();
}
bearer_context_setup_request_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_setup_request()
{
  assert_choice_type(types::bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<bearer_context_setup_request_s>();
}
bearer_context_mod_request_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_mod_request()
{
  assert_choice_type(types::bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_request_s>();
}
bearer_context_mod_required_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_mod_required()
{
  assert_choice_type(types::bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_required_s>();
}
bearer_context_release_cmd_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_release_cmd()
{
  assert_choice_type(types::bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<bearer_context_release_cmd_s>();
}
error_ind_s& e1_ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
bearer_context_release_request_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_release_request()
{
  assert_choice_type(types::bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<bearer_context_release_request_s>();
}
bearer_context_inactivity_notif_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_inactivity_notif()
{
  assert_choice_type(types::bearer_context_inactivity_notif, type_, "InitiatingMessage");
  return c.get<bearer_context_inactivity_notif_s>();
}
dl_data_notif_s& e1_ap_elem_procs_o::init_msg_c::dl_data_notif()
{
  assert_choice_type(types::dl_data_notif, type_, "InitiatingMessage");
  return c.get<dl_data_notif_s>();
}
ul_data_notif_s& e1_ap_elem_procs_o::init_msg_c::ul_data_notif()
{
  assert_choice_type(types::ul_data_notif, type_, "InitiatingMessage");
  return c.get<ul_data_notif_s>();
}
data_usage_report_s& e1_ap_elem_procs_o::init_msg_c::data_usage_report()
{
  assert_choice_type(types::data_usage_report, type_, "InitiatingMessage");
  return c.get<data_usage_report_s>();
}
gnb_cu_up_counter_check_request_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_up_counter_check_request()
{
  assert_choice_type(types::gnb_cu_up_counter_check_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_counter_check_request_s>();
}
gnb_cu_up_status_ind_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_up_status_ind()
{
  assert_choice_type(types::gnb_cu_up_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_status_ind_s>();
}
mrdc_data_usage_report_s& e1_ap_elem_procs_o::init_msg_c::mrdc_data_usage_report()
{
  assert_choice_type(types::mrdc_data_usage_report, type_, "InitiatingMessage");
  return c.get<mrdc_data_usage_report_s>();
}
private_msg_s& e1_ap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const reset_s& e1_ap_elem_procs_o::init_msg_c::reset() const
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
const gnb_cu_up_e1_setup_request_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_up_e1_setup_request() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_e1_setup_request_s>();
}
const gnb_cu_cp_e1_setup_request_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_cp_e1_setup_request() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_e1_setup_request_s>();
}
const gnb_cu_up_cfg_upd_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_up_cfg_upd() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_cfg_upd_s>();
}
const gnb_cu_cp_cfg_upd_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_cp_cfg_upd() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_cfg_upd_s>();
}
const e1_release_request_s& e1_ap_elem_procs_o::init_msg_c::e1_release_request() const
{
  assert_choice_type(types::e1_release_request, type_, "InitiatingMessage");
  return c.get<e1_release_request_s>();
}
const bearer_context_setup_request_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_setup_request() const
{
  assert_choice_type(types::bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<bearer_context_setup_request_s>();
}
const bearer_context_mod_request_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_mod_request() const
{
  assert_choice_type(types::bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_request_s>();
}
const bearer_context_mod_required_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_mod_required() const
{
  assert_choice_type(types::bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_required_s>();
}
const bearer_context_release_cmd_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_release_cmd() const
{
  assert_choice_type(types::bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<bearer_context_release_cmd_s>();
}
const error_ind_s& e1_ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const bearer_context_release_request_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_release_request() const
{
  assert_choice_type(types::bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<bearer_context_release_request_s>();
}
const bearer_context_inactivity_notif_s& e1_ap_elem_procs_o::init_msg_c::bearer_context_inactivity_notif() const
{
  assert_choice_type(types::bearer_context_inactivity_notif, type_, "InitiatingMessage");
  return c.get<bearer_context_inactivity_notif_s>();
}
const dl_data_notif_s& e1_ap_elem_procs_o::init_msg_c::dl_data_notif() const
{
  assert_choice_type(types::dl_data_notif, type_, "InitiatingMessage");
  return c.get<dl_data_notif_s>();
}
const ul_data_notif_s& e1_ap_elem_procs_o::init_msg_c::ul_data_notif() const
{
  assert_choice_type(types::ul_data_notif, type_, "InitiatingMessage");
  return c.get<ul_data_notif_s>();
}
const data_usage_report_s& e1_ap_elem_procs_o::init_msg_c::data_usage_report() const
{
  assert_choice_type(types::data_usage_report, type_, "InitiatingMessage");
  return c.get<data_usage_report_s>();
}
const gnb_cu_up_counter_check_request_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_up_counter_check_request() const
{
  assert_choice_type(types::gnb_cu_up_counter_check_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_counter_check_request_s>();
}
const gnb_cu_up_status_ind_s& e1_ap_elem_procs_o::init_msg_c::gnb_cu_up_status_ind() const
{
  assert_choice_type(types::gnb_cu_up_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_status_ind_s>();
}
const mrdc_data_usage_report_s& e1_ap_elem_procs_o::init_msg_c::mrdc_data_usage_report() const
{
  assert_choice_type(types::mrdc_data_usage_report, type_, "InitiatingMessage");
  return c.get<mrdc_data_usage_report_s>();
}
const private_msg_s& e1_ap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
void e1_ap_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::reset:
      j.write_fieldname("Reset");
      c.get<reset_s>().to_json(j);
      break;
    case types::gnb_cu_up_e1_setup_request:
      j.write_fieldname("GNB-CU-UP-E1SetupRequest");
      c.get<gnb_cu_up_e1_setup_request_s>().to_json(j);
      break;
    case types::gnb_cu_cp_e1_setup_request:
      j.write_fieldname("GNB-CU-CP-E1SetupRequest");
      c.get<gnb_cu_cp_e1_setup_request_s>().to_json(j);
      break;
    case types::gnb_cu_up_cfg_upd:
      j.write_fieldname("GNB-CU-UP-ConfigurationUpdate");
      c.get<gnb_cu_up_cfg_upd_s>().to_json(j);
      break;
    case types::gnb_cu_cp_cfg_upd:
      j.write_fieldname("GNB-CU-CP-ConfigurationUpdate");
      c.get<gnb_cu_cp_cfg_upd_s>().to_json(j);
      break;
    case types::e1_release_request:
      j.write_fieldname("E1ReleaseRequest");
      c.get<e1_release_request_s>().to_json(j);
      break;
    case types::bearer_context_setup_request:
      j.write_fieldname("BearerContextSetupRequest");
      c.get<bearer_context_setup_request_s>().to_json(j);
      break;
    case types::bearer_context_mod_request:
      j.write_fieldname("BearerContextModificationRequest");
      c.get<bearer_context_mod_request_s>().to_json(j);
      break;
    case types::bearer_context_mod_required:
      j.write_fieldname("BearerContextModificationRequired");
      c.get<bearer_context_mod_required_s>().to_json(j);
      break;
    case types::bearer_context_release_cmd:
      j.write_fieldname("BearerContextReleaseCommand");
      c.get<bearer_context_release_cmd_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::bearer_context_release_request:
      j.write_fieldname("BearerContextReleaseRequest");
      c.get<bearer_context_release_request_s>().to_json(j);
      break;
    case types::bearer_context_inactivity_notif:
      j.write_fieldname("BearerContextInactivityNotification");
      c.get<bearer_context_inactivity_notif_s>().to_json(j);
      break;
    case types::dl_data_notif:
      j.write_fieldname("DLDataNotification");
      c.get<dl_data_notif_s>().to_json(j);
      break;
    case types::ul_data_notif:
      j.write_fieldname("ULDataNotification");
      c.get<ul_data_notif_s>().to_json(j);
      break;
    case types::data_usage_report:
      j.write_fieldname("DataUsageReport");
      c.get<data_usage_report_s>().to_json(j);
      break;
    case types::gnb_cu_up_counter_check_request:
      j.write_fieldname("GNB-CU-UP-CounterCheckRequest");
      c.get<gnb_cu_up_counter_check_request_s>().to_json(j);
      break;
    case types::gnb_cu_up_status_ind:
      j.write_fieldname("GNB-CU-UP-StatusIndication");
      c.get<gnb_cu_up_status_ind_s>().to_json(j);
      break;
    case types::mrdc_data_usage_report:
      j.write_fieldname("MRDC-DataUsageReport");
      c.get<mrdc_data_usage_report_s>().to_json(j);
      break;
    case types::private_msg:
      j.write_fieldname("PrivateMessage");
      c.get<private_msg_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE e1_ap_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset:
      HANDLE_CODE(c.get<reset_s>().pack(bref));
      break;
    case types::gnb_cu_up_e1_setup_request:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_request_s>().pack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_request:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_request_s>().pack(bref));
      break;
    case types::gnb_cu_up_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_s>().pack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_s>().pack(bref));
      break;
    case types::e1_release_request:
      HANDLE_CODE(c.get<e1_release_request_s>().pack(bref));
      break;
    case types::bearer_context_setup_request:
      HANDLE_CODE(c.get<bearer_context_setup_request_s>().pack(bref));
      break;
    case types::bearer_context_mod_request:
      HANDLE_CODE(c.get<bearer_context_mod_request_s>().pack(bref));
      break;
    case types::bearer_context_mod_required:
      HANDLE_CODE(c.get<bearer_context_mod_required_s>().pack(bref));
      break;
    case types::bearer_context_release_cmd:
      HANDLE_CODE(c.get<bearer_context_release_cmd_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::bearer_context_release_request:
      HANDLE_CODE(c.get<bearer_context_release_request_s>().pack(bref));
      break;
    case types::bearer_context_inactivity_notif:
      HANDLE_CODE(c.get<bearer_context_inactivity_notif_s>().pack(bref));
      break;
    case types::dl_data_notif:
      HANDLE_CODE(c.get<dl_data_notif_s>().pack(bref));
      break;
    case types::ul_data_notif:
      HANDLE_CODE(c.get<ul_data_notif_s>().pack(bref));
      break;
    case types::data_usage_report:
      HANDLE_CODE(c.get<data_usage_report_s>().pack(bref));
      break;
    case types::gnb_cu_up_counter_check_request:
      HANDLE_CODE(c.get<gnb_cu_up_counter_check_request_s>().pack(bref));
      break;
    case types::gnb_cu_up_status_ind:
      HANDLE_CODE(c.get<gnb_cu_up_status_ind_s>().pack(bref));
      break;
    case types::mrdc_data_usage_report:
      HANDLE_CODE(c.get<mrdc_data_usage_report_s>().pack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1_ap_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset:
      HANDLE_CODE(c.get<reset_s>().unpack(bref));
      break;
    case types::gnb_cu_up_e1_setup_request:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_request_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_request:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_request_s>().unpack(bref));
      break;
    case types::gnb_cu_up_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_s>().unpack(bref));
      break;
    case types::e1_release_request:
      HANDLE_CODE(c.get<e1_release_request_s>().unpack(bref));
      break;
    case types::bearer_context_setup_request:
      HANDLE_CODE(c.get<bearer_context_setup_request_s>().unpack(bref));
      break;
    case types::bearer_context_mod_request:
      HANDLE_CODE(c.get<bearer_context_mod_request_s>().unpack(bref));
      break;
    case types::bearer_context_mod_required:
      HANDLE_CODE(c.get<bearer_context_mod_required_s>().unpack(bref));
      break;
    case types::bearer_context_release_cmd:
      HANDLE_CODE(c.get<bearer_context_release_cmd_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::bearer_context_release_request:
      HANDLE_CODE(c.get<bearer_context_release_request_s>().unpack(bref));
      break;
    case types::bearer_context_inactivity_notif:
      HANDLE_CODE(c.get<bearer_context_inactivity_notif_s>().unpack(bref));
      break;
    case types::dl_data_notif:
      HANDLE_CODE(c.get<dl_data_notif_s>().unpack(bref));
      break;
    case types::ul_data_notif:
      HANDLE_CODE(c.get<ul_data_notif_s>().unpack(bref));
      break;
    case types::data_usage_report:
      HANDLE_CODE(c.get<data_usage_report_s>().unpack(bref));
      break;
    case types::gnb_cu_up_counter_check_request:
      HANDLE_CODE(c.get<gnb_cu_up_counter_check_request_s>().unpack(bref));
      break;
    case types::gnb_cu_up_status_ind:
      HANDLE_CODE(c.get<gnb_cu_up_status_ind_s>().unpack(bref));
      break;
    case types::mrdc_data_usage_report:
      HANDLE_CODE(c.get<mrdc_data_usage_report_s>().unpack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1_ap_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* options[] = {"Reset",
                                  "GNB-CU-UP-E1SetupRequest",
                                  "GNB-CU-CP-E1SetupRequest",
                                  "GNB-CU-UP-ConfigurationUpdate",
                                  "GNB-CU-CP-ConfigurationUpdate",
                                  "E1ReleaseRequest",
                                  "BearerContextSetupRequest",
                                  "BearerContextModificationRequest",
                                  "BearerContextModificationRequired",
                                  "BearerContextReleaseCommand",
                                  "ErrorIndication",
                                  "BearerContextReleaseRequest",
                                  "BearerContextInactivityNotification",
                                  "DLDataNotification",
                                  "ULDataNotification",
                                  "DataUsageReport",
                                  "GNB-CU-UP-CounterCheckRequest",
                                  "GNB-CU-UP-StatusIndication",
                                  "MRDC-DataUsageReport",
                                  "PrivateMessage"};
  return convert_enum_idx(options, 20, value, "e1_ap_elem_procs_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void e1_ap_elem_procs_o::successful_outcome_c::destroy_()
{
  switch (type_) {
    case types::reset_ack:
      c.destroy<reset_ack_s>();
      break;
    case types::gnb_cu_up_e1_setup_resp:
      c.destroy<gnb_cu_up_e1_setup_resp_s>();
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      c.destroy<gnb_cu_cp_e1_setup_resp_s>();
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      c.destroy<gnb_cu_up_cfg_upd_ack_s>();
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      c.destroy<gnb_cu_cp_cfg_upd_ack_s>();
      break;
    case types::e1_release_resp:
      c.destroy<e1_release_resp_s>();
      break;
    case types::bearer_context_setup_resp:
      c.destroy<bearer_context_setup_resp_s>();
      break;
    case types::bearer_context_mod_resp:
      c.destroy<bearer_context_mod_resp_s>();
      break;
    case types::bearer_context_mod_confirm:
      c.destroy<bearer_context_mod_confirm_s>();
      break;
    case types::bearer_context_release_complete:
      c.destroy<bearer_context_release_complete_s>();
      break;
    default:
      break;
  }
}
void e1_ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::reset_ack:
      c.init<reset_ack_s>();
      break;
    case types::gnb_cu_up_e1_setup_resp:
      c.init<gnb_cu_up_e1_setup_resp_s>();
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      c.init<gnb_cu_cp_e1_setup_resp_s>();
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      c.init<gnb_cu_up_cfg_upd_ack_s>();
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      c.init<gnb_cu_cp_cfg_upd_ack_s>();
      break;
    case types::e1_release_resp:
      c.init<e1_release_resp_s>();
      break;
    case types::bearer_context_setup_resp:
      c.init<bearer_context_setup_resp_s>();
      break;
    case types::bearer_context_mod_resp:
      c.init<bearer_context_mod_resp_s>();
      break;
    case types::bearer_context_mod_confirm:
      c.init<bearer_context_mod_confirm_s>();
      break;
    case types::bearer_context_release_complete:
      c.init<bearer_context_release_complete_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::successful_outcome_c");
  }
}
e1_ap_elem_procs_o::successful_outcome_c::successful_outcome_c(const e1_ap_elem_procs_o::successful_outcome_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::reset_ack:
      c.init(other.c.get<reset_ack_s>());
      break;
    case types::gnb_cu_up_e1_setup_resp:
      c.init(other.c.get<gnb_cu_up_e1_setup_resp_s>());
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      c.init(other.c.get<gnb_cu_cp_e1_setup_resp_s>());
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      c.init(other.c.get<gnb_cu_up_cfg_upd_ack_s>());
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      c.init(other.c.get<gnb_cu_cp_cfg_upd_ack_s>());
      break;
    case types::e1_release_resp:
      c.init(other.c.get<e1_release_resp_s>());
      break;
    case types::bearer_context_setup_resp:
      c.init(other.c.get<bearer_context_setup_resp_s>());
      break;
    case types::bearer_context_mod_resp:
      c.init(other.c.get<bearer_context_mod_resp_s>());
      break;
    case types::bearer_context_mod_confirm:
      c.init(other.c.get<bearer_context_mod_confirm_s>());
      break;
    case types::bearer_context_release_complete:
      c.init(other.c.get<bearer_context_release_complete_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::successful_outcome_c");
  }
}
e1_ap_elem_procs_o::successful_outcome_c&
e1_ap_elem_procs_o::successful_outcome_c::operator=(const e1_ap_elem_procs_o::successful_outcome_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::reset_ack:
      c.set(other.c.get<reset_ack_s>());
      break;
    case types::gnb_cu_up_e1_setup_resp:
      c.set(other.c.get<gnb_cu_up_e1_setup_resp_s>());
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      c.set(other.c.get<gnb_cu_cp_e1_setup_resp_s>());
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      c.set(other.c.get<gnb_cu_up_cfg_upd_ack_s>());
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      c.set(other.c.get<gnb_cu_cp_cfg_upd_ack_s>());
      break;
    case types::e1_release_resp:
      c.set(other.c.get<e1_release_resp_s>());
      break;
    case types::bearer_context_setup_resp:
      c.set(other.c.get<bearer_context_setup_resp_s>());
      break;
    case types::bearer_context_mod_resp:
      c.set(other.c.get<bearer_context_mod_resp_s>());
      break;
    case types::bearer_context_mod_confirm:
      c.set(other.c.get<bearer_context_mod_confirm_s>());
      break;
    case types::bearer_context_release_complete:
      c.set(other.c.get<bearer_context_release_complete_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::successful_outcome_c");
  }

  return *this;
}
reset_ack_s& e1_ap_elem_procs_o::successful_outcome_c::reset_ack()
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
gnb_cu_up_e1_setup_resp_s& e1_ap_elem_procs_o::successful_outcome_c::gnb_cu_up_e1_setup_resp()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_resp_s>();
}
gnb_cu_cp_e1_setup_resp_s& e1_ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_e1_setup_resp()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_resp_s>();
}
gnb_cu_up_cfg_upd_ack_s& e1_ap_elem_procs_o::successful_outcome_c::gnb_cu_up_cfg_upd_ack()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_ack_s>();
}
gnb_cu_cp_cfg_upd_ack_s& e1_ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_cfg_upd_ack()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_ack_s>();
}
e1_release_resp_s& e1_ap_elem_procs_o::successful_outcome_c::e1_release_resp()
{
  assert_choice_type(types::e1_release_resp, type_, "SuccessfulOutcome");
  return c.get<e1_release_resp_s>();
}
bearer_context_setup_resp_s& e1_ap_elem_procs_o::successful_outcome_c::bearer_context_setup_resp()
{
  assert_choice_type(types::bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_setup_resp_s>();
}
bearer_context_mod_resp_s& e1_ap_elem_procs_o::successful_outcome_c::bearer_context_mod_resp()
{
  assert_choice_type(types::bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_resp_s>();
}
bearer_context_mod_confirm_s& e1_ap_elem_procs_o::successful_outcome_c::bearer_context_mod_confirm()
{
  assert_choice_type(types::bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_confirm_s>();
}
bearer_context_release_complete_s& e1_ap_elem_procs_o::successful_outcome_c::bearer_context_release_complete()
{
  assert_choice_type(types::bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<bearer_context_release_complete_s>();
}
const reset_ack_s& e1_ap_elem_procs_o::successful_outcome_c::reset_ack() const
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
const gnb_cu_up_e1_setup_resp_s& e1_ap_elem_procs_o::successful_outcome_c::gnb_cu_up_e1_setup_resp() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_resp_s>();
}
const gnb_cu_cp_e1_setup_resp_s& e1_ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_e1_setup_resp() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_resp_s>();
}
const gnb_cu_up_cfg_upd_ack_s& e1_ap_elem_procs_o::successful_outcome_c::gnb_cu_up_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_ack_s>();
}
const gnb_cu_cp_cfg_upd_ack_s& e1_ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_ack_s>();
}
const e1_release_resp_s& e1_ap_elem_procs_o::successful_outcome_c::e1_release_resp() const
{
  assert_choice_type(types::e1_release_resp, type_, "SuccessfulOutcome");
  return c.get<e1_release_resp_s>();
}
const bearer_context_setup_resp_s& e1_ap_elem_procs_o::successful_outcome_c::bearer_context_setup_resp() const
{
  assert_choice_type(types::bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_setup_resp_s>();
}
const bearer_context_mod_resp_s& e1_ap_elem_procs_o::successful_outcome_c::bearer_context_mod_resp() const
{
  assert_choice_type(types::bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_resp_s>();
}
const bearer_context_mod_confirm_s& e1_ap_elem_procs_o::successful_outcome_c::bearer_context_mod_confirm() const
{
  assert_choice_type(types::bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_confirm_s>();
}
const bearer_context_release_complete_s&
e1_ap_elem_procs_o::successful_outcome_c::bearer_context_release_complete() const
{
  assert_choice_type(types::bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<bearer_context_release_complete_s>();
}
void e1_ap_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::reset_ack:
      j.write_fieldname("ResetAcknowledge");
      c.get<reset_ack_s>().to_json(j);
      break;
    case types::gnb_cu_up_e1_setup_resp:
      j.write_fieldname("GNB-CU-UP-E1SetupResponse");
      c.get<gnb_cu_up_e1_setup_resp_s>().to_json(j);
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      j.write_fieldname("GNB-CU-CP-E1SetupResponse");
      c.get<gnb_cu_cp_e1_setup_resp_s>().to_json(j);
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      j.write_fieldname("GNB-CU-UP-ConfigurationUpdateAcknowledge");
      c.get<gnb_cu_up_cfg_upd_ack_s>().to_json(j);
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      j.write_fieldname("GNB-CU-CP-ConfigurationUpdateAcknowledge");
      c.get<gnb_cu_cp_cfg_upd_ack_s>().to_json(j);
      break;
    case types::e1_release_resp:
      j.write_fieldname("E1ReleaseResponse");
      c.get<e1_release_resp_s>().to_json(j);
      break;
    case types::bearer_context_setup_resp:
      j.write_fieldname("BearerContextSetupResponse");
      c.get<bearer_context_setup_resp_s>().to_json(j);
      break;
    case types::bearer_context_mod_resp:
      j.write_fieldname("BearerContextModificationResponse");
      c.get<bearer_context_mod_resp_s>().to_json(j);
      break;
    case types::bearer_context_mod_confirm:
      j.write_fieldname("BearerContextModificationConfirm");
      c.get<bearer_context_mod_confirm_s>().to_json(j);
      break;
    case types::bearer_context_release_complete:
      j.write_fieldname("BearerContextReleaseComplete");
      c.get<bearer_context_release_complete_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e1_ap_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset_ack:
      HANDLE_CODE(c.get<reset_ack_s>().pack(bref));
      break;
    case types::gnb_cu_up_e1_setup_resp:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_resp_s>().pack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_resp_s>().pack(bref));
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_ack_s>().pack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_ack_s>().pack(bref));
      break;
    case types::e1_release_resp:
      HANDLE_CODE(c.get<e1_release_resp_s>().pack(bref));
      break;
    case types::bearer_context_setup_resp:
      HANDLE_CODE(c.get<bearer_context_setup_resp_s>().pack(bref));
      break;
    case types::bearer_context_mod_resp:
      HANDLE_CODE(c.get<bearer_context_mod_resp_s>().pack(bref));
      break;
    case types::bearer_context_mod_confirm:
      HANDLE_CODE(c.get<bearer_context_mod_confirm_s>().pack(bref));
      break;
    case types::bearer_context_release_complete:
      HANDLE_CODE(c.get<bearer_context_release_complete_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1_ap_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset_ack:
      HANDLE_CODE(c.get<reset_ack_s>().unpack(bref));
      break;
    case types::gnb_cu_up_e1_setup_resp:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_resp_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_resp_s>().unpack(bref));
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::e1_release_resp:
      HANDLE_CODE(c.get<e1_release_resp_s>().unpack(bref));
      break;
    case types::bearer_context_setup_resp:
      HANDLE_CODE(c.get<bearer_context_setup_resp_s>().unpack(bref));
      break;
    case types::bearer_context_mod_resp:
      HANDLE_CODE(c.get<bearer_context_mod_resp_s>().unpack(bref));
      break;
    case types::bearer_context_mod_confirm:
      HANDLE_CODE(c.get<bearer_context_mod_confirm_s>().unpack(bref));
      break;
    case types::bearer_context_release_complete:
      HANDLE_CODE(c.get<bearer_context_release_complete_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1_ap_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* options[] = {"ResetAcknowledge",
                                  "GNB-CU-UP-E1SetupResponse",
                                  "GNB-CU-CP-E1SetupResponse",
                                  "GNB-CU-UP-ConfigurationUpdateAcknowledge",
                                  "GNB-CU-CP-ConfigurationUpdateAcknowledge",
                                  "E1ReleaseResponse",
                                  "BearerContextSetupResponse",
                                  "BearerContextModificationResponse",
                                  "BearerContextModificationConfirm",
                                  "BearerContextReleaseComplete"};
  return convert_enum_idx(options, 10, value, "e1_ap_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void e1_ap_elem_procs_o::unsuccessful_outcome_c::destroy_()
{
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      c.destroy<gnb_cu_up_e1_setup_fail_s>();
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      c.destroy<gnb_cu_cp_e1_setup_fail_s>();
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      c.destroy<gnb_cu_up_cfg_upd_fail_s>();
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      c.destroy<gnb_cu_cp_cfg_upd_fail_s>();
      break;
    case types::bearer_context_setup_fail:
      c.destroy<bearer_context_setup_fail_s>();
      break;
    case types::bearer_context_mod_fail:
      c.destroy<bearer_context_mod_fail_s>();
      break;
    default:
      break;
  }
}
void e1_ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      c.init<gnb_cu_up_e1_setup_fail_s>();
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      c.init<gnb_cu_cp_e1_setup_fail_s>();
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      c.init<gnb_cu_up_cfg_upd_fail_s>();
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      c.init<gnb_cu_cp_cfg_upd_fail_s>();
      break;
    case types::bearer_context_setup_fail:
      c.init<bearer_context_setup_fail_s>();
      break;
    case types::bearer_context_mod_fail:
      c.init<bearer_context_mod_fail_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
e1_ap_elem_procs_o::unsuccessful_outcome_c::unsuccessful_outcome_c(
    const e1_ap_elem_procs_o::unsuccessful_outcome_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      c.init(other.c.get<gnb_cu_up_e1_setup_fail_s>());
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      c.init(other.c.get<gnb_cu_cp_e1_setup_fail_s>());
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      c.init(other.c.get<gnb_cu_up_cfg_upd_fail_s>());
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      c.init(other.c.get<gnb_cu_cp_cfg_upd_fail_s>());
      break;
    case types::bearer_context_setup_fail:
      c.init(other.c.get<bearer_context_setup_fail_s>());
      break;
    case types::bearer_context_mod_fail:
      c.init(other.c.get<bearer_context_mod_fail_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
e1_ap_elem_procs_o::unsuccessful_outcome_c&
e1_ap_elem_procs_o::unsuccessful_outcome_c::operator=(const e1_ap_elem_procs_o::unsuccessful_outcome_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      c.set(other.c.get<gnb_cu_up_e1_setup_fail_s>());
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      c.set(other.c.get<gnb_cu_cp_e1_setup_fail_s>());
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      c.set(other.c.get<gnb_cu_up_cfg_upd_fail_s>());
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      c.set(other.c.get<gnb_cu_cp_cfg_upd_fail_s>());
      break;
    case types::bearer_context_setup_fail:
      c.set(other.c.get<bearer_context_setup_fail_s>());
      break;
    case types::bearer_context_mod_fail:
      c.set(other.c.get<bearer_context_mod_fail_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::unsuccessful_outcome_c");
  }

  return *this;
}
gnb_cu_up_e1_setup_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_e1_setup_fail()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_fail_s>();
}
gnb_cu_cp_e1_setup_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_e1_setup_fail()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_fail_s>();
}
gnb_cu_up_cfg_upd_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_cfg_upd_fail()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_fail_s>();
}
gnb_cu_cp_cfg_upd_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_cfg_upd_fail()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_fail_s>();
}
bearer_context_setup_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_setup_fail()
{
  assert_choice_type(types::bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_setup_fail_s>();
}
bearer_context_mod_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_mod_fail()
{
  assert_choice_type(types::bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_mod_fail_s>();
}
const gnb_cu_up_e1_setup_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_e1_setup_fail() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_fail_s>();
}
const gnb_cu_cp_e1_setup_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_e1_setup_fail() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_fail_s>();
}
const gnb_cu_up_cfg_upd_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_fail_s>();
}
const gnb_cu_cp_cfg_upd_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_fail_s>();
}
const bearer_context_setup_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_setup_fail() const
{
  assert_choice_type(types::bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_setup_fail_s>();
}
const bearer_context_mod_fail_s& e1_ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_mod_fail() const
{
  assert_choice_type(types::bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_mod_fail_s>();
}
void e1_ap_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      j.write_fieldname("GNB-CU-UP-E1SetupFailure");
      c.get<gnb_cu_up_e1_setup_fail_s>().to_json(j);
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      j.write_fieldname("GNB-CU-CP-E1SetupFailure");
      c.get<gnb_cu_cp_e1_setup_fail_s>().to_json(j);
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      j.write_fieldname("GNB-CU-UP-ConfigurationUpdateFailure");
      c.get<gnb_cu_up_cfg_upd_fail_s>().to_json(j);
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      j.write_fieldname("GNB-CU-CP-ConfigurationUpdateFailure");
      c.get<gnb_cu_cp_cfg_upd_fail_s>().to_json(j);
      break;
    case types::bearer_context_setup_fail:
      j.write_fieldname("BearerContextSetupFailure");
      c.get<bearer_context_setup_fail_s>().to_json(j);
      break;
    case types::bearer_context_mod_fail:
      j.write_fieldname("BearerContextModificationFailure");
      c.get<bearer_context_mod_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e1_ap_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_fail_s>().pack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_fail_s>().pack(bref));
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_fail_s>().pack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_fail_s>().pack(bref));
      break;
    case types::bearer_context_setup_fail:
      HANDLE_CODE(c.get<bearer_context_setup_fail_s>().pack(bref));
      break;
    case types::bearer_context_mod_fail:
      HANDLE_CODE(c.get<bearer_context_mod_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1_ap_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_fail_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_fail_s>().unpack(bref));
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::bearer_context_setup_fail:
      HANDLE_CODE(c.get<bearer_context_setup_fail_s>().unpack(bref));
      break;
    case types::bearer_context_mod_fail:
      HANDLE_CODE(c.get<bearer_context_mod_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1_ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* options[] = {"GNB-CU-UP-E1SetupFailure",
                                  "GNB-CU-CP-E1SetupFailure",
                                  "GNB-CU-UP-ConfigurationUpdateFailure",
                                  "GNB-CU-CP-ConfigurationUpdateFailure",
                                  "BearerContextSetupFailure",
                                  "BearerContextModificationFailure"};
  return convert_enum_idx(options, 6, value, "e1_ap_elem_procs_o::unsuccessful_outcome_c::types");
}

// InitiatingMessage ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e1_ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e1_ap_elem_procs_o::get_init_msg(proc_code);
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
  if (not e1_ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e1_ap_elem_procs_o::get_crit(proc_code);
  value     = e1_ap_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != e1_ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e1_ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e1_ap_elem_procs_o::get_successful_outcome(proc_code);
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
  if (not e1_ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e1_ap_elem_procs_o::get_crit(proc_code);
  value     = e1_ap_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != e1_ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e1_ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e1_ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
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
  if (not e1_ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e1_ap_elem_procs_o::get_crit(proc_code);
  value     = e1_ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != e1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// E1AP-PDU ::= CHOICE
void e1_ap_pdu_c::destroy_()
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
void e1_ap_pdu_c::set(types::options e)
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
      log_invalid_choice_id(type_, "e1_ap_pdu_c");
  }
}
e1_ap_pdu_c::e1_ap_pdu_c(const e1_ap_pdu_c& other)
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
      log_invalid_choice_id(type_, "e1_ap_pdu_c");
  }
}
e1_ap_pdu_c& e1_ap_pdu_c::operator=(const e1_ap_pdu_c& other)
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
      log_invalid_choice_id(type_, "e1_ap_pdu_c");
  }

  return *this;
}
init_msg_s& e1_ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& e1_ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& e1_ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
void e1_ap_pdu_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "e1_ap_pdu_c");
  }
  j.end_obj();
}
SRSASN_CODE e1_ap_pdu_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "e1_ap_pdu_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1_ap_pdu_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "e1_ap_pdu_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1_ap_pdu_c::types_opts::to_string() const
{
  static const char* options[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome"};
  return convert_enum_idx(options, 3, value, "e1_ap_pdu_c::types");
}
