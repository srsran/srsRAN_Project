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

#include "srsran/asn1/f1ap/f1ap_pdu_items.h"
#include <sstream>

using namespace asn1;
using namespace asn1::f1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// Associated-SCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t associated_scell_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {200};
  return map_enum_number(names, 1, idx, "id");
}
bool associated_scell_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 200 == id;
}
crit_e associated_scell_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 200) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
associated_scell_item_ies_o::value_c associated_scell_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 200) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e associated_scell_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 200) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void associated_scell_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Associated-SCell-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE associated_scell_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE associated_scell_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<associated_scell_item_ies_o>;

// BAP-Header-Rewriting-Added-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bap_hdr_rewriting_added_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {515};
  return map_enum_number(names, 1, idx, "id");
}
bool bap_hdr_rewriting_added_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 515 == id;
}
crit_e bap_hdr_rewriting_added_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 515) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bap_hdr_rewriting_added_list_item_ies_o::value_c bap_hdr_rewriting_added_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 515) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bap_hdr_rewriting_added_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 515) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bap_hdr_rewriting_added_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BAP-Header-Rewriting-Added-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bap_hdr_rewriting_added_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bap_hdr_rewriting_added_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bap_hdr_rewriting_added_list_item_ies_o>;

// BAP-Header-Rewriting-Removed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bap_hdr_rewriting_remd_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {642};
  return map_enum_number(names, 1, idx, "id");
}
bool bap_hdr_rewriting_remd_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 642 == id;
}
crit_e bap_hdr_rewriting_remd_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 642) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bap_hdr_rewriting_remd_list_item_ies_o::value_c bap_hdr_rewriting_remd_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 642) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bap_hdr_rewriting_remd_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 642) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bap_hdr_rewriting_remd_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BAP-Header-Rewriting-Removed-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bap_hdr_rewriting_remd_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bap_hdr_rewriting_remd_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bap_hdr_rewriting_remd_list_item_ies_o>;

// BH-Routing-Information-Added-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_routing_info_added_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {284};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_routing_info_added_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 284 == id;
}
crit_e bh_routing_info_added_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 284) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_routing_info_added_list_item_ies_o::value_c bh_routing_info_added_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 284) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_routing_info_added_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 284) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_routing_info_added_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BH-Routing-Information-Added-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_routing_info_added_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_routing_info_added_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// BH-Routing-Information-Removed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_routing_info_remd_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {286};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_routing_info_remd_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 286 == id;
}
crit_e bh_routing_info_remd_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 286) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_routing_info_remd_list_item_ies_o::value_c bh_routing_info_remd_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 286) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_routing_info_remd_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 286) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_routing_info_remd_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BH-Routing-Information-Removed-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_routing_info_remd_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_routing_info_remd_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_routing_info_added_list_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<bh_routing_info_remd_list_item_ies_o>;

// BHChannels-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_failed_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {268};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_failed_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 268 == id;
}
crit_e bh_chs_failed_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 268) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_failed_to_be_modified_item_ies_o::value_c bh_chs_failed_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 268) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_failed_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 268) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_failed_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-FailedToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_failed_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_failed_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_failed_to_be_modified_item_ies_o>;

// BHChannels-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_failed_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {278};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_failed_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 278 == id;
}
crit_e bh_chs_failed_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 278) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_failed_to_be_setup_item_ies_o::value_c bh_chs_failed_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 278) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_failed_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 278) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_failed_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-FailedToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_failed_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_failed_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_failed_to_be_setup_item_ies_o>;

// BHChannels-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_failed_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {270};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_failed_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 270 == id;
}
crit_e bh_chs_failed_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 270) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_failed_to_be_setup_mod_item_ies_o::value_c
bh_chs_failed_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 270) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_failed_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 270) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-FailedToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_failed_to_be_setup_mod_item_ies_o>;

// BHChannels-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {272};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 272 == id;
}
crit_e bh_chs_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 272) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_modified_item_ies_o::value_c bh_chs_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 272) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 272) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-Modified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_modified_item_ies_o>;

// BHChannels-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_required_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {276};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_required_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 276 == id;
}
crit_e bh_chs_required_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 276) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_required_to_be_released_item_ies_o::value_c
bh_chs_required_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 276) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_required_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 276) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_required_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-Required-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_required_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_required_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_required_to_be_released_item_ies_o>;

// BHChannels-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {261};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 261 == id;
}
crit_e bh_chs_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 261) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_setup_item_ies_o::value_c bh_chs_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 261) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 261) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_setup_item_ies_o>;

// BHChannels-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {274};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 274 == id;
}
crit_e bh_chs_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 274) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_setup_mod_item_ies_o::value_c bh_chs_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 274) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 274) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-SetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_setup_mod_item_ies_o>;

// BHChannels-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {262};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 262 == id;
}
crit_e bh_chs_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 262) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_to_be_modified_item_ies_o::value_c bh_chs_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 262) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 262) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-ToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_to_be_modified_item_ies_o>;

// BHChannels-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {264};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 264 == id;
}
crit_e bh_chs_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 264) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_to_be_released_item_ies_o::value_c bh_chs_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 264) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 264) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_to_be_released_item_ies_o>;

// BHChannels-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {259};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 259 == id;
}
crit_e bh_chs_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 259) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_to_be_setup_item_ies_o::value_c bh_chs_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 259) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 259) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_to_be_setup_item_ies_o>;

// BHChannels-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t bh_chs_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {266};
  return map_enum_number(names, 1, idx, "id");
}
bool bh_chs_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 266 == id;
}
crit_e bh_chs_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 266) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
bh_chs_to_be_setup_mod_item_ies_o::value_c bh_chs_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 266) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bh_chs_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 266) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void bh_chs_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BHChannels-ToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE bh_chs_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE bh_chs_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<bh_chs_to_be_setup_mod_item_ies_o>;

// Broadcast-To-Be-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_to_be_cancelled_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {149};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_to_be_cancelled_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 149 == id;
}
crit_e broadcast_to_be_cancelled_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 149) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_to_be_cancelled_list_item_ies_o::value_c
broadcast_to_be_cancelled_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 149) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_to_be_cancelled_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 149) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_to_be_cancelled_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Broadcast-To-Be-Cancelled-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_to_be_cancelled_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_to_be_cancelled_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<broadcast_to_be_cancelled_list_item_ies_o>;

// BroadcastMRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {471};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 471 == id;
}
crit_e broadcast_m_rbs_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 471) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_to_be_modified_item_ies_o::value_c
broadcast_m_rbs_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 471) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 471) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-ToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// BroadcastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {473};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 473 == id;
}
crit_e broadcast_m_rbs_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 473) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_to_be_released_item_ies_o::value_c
broadcast_m_rbs_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 473) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 473) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// BroadcastMRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {477};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 477 == id;
}
crit_e broadcast_m_rbs_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 477) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c
broadcast_m_rbs_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 477) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 477) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-ToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_to_be_released_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_to_be_setup_mod_item_ies_o>;

// BroadcastMRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_failed_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {459};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_failed_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 459 == id;
}
crit_e broadcast_m_rbs_failed_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 459) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c
broadcast_m_rbs_failed_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 459) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_failed_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 459) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-FailedToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// BroadcastMRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {463};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 463 == id;
}
crit_e broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 463) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c
broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 463) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 463) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-FailedToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// BroadcastMRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {465};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 465 == id;
}
crit_e broadcast_m_rbs_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 465) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_modified_item_ies_o::value_c broadcast_m_rbs_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 465) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 465) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-Modified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// BroadcastMRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {469};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 469 == id;
}
crit_e broadcast_m_rbs_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 469) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_setup_mod_item_ies_o::value_c broadcast_m_rbs_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 469) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 469) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-SetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_setup_mod_item_ies_o>;

// BroadcastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {475};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 475 == id;
}
crit_e broadcast_m_rbs_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 475) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_to_be_setup_item_ies_o::value_c broadcast_m_rbs_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 475) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 475) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_to_be_setup_item_ies_o>;

// BroadcastMRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_failed_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {461};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_failed_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 461 == id;
}
crit_e broadcast_m_rbs_failed_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 461) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c
broadcast_m_rbs_failed_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 461) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_failed_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 461) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-FailedToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// BroadcastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t broadcast_m_rbs_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {467};
  return map_enum_number(names, 1, idx, "id");
}
bool broadcast_m_rbs_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 467 == id;
}
crit_e broadcast_m_rbs_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 467) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
broadcast_m_rbs_setup_item_ies_o::value_c broadcast_m_rbs_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 467) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e broadcast_m_rbs_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 467) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void broadcast_m_rbs_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("BroadcastMRBs-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE broadcast_m_rbs_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE broadcast_m_rbs_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<broadcast_m_rbs_setup_item_ies_o>;

// Candidate-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t candidate_sp_cell_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {91};
  return map_enum_number(names, 1, idx, "id");
}
bool candidate_sp_cell_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 91 == id;
}
crit_e candidate_sp_cell_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 91) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
candidate_sp_cell_item_ies_o::value_c candidate_sp_cell_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 91) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e candidate_sp_cell_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 91) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void candidate_sp_cell_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Candidate-SpCell-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE candidate_sp_cell_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE candidate_sp_cell_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<candidate_sp_cell_item_ies_o>;

// Cells-Broadcast-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t cells_broadcast_cancelled_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {151};
  return map_enum_number(names, 1, idx, "id");
}
bool cells_broadcast_cancelled_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 151 == id;
}
crit_e cells_broadcast_cancelled_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 151) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cells_broadcast_cancelled_list_item_ies_o::value_c
cells_broadcast_cancelled_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 151) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cells_broadcast_cancelled_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 151) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cells_broadcast_cancelled_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cells-Broadcast-Cancelled-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cells_broadcast_cancelled_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_broadcast_cancelled_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<cells_broadcast_cancelled_list_item_ies_o>;

// Cells-Broadcast-Completed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t cells_broadcast_completed_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {147};
  return map_enum_number(names, 1, idx, "id");
}
bool cells_broadcast_completed_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 147 == id;
}
crit_e cells_broadcast_completed_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 147) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cells_broadcast_completed_list_item_ies_o::value_c
cells_broadcast_completed_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 147) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cells_broadcast_completed_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 147) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cells_broadcast_completed_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cells-Broadcast-Completed-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cells_broadcast_completed_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_broadcast_completed_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<cells_broadcast_completed_list_item_ies_o>;

// Cells-Failed-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t cells_failed_to_be_activ_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2};
  return map_enum_number(names, 1, idx, "id");
}
bool cells_failed_to_be_activ_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 2 == id;
}
crit_e cells_failed_to_be_activ_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 2) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cells_failed_to_be_activ_list_item_ies_o::value_c
cells_failed_to_be_activ_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 2) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cells_failed_to_be_activ_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 2) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cells_failed_to_be_activ_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cells-Failed-to-be-Activated-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cells_failed_to_be_activ_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_failed_to_be_activ_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<cells_failed_to_be_activ_list_item_ies_o>;

// Cells-Status-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t cells_status_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {88};
  return map_enum_number(names, 1, idx, "id");
}
bool cells_status_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 88 == id;
}
crit_e cells_status_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 88) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cells_status_item_ies_o::value_c cells_status_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 88) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cells_status_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 88) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cells_status_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cells-Status-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cells_status_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_status_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<cells_status_item_ies_o>;

// Cells-To-Be-Broadcast-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t cells_to_be_broadcast_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {145};
  return map_enum_number(names, 1, idx, "id");
}
bool cells_to_be_broadcast_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 145 == id;
}
crit_e cells_to_be_broadcast_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 145) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cells_to_be_broadcast_list_item_ies_o::value_c cells_to_be_broadcast_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 145) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cells_to_be_broadcast_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 145) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cells_to_be_broadcast_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cells-To-Be-Broadcast-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cells_to_be_broadcast_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_be_broadcast_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<cells_to_be_broadcast_list_item_ies_o>;

// Cells-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t cells_to_be_activ_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {4};
  return map_enum_number(names, 1, idx, "id");
}
bool cells_to_be_activ_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 4 == id;
}
crit_e cells_to_be_activ_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 4) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cells_to_be_activ_list_item_ies_o::value_c cells_to_be_activ_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 4) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cells_to_be_activ_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 4) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cells_to_be_activ_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cells-to-be-Activated-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cells_to_be_activ_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_be_activ_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>;

// Cells-to-be-Barred-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t cells_to_be_barred_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {130};
  return map_enum_number(names, 1, idx, "id");
}
bool cells_to_be_barred_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 130 == id;
}
crit_e cells_to_be_barred_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 130) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cells_to_be_barred_item_ies_o::value_c cells_to_be_barred_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 130) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cells_to_be_barred_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 130) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cells_to_be_barred_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cells-to-be-Barred-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cells_to_be_barred_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_be_barred_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<cells_to_be_barred_item_ies_o>;

// Cells-to-be-Deactivated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t cells_to_be_deactiv_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {6};
  return map_enum_number(names, 1, idx, "id");
}
bool cells_to_be_deactiv_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 6 == id;
}
crit_e cells_to_be_deactiv_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 6) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
cells_to_be_deactiv_list_item_ies_o::value_c cells_to_be_deactiv_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 6) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cells_to_be_deactiv_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 6) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void cells_to_be_deactiv_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Cells-to-be-Deactivated-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE cells_to_be_deactiv_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_to_be_deactiv_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<cells_to_be_deactiv_list_item_ies_o>;

// DL-UP-TNL-Address-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t dl_up_tnl_address_to_upd_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {305};
  return map_enum_number(names, 1, idx, "id");
}
bool dl_up_tnl_address_to_upd_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 305 == id;
}
crit_e dl_up_tnl_address_to_upd_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 305) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
dl_up_tnl_address_to_upd_list_item_ies_o::value_c
dl_up_tnl_address_to_upd_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 305) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e dl_up_tnl_address_to_upd_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 305) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void dl_up_tnl_address_to_upd_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DL-UP-TNL-Address-to-Update-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE dl_up_tnl_address_to_upd_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_up_tnl_address_to_upd_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<dl_up_tnl_address_to_upd_list_item_ies_o>;

// DRB-Activity-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drb_activity_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {99};
  return map_enum_number(names, 1, idx, "id");
}
bool drb_activity_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 99 == id;
}
crit_e drb_activity_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 99) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drb_activity_item_ies_o::value_c drb_activity_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 99) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_activity_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 99) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drb_activity_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRB-Activity-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drb_activity_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_activity_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drb_activity_item_ies_o>;

// DRB-Notify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drb_notify_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {136};
  return map_enum_number(names, 1, idx, "id");
}
bool drb_notify_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 136 == id;
}
crit_e drb_notify_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 136) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drb_notify_item_ies_o::value_c drb_notify_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 136) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drb_notify_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 136) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drb_notify_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRB-Notify-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drb_notify_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_notify_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drb_notify_item_ies_o>;

// DRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_failed_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {12};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_failed_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 12 == id;
}
crit_e drbs_failed_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 12) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_failed_to_be_modified_item_ies_o::value_c drbs_failed_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 12) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_failed_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 12) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_failed_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-FailedToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_failed_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_failed_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_failed_to_be_modified_item_ies_o>;

// DRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_failed_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {14};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_failed_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 14 == id;
}
crit_e drbs_failed_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 14) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_failed_to_be_setup_item_ies_o::value_c drbs_failed_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 14) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_failed_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 14) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_failed_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-FailedToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_failed_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_failed_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_failed_to_be_setup_item_ies_o>;

// DRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_failed_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {16};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_failed_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 16 == id;
}
crit_e drbs_failed_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 16) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_failed_to_be_setup_mod_item_ies_o::value_c drbs_failed_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 16) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_failed_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 16) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_failed_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-FailedToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_failed_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_failed_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_failed_to_be_setup_mod_item_ies_o>;

// DRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {20};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 20 == id;
}
crit_e drbs_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 20) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_modified_item_ies_o::value_c drbs_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 20) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 20) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-Modified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_modified_item_ies_o>;

// DRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_modified_conf_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {18};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_modified_conf_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 18 == id;
}
crit_e drbs_modified_conf_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 18) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_modified_conf_item_ies_o::value_c drbs_modified_conf_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 18) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_modified_conf_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 18) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_modified_conf_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-ModifiedConf-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_modified_conf_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_modified_conf_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_modified_conf_item_ies_o>;

// DRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_required_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {22};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_required_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 22 == id;
}
crit_e drbs_required_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 22) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_required_to_be_modified_item_ies_o::value_c drbs_required_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 22) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_required_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 22) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_required_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-Required-ToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_required_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_required_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_required_to_be_modified_item_ies_o>;

// DRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_required_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {24};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_required_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 24 == id;
}
crit_e drbs_required_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 24) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_required_to_be_released_item_ies_o::value_c drbs_required_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 24) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_required_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 24) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_required_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-Required-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_required_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_required_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_required_to_be_released_item_ies_o>;

// DRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {26};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 26 == id;
}
crit_e drbs_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 26) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_setup_item_ies_o::value_c drbs_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 26) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 26) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_setup_item_ies_o>;

// DRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {28};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 28 == id;
}
crit_e drbs_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 28) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_setup_mod_item_ies_o::value_c drbs_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 28) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 28) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-SetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_setup_mod_item_ies_o>;

// DRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {30};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 30 == id;
}
crit_e drbs_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 30) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_to_be_modified_item_ies_o::value_c drbs_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 30) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 30) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-ToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_to_be_modified_item_ies_o>;

// DRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {32};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 32 == id;
}
crit_e drbs_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 32) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_to_be_released_item_ies_o::value_c drbs_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 32) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 32) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_to_be_released_item_ies_o>;

// DRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {34};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 34 == id;
}
crit_e drbs_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 34) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_to_be_setup_item_ies_o::value_c drbs_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 34) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 34) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_to_be_setup_item_ies_o>;

// DRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t drbs_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {36};
  return map_enum_number(names, 1, idx, "id");
}
bool drbs_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 36 == id;
}
crit_e drbs_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 36) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
drbs_to_be_setup_mod_item_ies_o::value_c drbs_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 36) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e drbs_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 36) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void drbs_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("DRBs-ToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE drbs_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE drbs_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<drbs_to_be_setup_mod_item_ies_o>;

// Dedicated-SIDelivery-NeededUE-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ded_si_delivery_needed_ue_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {190};
  return map_enum_number(names, 1, idx, "id");
}
bool ded_si_delivery_needed_ue_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 190 == id;
}
crit_e ded_si_delivery_needed_ue_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 190) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ded_si_delivery_needed_ue_item_ies_o::value_c ded_si_delivery_needed_ue_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 190) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ded_si_delivery_needed_ue_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 190) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ded_si_delivery_needed_ue_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Dedicated-SIDelivery-NeededUE-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ded_si_delivery_needed_ue_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ded_si_delivery_needed_ue_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>;

// GNB-CU-TNL-Association-Failed-To-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {135};
  return map_enum_number(names, 1, idx, "id");
}
bool gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 135 == id;
}
crit_e gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 135) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c
gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 135) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 135) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("GNB-CU-TNL-Association-Failed-To-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// GNB-CU-TNL-Association-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t gnb_cu_tnl_assoc_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {133};
  return map_enum_number(names, 1, idx, "id");
}
bool gnb_cu_tnl_assoc_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 133 == id;
}
crit_e gnb_cu_tnl_assoc_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 133) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_cu_tnl_assoc_setup_item_ies_o::value_c gnb_cu_tnl_assoc_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 133) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_tnl_assoc_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 133) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_tnl_assoc_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("GNB-CU-TNL-Association-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_cu_tnl_assoc_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_tnl_assoc_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// GNB-CU-TNL-Association-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t gnb_cu_tnl_assoc_to_add_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {120};
  return map_enum_number(names, 1, idx, "id");
}
bool gnb_cu_tnl_assoc_to_add_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 120 == id;
}
crit_e gnb_cu_tnl_assoc_to_add_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 120) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_cu_tnl_assoc_to_add_item_ies_o::value_c gnb_cu_tnl_assoc_to_add_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 120) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_tnl_assoc_to_add_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 120) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("GNB-CU-TNL-Association-To-Add-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// GNB-CU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t gnb_cu_tnl_assoc_to_rem_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {122};
  return map_enum_number(names, 1, idx, "id");
}
bool gnb_cu_tnl_assoc_to_rem_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 122 == id;
}
crit_e gnb_cu_tnl_assoc_to_rem_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 122) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c gnb_cu_tnl_assoc_to_rem_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 122) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_tnl_assoc_to_rem_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 122) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("GNB-CU-TNL-Association-To-Remove-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// GNB-CU-TNL-Association-To-Update-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t gnb_cu_tnl_assoc_to_upd_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {124};
  return map_enum_number(names, 1, idx, "id");
}
bool gnb_cu_tnl_assoc_to_upd_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 124 == id;
}
crit_e gnb_cu_tnl_assoc_to_upd_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 124) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c gnb_cu_tnl_assoc_to_upd_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 124) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_tnl_assoc_to_upd_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 124) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("GNB-CU-TNL-Association-To-Update-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// GNB-DU-Served-Cells-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t gnb_du_served_cells_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {43};
  return map_enum_number(names, 1, idx, "id");
}
bool gnb_du_served_cells_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 43 == id;
}
crit_e gnb_du_served_cells_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 43) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_du_served_cells_item_ies_o::value_c gnb_du_served_cells_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 43) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_du_served_cells_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 43) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void gnb_du_served_cells_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("GNB-DU-Served-Cells-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_du_served_cells_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_du_served_cells_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// GNB-DU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t gnb_du_tnl_assoc_to_rem_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {227};
  return map_enum_number(names, 1, idx, "id");
}
bool gnb_du_tnl_assoc_to_rem_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 227 == id;
}
crit_e gnb_du_tnl_assoc_to_rem_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 227) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
gnb_du_tnl_assoc_to_rem_item_ies_o::value_c gnb_du_tnl_assoc_to_rem_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 227) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_du_tnl_assoc_to_rem_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 227) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("GNB-DU-TNL-Association-To-Remove-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// IAB-Allocated-TNL-Address-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t iab_allocated_tnl_address_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {295};
  return map_enum_number(names, 1, idx, "id");
}
bool iab_allocated_tnl_address_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 295 == id;
}
crit_e iab_allocated_tnl_address_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 295) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
iab_allocated_tnl_address_list_item_ies_o::value_c
iab_allocated_tnl_address_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 295) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e iab_allocated_tnl_address_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 295) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void iab_allocated_tnl_address_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("IAB-Allocated-TNL-Address-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE iab_allocated_tnl_address_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_allocated_tnl_address_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// IAB-TNL-Addresses-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t iab_tnl_addresses_to_rem_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {293};
  return map_enum_number(names, 1, idx, "id");
}
bool iab_tnl_addresses_to_rem_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 293 == id;
}
crit_e iab_tnl_addresses_to_rem_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 293) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
iab_tnl_addresses_to_rem_item_ies_o::value_c iab_tnl_addresses_to_rem_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 293) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e iab_tnl_addresses_to_rem_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 293) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void iab_tnl_addresses_to_rem_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("IAB-TNL-Addresses-To-Remove-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE iab_tnl_addresses_to_rem_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_tnl_addresses_to_rem_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MC-PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t mc_paging_cell_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {688};
  return map_enum_number(names, 1, idx, "id");
}
bool mc_paging_cell_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 688 == id;
}
crit_e mc_paging_cell_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 688) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
mc_paging_cell_item_ies_o::value_c mc_paging_cell_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 688) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e mc_paging_cell_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 688) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void mc_paging_cell_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MC-PagingCell-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE mc_paging_cell_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE mc_paging_cell_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastF1UContext-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_f1_u_context_failed_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {508};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_f1_u_context_failed_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 508 == id;
}
crit_e multicast_f1_u_context_failed_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 508) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c
multicast_f1_u_context_failed_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 508) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_f1_u_context_failed_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 508) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastF1UContext-FailedToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastF1UContext-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_f1_u_context_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {506};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_f1_u_context_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 506 == id;
}
crit_e multicast_f1_u_context_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 506) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_f1_u_context_setup_item_ies_o::value_c multicast_f1_u_context_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 506) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_f1_u_context_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 506) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_f1_u_context_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastF1UContext-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_f1_u_context_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_f1_u_context_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastF1UContext-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_f1_u_context_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {504};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_f1_u_context_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 504 == id;
}
crit_e multicast_f1_u_context_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 504) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_f1_u_context_to_be_setup_item_ies_o::value_c
multicast_f1_u_context_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 504) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_f1_u_context_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 504) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_f1_u_context_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastF1UContext-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_f1_u_context_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_f1_u_context_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_failed_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {483};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_failed_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 483 == id;
}
crit_e multicast_m_rbs_failed_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 483) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c
multicast_m_rbs_failed_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 483) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_failed_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 483) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-FailedToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_failed_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {485};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_failed_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 485 == id;
}
crit_e multicast_m_rbs_failed_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 485) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c
multicast_m_rbs_failed_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 485) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_failed_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 485) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-FailedToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {487};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 487 == id;
}
crit_e multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 487) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c
multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 487) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 487) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-FailedToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {489};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 489 == id;
}
crit_e multicast_m_rbs_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 489) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_modified_item_ies_o::value_c multicast_m_rbs_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 489) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 489) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-Modified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {491};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 491 == id;
}
crit_e multicast_m_rbs_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 491) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_setup_item_ies_o::value_c multicast_m_rbs_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 491) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 491) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {493};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 493 == id;
}
crit_e multicast_m_rbs_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 493) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_setup_mod_item_ies_o::value_c multicast_m_rbs_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 493) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 493) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-SetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {495};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 495 == id;
}
crit_e multicast_m_rbs_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 495) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_to_be_modified_item_ies_o::value_c
multicast_m_rbs_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 495) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 495) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-ToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {497};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 497 == id;
}
crit_e multicast_m_rbs_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 497) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_to_be_released_item_ies_o::value_c
multicast_m_rbs_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 497) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 497) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {499};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 499 == id;
}
crit_e multicast_m_rbs_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 499) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_to_be_setup_item_ies_o::value_c multicast_m_rbs_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 499) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 499) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// MulticastMRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t multicast_m_rbs_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {501};
  return map_enum_number(names, 1, idx, "id");
}
bool multicast_m_rbs_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 501 == id;
}
crit_e multicast_m_rbs_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 501) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c
multicast_m_rbs_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 501) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e multicast_m_rbs_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 501) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("MulticastMRBs-ToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// NR-CGI-List-For-Restart-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t nr_cgi_list_for_restart_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {153};
  return map_enum_number(names, 1, idx, "id");
}
bool nr_cgi_list_for_restart_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 153 == id;
}
crit_e nr_cgi_list_for_restart_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 153) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
nr_cgi_list_for_restart_list_item_ies_o::value_c nr_cgi_list_for_restart_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 153) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e nr_cgi_list_for_restart_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 153) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void nr_cgi_list_for_restart_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("NR-CGI-List-For-Restart-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE nr_cgi_list_for_restart_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_cgi_list_for_restart_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// Neighbour-Cell-Information-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t neighbour_cell_info_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {255};
  return map_enum_number(names, 1, idx, "id");
}
bool neighbour_cell_info_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 255 == id;
}
crit_e neighbour_cell_info_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 255) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
neighbour_cell_info_item_ies_o::value_c neighbour_cell_info_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 255) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e neighbour_cell_info_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 255) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void neighbour_cell_info_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Neighbour-Cell-Information-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE neighbour_cell_info_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE neighbour_cell_info_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// PWS-Failed-NR-CGI-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t pws_failed_nr_cgi_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {155};
  return map_enum_number(names, 1, idx, "id");
}
bool pws_failed_nr_cgi_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 155 == id;
}
crit_e pws_failed_nr_cgi_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 155) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
pws_failed_nr_cgi_list_item_ies_o::value_c pws_failed_nr_cgi_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 155) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e pws_failed_nr_cgi_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 155) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void pws_failed_nr_cgi_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("PWS-Failed-NR-CGI-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE pws_failed_nr_cgi_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE pws_failed_nr_cgi_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t paging_cell_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {112};
  return map_enum_number(names, 1, idx, "id");
}
bool paging_cell_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 112 == id;
}
crit_e paging_cell_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 112) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
paging_cell_item_ies_o::value_c paging_cell_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 112) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e paging_cell_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 112) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void paging_cell_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("PagingCell-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE paging_cell_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_cell_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// Potential-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t potential_sp_cell_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {93};
  return map_enum_number(names, 1, idx, "id");
}
bool potential_sp_cell_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 93 == id;
}
crit_e potential_sp_cell_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 93) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
potential_sp_cell_item_ies_o::value_c potential_sp_cell_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 93) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e potential_sp_cell_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 93) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void potential_sp_cell_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Potential-SpCell-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE potential_sp_cell_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE potential_sp_cell_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// Protected-EUTRA-Resources-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t protected_eutra_res_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {168};
  return map_enum_number(names, 1, idx, "id");
}
bool protected_eutra_res_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 168 == id;
}
crit_e protected_eutra_res_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 168) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
protected_eutra_res_item_ies_o::value_c protected_eutra_res_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 168) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e protected_eutra_res_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 168) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void protected_eutra_res_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Protected-EUTRA-Resources-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE protected_eutra_res_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE protected_eutra_res_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SCell-FailedtoSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t scell_failedto_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {84};
  return map_enum_number(names, 1, idx, "id");
}
bool scell_failedto_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 84 == id;
}
crit_e scell_failedto_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 84) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
scell_failedto_setup_item_ies_o::value_c scell_failedto_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 84) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e scell_failedto_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 84) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void scell_failedto_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SCell-FailedtoSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE scell_failedto_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_failedto_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SCell-FailedtoSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t scell_failedto_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {86};
  return map_enum_number(names, 1, idx, "id");
}
bool scell_failedto_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 86 == id;
}
crit_e scell_failedto_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 86) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
scell_failedto_setup_mod_item_ies_o::value_c scell_failedto_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 86) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e scell_failedto_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 86) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void scell_failedto_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SCell-FailedtoSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE scell_failedto_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_failedto_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SCell-ToBeRemoved-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t scell_to_be_remd_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {51};
  return map_enum_number(names, 1, idx, "id");
}
bool scell_to_be_remd_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 51 == id;
}
crit_e scell_to_be_remd_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 51) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
scell_to_be_remd_item_ies_o::value_c scell_to_be_remd_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 51) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e scell_to_be_remd_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 51) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void scell_to_be_remd_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SCell-ToBeRemoved-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE scell_to_be_remd_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_be_remd_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SCell-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t scell_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {53};
  return map_enum_number(names, 1, idx, "id");
}
bool scell_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 53 == id;
}
crit_e scell_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 53) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
scell_to_be_setup_item_ies_o::value_c scell_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 53) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e scell_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 53) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void scell_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SCell-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE scell_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SCell-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t scell_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {55};
  return map_enum_number(names, 1, idx, "id");
}
bool scell_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 55 == id;
}
crit_e scell_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 55) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
scell_to_be_setup_mod_item_ies_o::value_c scell_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 55) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e scell_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 55) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void scell_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SCell-ToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE scell_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_failed_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {313};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_failed_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 313 == id;
}
crit_e sl_drbs_failed_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 313) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_failed_to_be_modified_item_ies_o::value_c
sl_drbs_failed_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 313) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_failed_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 313) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_failed_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-FailedToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_failed_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_failed_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_failed_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {315};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_failed_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 315 == id;
}
crit_e sl_drbs_failed_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 315) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_failed_to_be_setup_item_ies_o::value_c sl_drbs_failed_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 315) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_failed_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 315) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_failed_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-FailedToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_failed_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_failed_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_failed_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {336};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_failed_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 336 == id;
}
crit_e sl_drbs_failed_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 336) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c
sl_drbs_failed_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 336) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_failed_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 336) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-FailedToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {317};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 317 == id;
}
crit_e sl_drbs_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 317) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_modified_item_ies_o::value_c sl_drbs_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 317) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 317) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-Modified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_modified_conf_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {338};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_modified_conf_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 338 == id;
}
crit_e sl_drbs_modified_conf_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 338) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_modified_conf_item_ies_o::value_c sl_drbs_modified_conf_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 338) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_modified_conf_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 338) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_modified_conf_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-ModifiedConf-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_modified_conf_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_modified_conf_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_required_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {319};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_required_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 319 == id;
}
crit_e sl_drbs_required_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 319) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_required_to_be_modified_item_ies_o::value_c
sl_drbs_required_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 319) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_required_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 319) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_required_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-Required-ToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_required_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_required_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_required_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {321};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_required_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 321 == id;
}
crit_e sl_drbs_required_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 321) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_required_to_be_released_item_ies_o::value_c
sl_drbs_required_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 321) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_required_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 321) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_required_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-Required-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_required_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_required_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {323};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 323 == id;
}
crit_e sl_drbs_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 323) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_setup_item_ies_o::value_c sl_drbs_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 323) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 323) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {335};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 335 == id;
}
crit_e sl_drbs_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 335) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_setup_mod_item_ies_o::value_c sl_drbs_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 335) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 335) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-SetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {325};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 325 == id;
}
crit_e sl_drbs_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 325) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_to_be_modified_item_ies_o::value_c sl_drbs_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 325) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 325) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-ToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {327};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 327 == id;
}
crit_e sl_drbs_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 327) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_to_be_released_item_ies_o::value_c sl_drbs_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 327) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 327) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {329};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 329 == id;
}
crit_e sl_drbs_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 329) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_to_be_setup_item_ies_o::value_c sl_drbs_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 329) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 329) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SLDRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t sl_drbs_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {331};
  return map_enum_number(names, 1, idx, "id");
}
bool sl_drbs_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 331 == id;
}
crit_e sl_drbs_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 331) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
sl_drbs_to_be_setup_mod_item_ies_o::value_c sl_drbs_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 331) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e sl_drbs_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 331) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void sl_drbs_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SLDRBs-ToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sl_drbs_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_drbs_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_failed_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {65};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_failed_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 65 == id;
}
crit_e srbs_failed_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 65) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_failed_to_be_setup_item_ies_o::value_c srbs_failed_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 65) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_failed_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 65) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_failed_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-FailedToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_failed_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_failed_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_failed_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {67};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_failed_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 67 == id;
}
crit_e srbs_failed_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 67) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_failed_to_be_setup_mod_item_ies_o::value_c srbs_failed_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 67) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_failed_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 67) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_failed_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-FailedToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_failed_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_failed_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {207};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 207 == id;
}
crit_e srbs_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 207) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_modified_item_ies_o::value_c srbs_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 207) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 207) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-Modified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_required_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {69};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_required_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 69 == id;
}
crit_e srbs_required_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 69) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_required_to_be_released_item_ies_o::value_c srbs_required_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 69) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_required_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 69) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_required_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-Required-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_required_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_required_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {203};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 203 == id;
}
crit_e srbs_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 203) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_setup_item_ies_o::value_c srbs_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 203) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 203) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {205};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 205 == id;
}
crit_e srbs_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 205) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_setup_mod_item_ies_o::value_c srbs_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 205) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 205) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-SetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {71};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 71 == id;
}
crit_e srbs_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 71) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_to_be_released_item_ies_o::value_c srbs_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 71) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 71) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {73};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 73 == id;
}
crit_e srbs_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 73) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_to_be_setup_item_ies_o::value_c srbs_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 73) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 73) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// SRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t srbs_to_be_setup_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {75};
  return map_enum_number(names, 1, idx, "id");
}
bool srbs_to_be_setup_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 75 == id;
}
crit_e srbs_to_be_setup_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 75) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
srbs_to_be_setup_mod_item_ies_o::value_c srbs_to_be_setup_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 75) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e srbs_to_be_setup_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 75) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void srbs_to_be_setup_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("SRBs-ToBeSetupMod-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE srbs_to_be_setup_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE srbs_to_be_setup_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// Served-Cells-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t served_cells_to_add_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57};
  return map_enum_number(names, 1, idx, "id");
}
bool served_cells_to_add_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 57 == id;
}
crit_e served_cells_to_add_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 57) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
served_cells_to_add_item_ies_o::value_c served_cells_to_add_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 57) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e served_cells_to_add_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 57) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void served_cells_to_add_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Served-Cells-To-Add-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE served_cells_to_add_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE served_cells_to_add_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// Served-Cells-To-Delete-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t served_cells_to_delete_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {59};
  return map_enum_number(names, 1, idx, "id");
}
bool served_cells_to_delete_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 59 == id;
}
crit_e served_cells_to_delete_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 59) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
served_cells_to_delete_item_ies_o::value_c served_cells_to_delete_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 59) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e served_cells_to_delete_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 59) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void served_cells_to_delete_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Served-Cells-To-Delete-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE served_cells_to_delete_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE served_cells_to_delete_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// Served-Cells-To-Modify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t served_cells_to_modify_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {61};
  return map_enum_number(names, 1, idx, "id");
}
bool served_cells_to_modify_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 61 == id;
}
crit_e served_cells_to_modify_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 61) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
served_cells_to_modify_item_ies_o::value_c served_cells_to_modify_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 61) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e served_cells_to_modify_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 61) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void served_cells_to_modify_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("Served-Cells-To-Modify-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE served_cells_to_modify_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE served_cells_to_modify_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// ServingCellMO-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t serving_cell_mo_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {696};
  return map_enum_number(names, 1, idx, "id");
}
bool serving_cell_mo_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 696 == id;
}
crit_e serving_cell_mo_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 696) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
serving_cell_mo_list_item_ies_o::value_c serving_cell_mo_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 696) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e serving_cell_mo_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 696) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void serving_cell_mo_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ServingCellMO-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE serving_cell_mo_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_mo_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UE-MulticastMRBs-ConfirmedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {654};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 654 == id;
}
crit_e ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 654) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c
ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 654) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 654) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-MulticastMRBs-ConfirmedToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UE-MulticastMRBs-RequiredToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_multicast_m_rbs_required_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {656};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_multicast_m_rbs_required_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 656 == id;
}
crit_e ue_multicast_m_rbs_required_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 656) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c
ue_multicast_m_rbs_required_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 656) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_multicast_m_rbs_required_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 656) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-MulticastMRBs-RequiredToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UE-MulticastMRBs-RequiredToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_multicast_m_rbs_required_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {658};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_multicast_m_rbs_required_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 658 == id;
}
crit_e ue_multicast_m_rbs_required_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 658) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c
ue_multicast_m_rbs_required_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 658) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_multicast_m_rbs_required_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 658) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-MulticastMRBs-RequiredToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UE-MulticastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_multicast_m_rbs_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {680};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_multicast_m_rbs_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 680 == id;
}
crit_e ue_multicast_m_rbs_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 680) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_multicast_m_rbs_setup_item_ies_o::value_c ue_multicast_m_rbs_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 680) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_multicast_m_rbs_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 680) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_multicast_m_rbs_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-MulticastMRBs-Setup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_multicast_m_rbs_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_multicast_m_rbs_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UE-MulticastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_multicast_m_rbs_to_be_released_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {629};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_multicast_m_rbs_to_be_released_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 629 == id;
}
crit_e ue_multicast_m_rbs_to_be_released_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 629) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_multicast_m_rbs_to_be_released_item_ies_o::value_c
ue_multicast_m_rbs_to_be_released_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 629) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_multicast_m_rbs_to_be_released_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 629) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-MulticastMRBs-ToBeReleased-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UE-MulticastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_multicast_m_rbs_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {631};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_multicast_m_rbs_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 631 == id;
}
crit_e ue_multicast_m_rbs_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 631) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c
ue_multicast_m_rbs_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 631) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_multicast_m_rbs_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 631) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-MulticastMRBs-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UE-MulticastMRBs-ToBeSetup-atModify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {686};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 686 == id;
}
crit_e ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 686) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c
ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 686) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 686) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-MulticastMRBs-ToBeSetup-atModify-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UEIdentity-List-For-Paging-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_id_list_for_paging_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {480};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_id_list_for_paging_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 480 == id;
}
crit_e ue_id_list_for_paging_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 480) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_id_list_for_paging_item_ies_o::value_c ue_id_list_for_paging_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 480) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_id_list_for_paging_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 480) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_id_list_for_paging_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UEIdentity-List-For-Paging-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_id_list_for_paging_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_id_list_for_paging_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UL-UP-TNL-Address-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ul_up_tnl_address_to_upd_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {303};
  return map_enum_number(names, 1, idx, "id");
}
bool ul_up_tnl_address_to_upd_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 303 == id;
}
crit_e ul_up_tnl_address_to_upd_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 303) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ul_up_tnl_address_to_upd_list_item_ies_o::value_c
ul_up_tnl_address_to_upd_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 303) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ul_up_tnl_address_to_upd_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 303) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ul_up_tnl_address_to_upd_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UL-UP-TNL-Address-to-Update-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ul_up_tnl_address_to_upd_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_up_tnl_address_to_upd_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

// UL-UP-TNL-Information-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ul_up_tnl_info_to_upd_list_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {301};
  return map_enum_number(names, 1, idx, "id");
}
bool ul_up_tnl_info_to_upd_list_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 301 == id;
}
crit_e ul_up_tnl_info_to_upd_list_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 301) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ul_up_tnl_info_to_upd_list_item_ies_o::value_c ul_up_tnl_info_to_upd_list_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 301) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ul_up_tnl_info_to_upd_list_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 301) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ul_up_tnl_info_to_upd_list_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UL-UP-TNL-Information-to-Update-List-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ul_up_tnl_info_to_upd_list_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_up_tnl_info_to_upd_list_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

template struct asn1::protocol_ie_single_container_s<gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<gnb_cu_tnl_assoc_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_add_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_rem_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_upd_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<gnb_du_served_cells_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<gnb_du_tnl_assoc_to_rem_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<iab_allocated_tnl_address_list_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<iab_tnl_addresses_to_rem_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_f1_u_context_failed_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_f1_u_context_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_f1_u_context_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_to_be_released_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<multicast_m_rbs_to_be_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<nr_cgi_list_for_restart_list_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<neighbour_cell_info_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<pws_failed_nr_cgi_list_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<potential_sp_cell_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<protected_eutra_res_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<scell_failedto_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<scell_failedto_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<scell_to_be_remd_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<scell_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<scell_to_be_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_failed_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_failed_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_failed_to_be_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_modified_conf_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_required_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_required_to_be_released_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_to_be_released_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<sl_drbs_to_be_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_failed_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_failed_to_be_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_required_to_be_released_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_to_be_released_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<srbs_to_be_setup_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<served_cells_to_add_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<served_cells_to_delete_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<served_cells_to_modify_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<serving_cell_mo_list_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ue_multicast_m_rbs_required_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ue_multicast_m_rbs_required_to_be_released_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ue_multicast_m_rbs_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_released_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_setup_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ue_id_list_for_paging_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ul_up_tnl_address_to_upd_list_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ul_up_tnl_info_to_upd_list_item_ies_o>;
