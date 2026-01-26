/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/asn1/xnap/xnap.h"
#include "srsran/asn1/xnap/xnap_pdu_contents.h"
using namespace asn1;
using namespace asn1::xnap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// ProtocolIE-FieldPair{XNAP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{XNAP-PROTOCOL-IES-PAIR}}
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

// XNAP-ELEMENTARY-PROCEDURES-CLASS-1 ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
uint16_t xnap_elem_procs_class_1_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {0, 3, 7, 9, 10, 11, 12, 14, 16, 17, 18, 25, 19, 20, 34, 36, 44, 45, 46, 49};
  return map_enum_number(names, 20, idx, "proc_code");
}
bool xnap_elem_procs_class_1_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {0, 3, 7, 9, 10, 11, 12, 14, 16, 17, 18, 25, 19, 20, 34, 36, 44, 45, 46, 49};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
xnap_elem_procs_class_1_o::init_msg_c xnap_elem_procs_class_1_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(init_msg_c::types::ho_request);
      break;
    case 3:
      ret.set(init_msg_c::types::retrieve_ue_context_request);
      break;
    case 7:
      ret.set(init_msg_c::types::sn_ode_addition_request);
      break;
    case 9:
      ret.set(init_msg_c::types::sn_ode_mod_request);
      break;
    case 10:
      ret.set(init_msg_c::types::sn_ode_mod_required);
      break;
    case 11:
      ret.set(init_msg_c::types::sn_ode_release_request);
      break;
    case 12:
      ret.set(init_msg_c::types::sn_ode_release_required);
      break;
    case 14:
      ret.set(init_msg_c::types::sn_ode_change_required);
      break;
    case 16:
      ret.set(init_msg_c::types::xn_removal_request);
      break;
    case 17:
      ret.set(init_msg_c::types::xn_setup_request);
      break;
    case 18:
      ret.set(init_msg_c::types::ngran_node_cfg_upd);
      break;
    case 25:
      ret.set(init_msg_c::types::e_utra_nr_cell_res_coordination_request);
      break;
    case 19:
      ret.set(init_msg_c::types::cell_activation_request);
      break;
    case 20:
      ret.set(init_msg_c::types::reset_request);
      break;
    case 34:
      ret.set(init_msg_c::types::res_status_request);
      break;
    case 36:
      ret.set(init_msg_c::types::mob_change_request);
      break;
    case 44:
      ret.set(init_msg_c::types::iab_transport_migration_management_request);
      break;
    case 45:
      ret.set(init_msg_c::types::iab_transport_migration_mod_request);
      break;
    case 46:
      ret.set(init_msg_c::types::iab_res_coordination_request);
      break;
    case 49:
      ret.set(init_msg_c::types::partial_ue_context_transfer);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
xnap_elem_procs_class_1_o::successful_outcome_c
xnap_elem_procs_class_1_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(successful_outcome_c::types::ho_request_ack);
      break;
    case 3:
      ret.set(successful_outcome_c::types::retrieve_ue_context_resp);
      break;
    case 7:
      ret.set(successful_outcome_c::types::sn_ode_addition_request_ack);
      break;
    case 9:
      ret.set(successful_outcome_c::types::sn_ode_mod_request_ack);
      break;
    case 10:
      ret.set(successful_outcome_c::types::sn_ode_mod_confirm);
      break;
    case 11:
      ret.set(successful_outcome_c::types::sn_ode_release_request_ack);
      break;
    case 12:
      ret.set(successful_outcome_c::types::sn_ode_release_confirm);
      break;
    case 14:
      ret.set(successful_outcome_c::types::sn_ode_change_confirm);
      break;
    case 16:
      ret.set(successful_outcome_c::types::xn_removal_resp);
      break;
    case 17:
      ret.set(successful_outcome_c::types::xn_setup_resp);
      break;
    case 18:
      ret.set(successful_outcome_c::types::ngran_node_cfg_upd_ack);
      break;
    case 25:
      ret.set(successful_outcome_c::types::e_utra_nr_cell_res_coordination_resp);
      break;
    case 19:
      ret.set(successful_outcome_c::types::cell_activation_resp);
      break;
    case 20:
      ret.set(successful_outcome_c::types::reset_resp);
      break;
    case 34:
      ret.set(successful_outcome_c::types::res_status_resp);
      break;
    case 36:
      ret.set(successful_outcome_c::types::mob_change_ack);
      break;
    case 44:
      ret.set(successful_outcome_c::types::iab_transport_migration_management_resp);
      break;
    case 45:
      ret.set(successful_outcome_c::types::iab_transport_migration_mod_resp);
      break;
    case 46:
      ret.set(successful_outcome_c::types::iab_res_coordination_resp);
      break;
    case 49:
      ret.set(successful_outcome_c::types::partial_ue_context_transfer_ack);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
xnap_elem_procs_class_1_o::unsuccessful_outcome_c
xnap_elem_procs_class_1_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(unsuccessful_outcome_c::types::ho_prep_fail);
      break;
    case 3:
      ret.set(unsuccessful_outcome_c::types::retrieve_ue_context_fail);
      break;
    case 7:
      ret.set(unsuccessful_outcome_c::types::sn_ode_addition_request_reject);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::sn_ode_mod_request_reject);
      break;
    case 10:
      ret.set(unsuccessful_outcome_c::types::sn_ode_mod_refuse);
      break;
    case 11:
      ret.set(unsuccessful_outcome_c::types::sn_ode_release_reject);
      break;
    case 14:
      ret.set(unsuccessful_outcome_c::types::sn_ode_change_refuse);
      break;
    case 16:
      ret.set(unsuccessful_outcome_c::types::xn_removal_fail);
      break;
    case 17:
      ret.set(unsuccessful_outcome_c::types::xn_setup_fail);
      break;
    case 18:
      ret.set(unsuccessful_outcome_c::types::ngran_node_cfg_upd_fail);
      break;
    case 19:
      ret.set(unsuccessful_outcome_c::types::cell_activation_fail);
      break;
    case 34:
      ret.set(unsuccessful_outcome_c::types::res_status_fail);
      break;
    case 36:
      ret.set(unsuccessful_outcome_c::types::mob_change_fail);
      break;
    case 44:
      ret.set(unsuccessful_outcome_c::types::iab_transport_migration_management_reject);
      break;
    case 49:
      ret.set(unsuccessful_outcome_c::types::partial_ue_context_transfer_fail);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e xnap_elem_procs_class_1_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 0:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 12:
      return crit_e::reject;
    case 14:
      return crit_e::reject;
    case 16:
      return crit_e::reject;
    case 17:
      return crit_e::reject;
    case 18:
      return crit_e::reject;
    case 25:
      return crit_e::reject;
    case 19:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 34:
      return crit_e::reject;
    case 36:
      return crit_e::reject;
    case 44:
      return crit_e::reject;
    case 45:
      return crit_e::reject;
    case 46:
      return crit_e::reject;
    case 49:
      return crit_e::reject;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void xnap_elem_procs_class_1_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ho_request:
      c = ho_request_s{};
      break;
    case types::retrieve_ue_context_request:
      c = retrieve_ue_context_request_s{};
      break;
    case types::sn_ode_addition_request:
      c = sn_ode_addition_request_s{};
      break;
    case types::sn_ode_mod_request:
      c = sn_ode_mod_request_s{};
      break;
    case types::sn_ode_mod_required:
      c = sn_ode_mod_required_s{};
      break;
    case types::sn_ode_release_request:
      c = sn_ode_release_request_s{};
      break;
    case types::sn_ode_release_required:
      c = sn_ode_release_required_s{};
      break;
    case types::sn_ode_change_required:
      c = sn_ode_change_required_s{};
      break;
    case types::xn_removal_request:
      c = xn_removal_request_s{};
      break;
    case types::xn_setup_request:
      c = xn_setup_request_s{};
      break;
    case types::ngran_node_cfg_upd:
      c = ngran_node_cfg_upd_s{};
      break;
    case types::e_utra_nr_cell_res_coordination_request:
      c = e_utra_nr_cell_res_coordination_request_s{};
      break;
    case types::cell_activation_request:
      c = cell_activation_request_s{};
      break;
    case types::reset_request:
      c = reset_request_s{};
      break;
    case types::res_status_request:
      c = res_status_request_s{};
      break;
    case types::mob_change_request:
      c = mob_change_request_s{};
      break;
    case types::iab_transport_migration_management_request:
      c = iab_transport_migration_management_request_s{};
      break;
    case types::iab_transport_migration_mod_request:
      c = iab_transport_migration_mod_request_s{};
      break;
    case types::iab_res_coordination_request:
      c = iab_res_coordination_request_s{};
      break;
    case types::partial_ue_context_transfer:
      c = partial_ue_context_transfer_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::init_msg_c");
  }
}
ho_request_s& xnap_elem_procs_class_1_o::init_msg_c::ho_request()
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
retrieve_ue_context_request_s& xnap_elem_procs_class_1_o::init_msg_c::retrieve_ue_context_request()
{
  assert_choice_type(types::retrieve_ue_context_request, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_request_s>();
}
sn_ode_addition_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_addition_request()
{
  assert_choice_type(types::sn_ode_addition_request, type_, "InitiatingMessage");
  return c.get<sn_ode_addition_request_s>();
}
sn_ode_mod_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_mod_request()
{
  assert_choice_type(types::sn_ode_mod_request, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_request_s>();
}
sn_ode_mod_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_mod_required()
{
  assert_choice_type(types::sn_ode_mod_required, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_required_s>();
}
sn_ode_release_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_release_request()
{
  assert_choice_type(types::sn_ode_release_request, type_, "InitiatingMessage");
  return c.get<sn_ode_release_request_s>();
}
sn_ode_release_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_release_required()
{
  assert_choice_type(types::sn_ode_release_required, type_, "InitiatingMessage");
  return c.get<sn_ode_release_required_s>();
}
sn_ode_change_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_change_required()
{
  assert_choice_type(types::sn_ode_change_required, type_, "InitiatingMessage");
  return c.get<sn_ode_change_required_s>();
}
xn_removal_request_s& xnap_elem_procs_class_1_o::init_msg_c::xn_removal_request()
{
  assert_choice_type(types::xn_removal_request, type_, "InitiatingMessage");
  return c.get<xn_removal_request_s>();
}
xn_setup_request_s& xnap_elem_procs_class_1_o::init_msg_c::xn_setup_request()
{
  assert_choice_type(types::xn_setup_request, type_, "InitiatingMessage");
  return c.get<xn_setup_request_s>();
}
ngran_node_cfg_upd_s& xnap_elem_procs_class_1_o::init_msg_c::ngran_node_cfg_upd()
{
  assert_choice_type(types::ngran_node_cfg_upd, type_, "InitiatingMessage");
  return c.get<ngran_node_cfg_upd_s>();
}
e_utra_nr_cell_res_coordination_request_s&
xnap_elem_procs_class_1_o::init_msg_c::e_utra_nr_cell_res_coordination_request()
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_request, type_, "InitiatingMessage");
  return c.get<e_utra_nr_cell_res_coordination_request_s>();
}
cell_activation_request_s& xnap_elem_procs_class_1_o::init_msg_c::cell_activation_request()
{
  assert_choice_type(types::cell_activation_request, type_, "InitiatingMessage");
  return c.get<cell_activation_request_s>();
}
reset_request_s& xnap_elem_procs_class_1_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
res_status_request_s& xnap_elem_procs_class_1_o::init_msg_c::res_status_request()
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
mob_change_request_s& xnap_elem_procs_class_1_o::init_msg_c::mob_change_request()
{
  assert_choice_type(types::mob_change_request, type_, "InitiatingMessage");
  return c.get<mob_change_request_s>();
}
iab_transport_migration_management_request_s&
xnap_elem_procs_class_1_o::init_msg_c::iab_transport_migration_management_request()
{
  assert_choice_type(types::iab_transport_migration_management_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_management_request_s>();
}
iab_transport_migration_mod_request_s& xnap_elem_procs_class_1_o::init_msg_c::iab_transport_migration_mod_request()
{
  assert_choice_type(types::iab_transport_migration_mod_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_mod_request_s>();
}
iab_res_coordination_request_s& xnap_elem_procs_class_1_o::init_msg_c::iab_res_coordination_request()
{
  assert_choice_type(types::iab_res_coordination_request, type_, "InitiatingMessage");
  return c.get<iab_res_coordination_request_s>();
}
partial_ue_context_transfer_s& xnap_elem_procs_class_1_o::init_msg_c::partial_ue_context_transfer()
{
  assert_choice_type(types::partial_ue_context_transfer, type_, "InitiatingMessage");
  return c.get<partial_ue_context_transfer_s>();
}
const ho_request_s& xnap_elem_procs_class_1_o::init_msg_c::ho_request() const
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
const retrieve_ue_context_request_s& xnap_elem_procs_class_1_o::init_msg_c::retrieve_ue_context_request() const
{
  assert_choice_type(types::retrieve_ue_context_request, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_request_s>();
}
const sn_ode_addition_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_addition_request() const
{
  assert_choice_type(types::sn_ode_addition_request, type_, "InitiatingMessage");
  return c.get<sn_ode_addition_request_s>();
}
const sn_ode_mod_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_mod_request() const
{
  assert_choice_type(types::sn_ode_mod_request, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_request_s>();
}
const sn_ode_mod_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_mod_required() const
{
  assert_choice_type(types::sn_ode_mod_required, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_required_s>();
}
const sn_ode_release_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_release_request() const
{
  assert_choice_type(types::sn_ode_release_request, type_, "InitiatingMessage");
  return c.get<sn_ode_release_request_s>();
}
const sn_ode_release_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_release_required() const
{
  assert_choice_type(types::sn_ode_release_required, type_, "InitiatingMessage");
  return c.get<sn_ode_release_required_s>();
}
const sn_ode_change_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_change_required() const
{
  assert_choice_type(types::sn_ode_change_required, type_, "InitiatingMessage");
  return c.get<sn_ode_change_required_s>();
}
const xn_removal_request_s& xnap_elem_procs_class_1_o::init_msg_c::xn_removal_request() const
{
  assert_choice_type(types::xn_removal_request, type_, "InitiatingMessage");
  return c.get<xn_removal_request_s>();
}
const xn_setup_request_s& xnap_elem_procs_class_1_o::init_msg_c::xn_setup_request() const
{
  assert_choice_type(types::xn_setup_request, type_, "InitiatingMessage");
  return c.get<xn_setup_request_s>();
}
const ngran_node_cfg_upd_s& xnap_elem_procs_class_1_o::init_msg_c::ngran_node_cfg_upd() const
{
  assert_choice_type(types::ngran_node_cfg_upd, type_, "InitiatingMessage");
  return c.get<ngran_node_cfg_upd_s>();
}
const e_utra_nr_cell_res_coordination_request_s&
xnap_elem_procs_class_1_o::init_msg_c::e_utra_nr_cell_res_coordination_request() const
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_request, type_, "InitiatingMessage");
  return c.get<e_utra_nr_cell_res_coordination_request_s>();
}
const cell_activation_request_s& xnap_elem_procs_class_1_o::init_msg_c::cell_activation_request() const
{
  assert_choice_type(types::cell_activation_request, type_, "InitiatingMessage");
  return c.get<cell_activation_request_s>();
}
const reset_request_s& xnap_elem_procs_class_1_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const res_status_request_s& xnap_elem_procs_class_1_o::init_msg_c::res_status_request() const
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
const mob_change_request_s& xnap_elem_procs_class_1_o::init_msg_c::mob_change_request() const
{
  assert_choice_type(types::mob_change_request, type_, "InitiatingMessage");
  return c.get<mob_change_request_s>();
}
const iab_transport_migration_management_request_s&
xnap_elem_procs_class_1_o::init_msg_c::iab_transport_migration_management_request() const
{
  assert_choice_type(types::iab_transport_migration_management_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_management_request_s>();
}
const iab_transport_migration_mod_request_s&
xnap_elem_procs_class_1_o::init_msg_c::iab_transport_migration_mod_request() const
{
  assert_choice_type(types::iab_transport_migration_mod_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_mod_request_s>();
}
const iab_res_coordination_request_s& xnap_elem_procs_class_1_o::init_msg_c::iab_res_coordination_request() const
{
  assert_choice_type(types::iab_res_coordination_request, type_, "InitiatingMessage");
  return c.get<iab_res_coordination_request_s>();
}
const partial_ue_context_transfer_s& xnap_elem_procs_class_1_o::init_msg_c::partial_ue_context_transfer() const
{
  assert_choice_type(types::partial_ue_context_transfer, type_, "InitiatingMessage");
  return c.get<partial_ue_context_transfer_s>();
}
void xnap_elem_procs_class_1_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_request:
      j.write_fieldname("HandoverRequest");
      c.get<ho_request_s>().to_json(j);
      break;
    case types::retrieve_ue_context_request:
      j.write_fieldname("RetrieveUEContextRequest");
      c.get<retrieve_ue_context_request_s>().to_json(j);
      break;
    case types::sn_ode_addition_request:
      j.write_fieldname("SNodeAdditionRequest");
      c.get<sn_ode_addition_request_s>().to_json(j);
      break;
    case types::sn_ode_mod_request:
      j.write_fieldname("SNodeModificationRequest");
      c.get<sn_ode_mod_request_s>().to_json(j);
      break;
    case types::sn_ode_mod_required:
      j.write_fieldname("SNodeModificationRequired");
      c.get<sn_ode_mod_required_s>().to_json(j);
      break;
    case types::sn_ode_release_request:
      j.write_fieldname("SNodeReleaseRequest");
      c.get<sn_ode_release_request_s>().to_json(j);
      break;
    case types::sn_ode_release_required:
      j.write_fieldname("SNodeReleaseRequired");
      c.get<sn_ode_release_required_s>().to_json(j);
      break;
    case types::sn_ode_change_required:
      j.write_fieldname("SNodeChangeRequired");
      c.get<sn_ode_change_required_s>().to_json(j);
      break;
    case types::xn_removal_request:
      j.write_fieldname("XnRemovalRequest");
      c.get<xn_removal_request_s>().to_json(j);
      break;
    case types::xn_setup_request:
      j.write_fieldname("XnSetupRequest");
      c.get<xn_setup_request_s>().to_json(j);
      break;
    case types::ngran_node_cfg_upd:
      j.write_fieldname("NGRANNodeConfigurationUpdate");
      c.get<ngran_node_cfg_upd_s>().to_json(j);
      break;
    case types::e_utra_nr_cell_res_coordination_request:
      j.write_fieldname("E-UTRA-NR-CellResourceCoordinationRequest");
      c.get<e_utra_nr_cell_res_coordination_request_s>().to_json(j);
      break;
    case types::cell_activation_request:
      j.write_fieldname("CellActivationRequest");
      c.get<cell_activation_request_s>().to_json(j);
      break;
    case types::reset_request:
      j.write_fieldname("ResetRequest");
      c.get<reset_request_s>().to_json(j);
      break;
    case types::res_status_request:
      j.write_fieldname("ResourceStatusRequest");
      c.get<res_status_request_s>().to_json(j);
      break;
    case types::mob_change_request:
      j.write_fieldname("MobilityChangeRequest");
      c.get<mob_change_request_s>().to_json(j);
      break;
    case types::iab_transport_migration_management_request:
      j.write_fieldname("IABTransportMigrationManagementRequest");
      c.get<iab_transport_migration_management_request_s>().to_json(j);
      break;
    case types::iab_transport_migration_mod_request:
      j.write_fieldname("IABTransportMigrationModificationRequest");
      c.get<iab_transport_migration_mod_request_s>().to_json(j);
      break;
    case types::iab_res_coordination_request:
      j.write_fieldname("IABResourceCoordinationRequest");
      c.get<iab_res_coordination_request_s>().to_json(j);
      break;
    case types::partial_ue_context_transfer:
      j.write_fieldname("PartialUEContextTransfer");
      c.get<partial_ue_context_transfer_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_class_1_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_request:
      HANDLE_CODE(c.get<ho_request_s>().pack(bref));
      break;
    case types::retrieve_ue_context_request:
      HANDLE_CODE(c.get<retrieve_ue_context_request_s>().pack(bref));
      break;
    case types::sn_ode_addition_request:
      HANDLE_CODE(c.get<sn_ode_addition_request_s>().pack(bref));
      break;
    case types::sn_ode_mod_request:
      HANDLE_CODE(c.get<sn_ode_mod_request_s>().pack(bref));
      break;
    case types::sn_ode_mod_required:
      HANDLE_CODE(c.get<sn_ode_mod_required_s>().pack(bref));
      break;
    case types::sn_ode_release_request:
      HANDLE_CODE(c.get<sn_ode_release_request_s>().pack(bref));
      break;
    case types::sn_ode_release_required:
      HANDLE_CODE(c.get<sn_ode_release_required_s>().pack(bref));
      break;
    case types::sn_ode_change_required:
      HANDLE_CODE(c.get<sn_ode_change_required_s>().pack(bref));
      break;
    case types::xn_removal_request:
      HANDLE_CODE(c.get<xn_removal_request_s>().pack(bref));
      break;
    case types::xn_setup_request:
      HANDLE_CODE(c.get<xn_setup_request_s>().pack(bref));
      break;
    case types::ngran_node_cfg_upd:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_s>().pack(bref));
      break;
    case types::e_utra_nr_cell_res_coordination_request:
      HANDLE_CODE(c.get<e_utra_nr_cell_res_coordination_request_s>().pack(bref));
      break;
    case types::cell_activation_request:
      HANDLE_CODE(c.get<cell_activation_request_s>().pack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().pack(bref));
      break;
    case types::res_status_request:
      HANDLE_CODE(c.get<res_status_request_s>().pack(bref));
      break;
    case types::mob_change_request:
      HANDLE_CODE(c.get<mob_change_request_s>().pack(bref));
      break;
    case types::iab_transport_migration_management_request:
      HANDLE_CODE(c.get<iab_transport_migration_management_request_s>().pack(bref));
      break;
    case types::iab_transport_migration_mod_request:
      HANDLE_CODE(c.get<iab_transport_migration_mod_request_s>().pack(bref));
      break;
    case types::iab_res_coordination_request:
      HANDLE_CODE(c.get<iab_res_coordination_request_s>().pack(bref));
      break;
    case types::partial_ue_context_transfer:
      HANDLE_CODE(c.get<partial_ue_context_transfer_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_class_1_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_request:
      HANDLE_CODE(c.get<ho_request_s>().unpack(bref));
      break;
    case types::retrieve_ue_context_request:
      HANDLE_CODE(c.get<retrieve_ue_context_request_s>().unpack(bref));
      break;
    case types::sn_ode_addition_request:
      HANDLE_CODE(c.get<sn_ode_addition_request_s>().unpack(bref));
      break;
    case types::sn_ode_mod_request:
      HANDLE_CODE(c.get<sn_ode_mod_request_s>().unpack(bref));
      break;
    case types::sn_ode_mod_required:
      HANDLE_CODE(c.get<sn_ode_mod_required_s>().unpack(bref));
      break;
    case types::sn_ode_release_request:
      HANDLE_CODE(c.get<sn_ode_release_request_s>().unpack(bref));
      break;
    case types::sn_ode_release_required:
      HANDLE_CODE(c.get<sn_ode_release_required_s>().unpack(bref));
      break;
    case types::sn_ode_change_required:
      HANDLE_CODE(c.get<sn_ode_change_required_s>().unpack(bref));
      break;
    case types::xn_removal_request:
      HANDLE_CODE(c.get<xn_removal_request_s>().unpack(bref));
      break;
    case types::xn_setup_request:
      HANDLE_CODE(c.get<xn_setup_request_s>().unpack(bref));
      break;
    case types::ngran_node_cfg_upd:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_s>().unpack(bref));
      break;
    case types::e_utra_nr_cell_res_coordination_request:
      HANDLE_CODE(c.get<e_utra_nr_cell_res_coordination_request_s>().unpack(bref));
      break;
    case types::cell_activation_request:
      HANDLE_CODE(c.get<cell_activation_request_s>().unpack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().unpack(bref));
      break;
    case types::res_status_request:
      HANDLE_CODE(c.get<res_status_request_s>().unpack(bref));
      break;
    case types::mob_change_request:
      HANDLE_CODE(c.get<mob_change_request_s>().unpack(bref));
      break;
    case types::iab_transport_migration_management_request:
      HANDLE_CODE(c.get<iab_transport_migration_management_request_s>().unpack(bref));
      break;
    case types::iab_transport_migration_mod_request:
      HANDLE_CODE(c.get<iab_transport_migration_mod_request_s>().unpack(bref));
      break;
    case types::iab_res_coordination_request:
      HANDLE_CODE(c.get<iab_res_coordination_request_s>().unpack(bref));
      break;
    case types::partial_ue_context_transfer:
      HANDLE_CODE(c.get<partial_ue_context_transfer_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_class_1_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"HandoverRequest",
                                "RetrieveUEContextRequest",
                                "SNodeAdditionRequest",
                                "SNodeModificationRequest",
                                "SNodeModificationRequired",
                                "SNodeReleaseRequest",
                                "SNodeReleaseRequired",
                                "SNodeChangeRequired",
                                "XnRemovalRequest",
                                "XnSetupRequest",
                                "NGRANNodeConfigurationUpdate",
                                "E-UTRA-NR-CellResourceCoordinationRequest",
                                "CellActivationRequest",
                                "ResetRequest",
                                "ResourceStatusRequest",
                                "MobilityChangeRequest",
                                "IABTransportMigrationManagementRequest",
                                "IABTransportMigrationModificationRequest",
                                "IABResourceCoordinationRequest",
                                "PartialUEContextTransfer"};
  return convert_enum_idx(names, 20, value, "xnap_elem_procs_class_1_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void xnap_elem_procs_class_1_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ho_request_ack:
      c = ho_request_ack_s{};
      break;
    case types::retrieve_ue_context_resp:
      c = retrieve_ue_context_resp_s{};
      break;
    case types::sn_ode_addition_request_ack:
      c = sn_ode_addition_request_ack_s{};
      break;
    case types::sn_ode_mod_request_ack:
      c = sn_ode_mod_request_ack_s{};
      break;
    case types::sn_ode_mod_confirm:
      c = sn_ode_mod_confirm_s{};
      break;
    case types::sn_ode_release_request_ack:
      c = sn_ode_release_request_ack_s{};
      break;
    case types::sn_ode_release_confirm:
      c = sn_ode_release_confirm_s{};
      break;
    case types::sn_ode_change_confirm:
      c = sn_ode_change_confirm_s{};
      break;
    case types::xn_removal_resp:
      c = xn_removal_resp_s{};
      break;
    case types::xn_setup_resp:
      c = xn_setup_resp_s{};
      break;
    case types::ngran_node_cfg_upd_ack:
      c = ngran_node_cfg_upd_ack_s{};
      break;
    case types::e_utra_nr_cell_res_coordination_resp:
      c = e_utra_nr_cell_res_coordination_resp_s{};
      break;
    case types::cell_activation_resp:
      c = cell_activation_resp_s{};
      break;
    case types::reset_resp:
      c = reset_resp_s{};
      break;
    case types::res_status_resp:
      c = res_status_resp_s{};
      break;
    case types::mob_change_ack:
      c = mob_change_ack_s{};
      break;
    case types::iab_transport_migration_management_resp:
      c = iab_transport_migration_management_resp_s{};
      break;
    case types::iab_transport_migration_mod_resp:
      c = iab_transport_migration_mod_resp_s{};
      break;
    case types::iab_res_coordination_resp:
      c = iab_res_coordination_resp_s{};
      break;
    case types::partial_ue_context_transfer_ack:
      c = partial_ue_context_transfer_ack_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::successful_outcome_c");
  }
}
ho_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::ho_request_ack()
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
retrieve_ue_context_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::retrieve_ue_context_resp()
{
  assert_choice_type(types::retrieve_ue_context_resp, type_, "SuccessfulOutcome");
  return c.get<retrieve_ue_context_resp_s>();
}
sn_ode_addition_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_addition_request_ack()
{
  assert_choice_type(types::sn_ode_addition_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_addition_request_ack_s>();
}
sn_ode_mod_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_mod_request_ack()
{
  assert_choice_type(types::sn_ode_mod_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_request_ack_s>();
}
sn_ode_mod_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_mod_confirm()
{
  assert_choice_type(types::sn_ode_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_confirm_s>();
}
sn_ode_release_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_release_request_ack()
{
  assert_choice_type(types::sn_ode_release_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_request_ack_s>();
}
sn_ode_release_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_release_confirm()
{
  assert_choice_type(types::sn_ode_release_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_confirm_s>();
}
sn_ode_change_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_change_confirm()
{
  assert_choice_type(types::sn_ode_change_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_change_confirm_s>();
}
xn_removal_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::xn_removal_resp()
{
  assert_choice_type(types::xn_removal_resp, type_, "SuccessfulOutcome");
  return c.get<xn_removal_resp_s>();
}
xn_setup_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::xn_setup_resp()
{
  assert_choice_type(types::xn_setup_resp, type_, "SuccessfulOutcome");
  return c.get<xn_setup_resp_s>();
}
ngran_node_cfg_upd_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::ngran_node_cfg_upd_ack()
{
  assert_choice_type(types::ngran_node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_ack_s>();
}
e_utra_nr_cell_res_coordination_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::e_utra_nr_cell_res_coordination_resp()
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<e_utra_nr_cell_res_coordination_resp_s>();
}
cell_activation_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::cell_activation_resp()
{
  assert_choice_type(types::cell_activation_resp, type_, "SuccessfulOutcome");
  return c.get<cell_activation_resp_s>();
}
reset_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
res_status_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::res_status_resp()
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
mob_change_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::mob_change_ack()
{
  assert_choice_type(types::mob_change_ack, type_, "SuccessfulOutcome");
  return c.get<mob_change_ack_s>();
}
iab_transport_migration_management_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::iab_transport_migration_management_resp()
{
  assert_choice_type(types::iab_transport_migration_management_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_management_resp_s>();
}
iab_transport_migration_mod_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::iab_transport_migration_mod_resp()
{
  assert_choice_type(types::iab_transport_migration_mod_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_mod_resp_s>();
}
iab_res_coordination_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::iab_res_coordination_resp()
{
  assert_choice_type(types::iab_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<iab_res_coordination_resp_s>();
}
partial_ue_context_transfer_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::partial_ue_context_transfer_ack()
{
  assert_choice_type(types::partial_ue_context_transfer_ack, type_, "SuccessfulOutcome");
  return c.get<partial_ue_context_transfer_ack_s>();
}
const ho_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::ho_request_ack() const
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
const retrieve_ue_context_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::retrieve_ue_context_resp() const
{
  assert_choice_type(types::retrieve_ue_context_resp, type_, "SuccessfulOutcome");
  return c.get<retrieve_ue_context_resp_s>();
}
const sn_ode_addition_request_ack_s&
xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_addition_request_ack() const
{
  assert_choice_type(types::sn_ode_addition_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_addition_request_ack_s>();
}
const sn_ode_mod_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_mod_request_ack() const
{
  assert_choice_type(types::sn_ode_mod_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_request_ack_s>();
}
const sn_ode_mod_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_mod_confirm() const
{
  assert_choice_type(types::sn_ode_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_confirm_s>();
}
const sn_ode_release_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_release_request_ack() const
{
  assert_choice_type(types::sn_ode_release_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_request_ack_s>();
}
const sn_ode_release_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_release_confirm() const
{
  assert_choice_type(types::sn_ode_release_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_confirm_s>();
}
const sn_ode_change_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_change_confirm() const
{
  assert_choice_type(types::sn_ode_change_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_change_confirm_s>();
}
const xn_removal_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::xn_removal_resp() const
{
  assert_choice_type(types::xn_removal_resp, type_, "SuccessfulOutcome");
  return c.get<xn_removal_resp_s>();
}
const xn_setup_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::xn_setup_resp() const
{
  assert_choice_type(types::xn_setup_resp, type_, "SuccessfulOutcome");
  return c.get<xn_setup_resp_s>();
}
const ngran_node_cfg_upd_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::ngran_node_cfg_upd_ack() const
{
  assert_choice_type(types::ngran_node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_ack_s>();
}
const e_utra_nr_cell_res_coordination_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::e_utra_nr_cell_res_coordination_resp() const
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<e_utra_nr_cell_res_coordination_resp_s>();
}
const cell_activation_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::cell_activation_resp() const
{
  assert_choice_type(types::cell_activation_resp, type_, "SuccessfulOutcome");
  return c.get<cell_activation_resp_s>();
}
const reset_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const res_status_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::res_status_resp() const
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
const mob_change_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::mob_change_ack() const
{
  assert_choice_type(types::mob_change_ack, type_, "SuccessfulOutcome");
  return c.get<mob_change_ack_s>();
}
const iab_transport_migration_management_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::iab_transport_migration_management_resp() const
{
  assert_choice_type(types::iab_transport_migration_management_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_management_resp_s>();
}
const iab_transport_migration_mod_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::iab_transport_migration_mod_resp() const
{
  assert_choice_type(types::iab_transport_migration_mod_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_mod_resp_s>();
}
const iab_res_coordination_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::iab_res_coordination_resp() const
{
  assert_choice_type(types::iab_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<iab_res_coordination_resp_s>();
}
const partial_ue_context_transfer_ack_s&
xnap_elem_procs_class_1_o::successful_outcome_c::partial_ue_context_transfer_ack() const
{
  assert_choice_type(types::partial_ue_context_transfer_ack, type_, "SuccessfulOutcome");
  return c.get<partial_ue_context_transfer_ack_s>();
}
void xnap_elem_procs_class_1_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_request_ack:
      j.write_fieldname("HandoverRequestAcknowledge");
      c.get<ho_request_ack_s>().to_json(j);
      break;
    case types::retrieve_ue_context_resp:
      j.write_fieldname("RetrieveUEContextResponse");
      c.get<retrieve_ue_context_resp_s>().to_json(j);
      break;
    case types::sn_ode_addition_request_ack:
      j.write_fieldname("SNodeAdditionRequestAcknowledge");
      c.get<sn_ode_addition_request_ack_s>().to_json(j);
      break;
    case types::sn_ode_mod_request_ack:
      j.write_fieldname("SNodeModificationRequestAcknowledge");
      c.get<sn_ode_mod_request_ack_s>().to_json(j);
      break;
    case types::sn_ode_mod_confirm:
      j.write_fieldname("SNodeModificationConfirm");
      c.get<sn_ode_mod_confirm_s>().to_json(j);
      break;
    case types::sn_ode_release_request_ack:
      j.write_fieldname("SNodeReleaseRequestAcknowledge");
      c.get<sn_ode_release_request_ack_s>().to_json(j);
      break;
    case types::sn_ode_release_confirm:
      j.write_fieldname("SNodeReleaseConfirm");
      c.get<sn_ode_release_confirm_s>().to_json(j);
      break;
    case types::sn_ode_change_confirm:
      j.write_fieldname("SNodeChangeConfirm");
      c.get<sn_ode_change_confirm_s>().to_json(j);
      break;
    case types::xn_removal_resp:
      j.write_fieldname("XnRemovalResponse");
      c.get<xn_removal_resp_s>().to_json(j);
      break;
    case types::xn_setup_resp:
      j.write_fieldname("XnSetupResponse");
      c.get<xn_setup_resp_s>().to_json(j);
      break;
    case types::ngran_node_cfg_upd_ack:
      j.write_fieldname("NGRANNodeConfigurationUpdateAcknowledge");
      c.get<ngran_node_cfg_upd_ack_s>().to_json(j);
      break;
    case types::e_utra_nr_cell_res_coordination_resp:
      j.write_fieldname("E-UTRA-NR-CellResourceCoordinationResponse");
      c.get<e_utra_nr_cell_res_coordination_resp_s>().to_json(j);
      break;
    case types::cell_activation_resp:
      j.write_fieldname("CellActivationResponse");
      c.get<cell_activation_resp_s>().to_json(j);
      break;
    case types::reset_resp:
      j.write_fieldname("ResetResponse");
      c.get<reset_resp_s>().to_json(j);
      break;
    case types::res_status_resp:
      j.write_fieldname("ResourceStatusResponse");
      c.get<res_status_resp_s>().to_json(j);
      break;
    case types::mob_change_ack:
      j.write_fieldname("MobilityChangeAcknowledge");
      c.get<mob_change_ack_s>().to_json(j);
      break;
    case types::iab_transport_migration_management_resp:
      j.write_fieldname("IABTransportMigrationManagementResponse");
      c.get<iab_transport_migration_management_resp_s>().to_json(j);
      break;
    case types::iab_transport_migration_mod_resp:
      j.write_fieldname("IABTransportMigrationModificationResponse");
      c.get<iab_transport_migration_mod_resp_s>().to_json(j);
      break;
    case types::iab_res_coordination_resp:
      j.write_fieldname("IABResourceCoordinationResponse");
      c.get<iab_res_coordination_resp_s>().to_json(j);
      break;
    case types::partial_ue_context_transfer_ack:
      j.write_fieldname("PartialUEContextTransferAcknowledge");
      c.get<partial_ue_context_transfer_ack_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_class_1_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_request_ack:
      HANDLE_CODE(c.get<ho_request_ack_s>().pack(bref));
      break;
    case types::retrieve_ue_context_resp:
      HANDLE_CODE(c.get<retrieve_ue_context_resp_s>().pack(bref));
      break;
    case types::sn_ode_addition_request_ack:
      HANDLE_CODE(c.get<sn_ode_addition_request_ack_s>().pack(bref));
      break;
    case types::sn_ode_mod_request_ack:
      HANDLE_CODE(c.get<sn_ode_mod_request_ack_s>().pack(bref));
      break;
    case types::sn_ode_mod_confirm:
      HANDLE_CODE(c.get<sn_ode_mod_confirm_s>().pack(bref));
      break;
    case types::sn_ode_release_request_ack:
      HANDLE_CODE(c.get<sn_ode_release_request_ack_s>().pack(bref));
      break;
    case types::sn_ode_release_confirm:
      HANDLE_CODE(c.get<sn_ode_release_confirm_s>().pack(bref));
      break;
    case types::sn_ode_change_confirm:
      HANDLE_CODE(c.get<sn_ode_change_confirm_s>().pack(bref));
      break;
    case types::xn_removal_resp:
      HANDLE_CODE(c.get<xn_removal_resp_s>().pack(bref));
      break;
    case types::xn_setup_resp:
      HANDLE_CODE(c.get<xn_setup_resp_s>().pack(bref));
      break;
    case types::ngran_node_cfg_upd_ack:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_ack_s>().pack(bref));
      break;
    case types::e_utra_nr_cell_res_coordination_resp:
      HANDLE_CODE(c.get<e_utra_nr_cell_res_coordination_resp_s>().pack(bref));
      break;
    case types::cell_activation_resp:
      HANDLE_CODE(c.get<cell_activation_resp_s>().pack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().pack(bref));
      break;
    case types::res_status_resp:
      HANDLE_CODE(c.get<res_status_resp_s>().pack(bref));
      break;
    case types::mob_change_ack:
      HANDLE_CODE(c.get<mob_change_ack_s>().pack(bref));
      break;
    case types::iab_transport_migration_management_resp:
      HANDLE_CODE(c.get<iab_transport_migration_management_resp_s>().pack(bref));
      break;
    case types::iab_transport_migration_mod_resp:
      HANDLE_CODE(c.get<iab_transport_migration_mod_resp_s>().pack(bref));
      break;
    case types::iab_res_coordination_resp:
      HANDLE_CODE(c.get<iab_res_coordination_resp_s>().pack(bref));
      break;
    case types::partial_ue_context_transfer_ack:
      HANDLE_CODE(c.get<partial_ue_context_transfer_ack_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_class_1_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_request_ack:
      HANDLE_CODE(c.get<ho_request_ack_s>().unpack(bref));
      break;
    case types::retrieve_ue_context_resp:
      HANDLE_CODE(c.get<retrieve_ue_context_resp_s>().unpack(bref));
      break;
    case types::sn_ode_addition_request_ack:
      HANDLE_CODE(c.get<sn_ode_addition_request_ack_s>().unpack(bref));
      break;
    case types::sn_ode_mod_request_ack:
      HANDLE_CODE(c.get<sn_ode_mod_request_ack_s>().unpack(bref));
      break;
    case types::sn_ode_mod_confirm:
      HANDLE_CODE(c.get<sn_ode_mod_confirm_s>().unpack(bref));
      break;
    case types::sn_ode_release_request_ack:
      HANDLE_CODE(c.get<sn_ode_release_request_ack_s>().unpack(bref));
      break;
    case types::sn_ode_release_confirm:
      HANDLE_CODE(c.get<sn_ode_release_confirm_s>().unpack(bref));
      break;
    case types::sn_ode_change_confirm:
      HANDLE_CODE(c.get<sn_ode_change_confirm_s>().unpack(bref));
      break;
    case types::xn_removal_resp:
      HANDLE_CODE(c.get<xn_removal_resp_s>().unpack(bref));
      break;
    case types::xn_setup_resp:
      HANDLE_CODE(c.get<xn_setup_resp_s>().unpack(bref));
      break;
    case types::ngran_node_cfg_upd_ack:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::e_utra_nr_cell_res_coordination_resp:
      HANDLE_CODE(c.get<e_utra_nr_cell_res_coordination_resp_s>().unpack(bref));
      break;
    case types::cell_activation_resp:
      HANDLE_CODE(c.get<cell_activation_resp_s>().unpack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().unpack(bref));
      break;
    case types::res_status_resp:
      HANDLE_CODE(c.get<res_status_resp_s>().unpack(bref));
      break;
    case types::mob_change_ack:
      HANDLE_CODE(c.get<mob_change_ack_s>().unpack(bref));
      break;
    case types::iab_transport_migration_management_resp:
      HANDLE_CODE(c.get<iab_transport_migration_management_resp_s>().unpack(bref));
      break;
    case types::iab_transport_migration_mod_resp:
      HANDLE_CODE(c.get<iab_transport_migration_mod_resp_s>().unpack(bref));
      break;
    case types::iab_res_coordination_resp:
      HANDLE_CODE(c.get<iab_res_coordination_resp_s>().unpack(bref));
      break;
    case types::partial_ue_context_transfer_ack:
      HANDLE_CODE(c.get<partial_ue_context_transfer_ack_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_class_1_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"HandoverRequestAcknowledge",
                                "RetrieveUEContextResponse",
                                "SNodeAdditionRequestAcknowledge",
                                "SNodeModificationRequestAcknowledge",
                                "SNodeModificationConfirm",
                                "SNodeReleaseRequestAcknowledge",
                                "SNodeReleaseConfirm",
                                "SNodeChangeConfirm",
                                "XnRemovalResponse",
                                "XnSetupResponse",
                                "NGRANNodeConfigurationUpdateAcknowledge",
                                "E-UTRA-NR-CellResourceCoordinationResponse",
                                "CellActivationResponse",
                                "ResetResponse",
                                "ResourceStatusResponse",
                                "MobilityChangeAcknowledge",
                                "IABTransportMigrationManagementResponse",
                                "IABTransportMigrationModificationResponse",
                                "IABResourceCoordinationResponse",
                                "PartialUEContextTransferAcknowledge"};
  return convert_enum_idx(names, 20, value, "xnap_elem_procs_class_1_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void xnap_elem_procs_class_1_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ho_prep_fail:
      c = ho_prep_fail_s{};
      break;
    case types::retrieve_ue_context_fail:
      c = retrieve_ue_context_fail_s{};
      break;
    case types::sn_ode_addition_request_reject:
      c = sn_ode_addition_request_reject_s{};
      break;
    case types::sn_ode_mod_request_reject:
      c = sn_ode_mod_request_reject_s{};
      break;
    case types::sn_ode_mod_refuse:
      c = sn_ode_mod_refuse_s{};
      break;
    case types::sn_ode_release_reject:
      c = sn_ode_release_reject_s{};
      break;
    case types::sn_ode_change_refuse:
      c = sn_ode_change_refuse_s{};
      break;
    case types::xn_removal_fail:
      c = xn_removal_fail_s{};
      break;
    case types::xn_setup_fail:
      c = xn_setup_fail_s{};
      break;
    case types::ngran_node_cfg_upd_fail:
      c = ngran_node_cfg_upd_fail_s{};
      break;
    case types::cell_activation_fail:
      c = cell_activation_fail_s{};
      break;
    case types::res_status_fail:
      c = res_status_fail_s{};
      break;
    case types::mob_change_fail:
      c = mob_change_fail_s{};
      break;
    case types::iab_transport_migration_management_reject:
      c = iab_transport_migration_management_reject_s{};
      break;
    case types::partial_ue_context_transfer_fail:
      c = partial_ue_context_transfer_fail_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::unsuccessful_outcome_c");
  }
}
ho_prep_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::ho_prep_fail()
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
retrieve_ue_context_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::retrieve_ue_context_fail()
{
  assert_choice_type(types::retrieve_ue_context_fail, type_, "UnsuccessfulOutcome");
  return c.get<retrieve_ue_context_fail_s>();
}
sn_ode_addition_request_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_addition_request_reject()
{
  assert_choice_type(types::sn_ode_addition_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_addition_request_reject_s>();
}
sn_ode_mod_request_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_mod_request_reject()
{
  assert_choice_type(types::sn_ode_mod_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_request_reject_s>();
}
sn_ode_mod_refuse_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_mod_refuse()
{
  assert_choice_type(types::sn_ode_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_refuse_s>();
}
sn_ode_release_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_release_reject()
{
  assert_choice_type(types::sn_ode_release_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_release_reject_s>();
}
sn_ode_change_refuse_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_change_refuse()
{
  assert_choice_type(types::sn_ode_change_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_change_refuse_s>();
}
xn_removal_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::xn_removal_fail()
{
  assert_choice_type(types::xn_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_removal_fail_s>();
}
xn_setup_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::xn_setup_fail()
{
  assert_choice_type(types::xn_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_setup_fail_s>();
}
ngran_node_cfg_upd_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::ngran_node_cfg_upd_fail()
{
  assert_choice_type(types::ngran_node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_fail_s>();
}
cell_activation_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::cell_activation_fail()
{
  assert_choice_type(types::cell_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<cell_activation_fail_s>();
}
res_status_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::res_status_fail()
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
mob_change_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::mob_change_fail()
{
  assert_choice_type(types::mob_change_fail, type_, "UnsuccessfulOutcome");
  return c.get<mob_change_fail_s>();
}
iab_transport_migration_management_reject_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::iab_transport_migration_management_reject()
{
  assert_choice_type(types::iab_transport_migration_management_reject, type_, "UnsuccessfulOutcome");
  return c.get<iab_transport_migration_management_reject_s>();
}
partial_ue_context_transfer_fail_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::partial_ue_context_transfer_fail()
{
  assert_choice_type(types::partial_ue_context_transfer_fail, type_, "UnsuccessfulOutcome");
  return c.get<partial_ue_context_transfer_fail_s>();
}
const ho_prep_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::ho_prep_fail() const
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
const retrieve_ue_context_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::retrieve_ue_context_fail() const
{
  assert_choice_type(types::retrieve_ue_context_fail, type_, "UnsuccessfulOutcome");
  return c.get<retrieve_ue_context_fail_s>();
}
const sn_ode_addition_request_reject_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_addition_request_reject() const
{
  assert_choice_type(types::sn_ode_addition_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_addition_request_reject_s>();
}
const sn_ode_mod_request_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_mod_request_reject() const
{
  assert_choice_type(types::sn_ode_mod_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_request_reject_s>();
}
const sn_ode_mod_refuse_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_mod_refuse() const
{
  assert_choice_type(types::sn_ode_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_refuse_s>();
}
const sn_ode_release_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_release_reject() const
{
  assert_choice_type(types::sn_ode_release_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_release_reject_s>();
}
const sn_ode_change_refuse_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_change_refuse() const
{
  assert_choice_type(types::sn_ode_change_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_change_refuse_s>();
}
const xn_removal_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::xn_removal_fail() const
{
  assert_choice_type(types::xn_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_removal_fail_s>();
}
const xn_setup_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::xn_setup_fail() const
{
  assert_choice_type(types::xn_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_setup_fail_s>();
}
const ngran_node_cfg_upd_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::ngran_node_cfg_upd_fail() const
{
  assert_choice_type(types::ngran_node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_fail_s>();
}
const cell_activation_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::cell_activation_fail() const
{
  assert_choice_type(types::cell_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<cell_activation_fail_s>();
}
const res_status_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::res_status_fail() const
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
const mob_change_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::mob_change_fail() const
{
  assert_choice_type(types::mob_change_fail, type_, "UnsuccessfulOutcome");
  return c.get<mob_change_fail_s>();
}
const iab_transport_migration_management_reject_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::iab_transport_migration_management_reject() const
{
  assert_choice_type(types::iab_transport_migration_management_reject, type_, "UnsuccessfulOutcome");
  return c.get<iab_transport_migration_management_reject_s>();
}
const partial_ue_context_transfer_fail_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::partial_ue_context_transfer_fail() const
{
  assert_choice_type(types::partial_ue_context_transfer_fail, type_, "UnsuccessfulOutcome");
  return c.get<partial_ue_context_transfer_fail_s>();
}
void xnap_elem_procs_class_1_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_prep_fail:
      j.write_fieldname("HandoverPreparationFailure");
      c.get<ho_prep_fail_s>().to_json(j);
      break;
    case types::retrieve_ue_context_fail:
      j.write_fieldname("RetrieveUEContextFailure");
      c.get<retrieve_ue_context_fail_s>().to_json(j);
      break;
    case types::sn_ode_addition_request_reject:
      j.write_fieldname("SNodeAdditionRequestReject");
      c.get<sn_ode_addition_request_reject_s>().to_json(j);
      break;
    case types::sn_ode_mod_request_reject:
      j.write_fieldname("SNodeModificationRequestReject");
      c.get<sn_ode_mod_request_reject_s>().to_json(j);
      break;
    case types::sn_ode_mod_refuse:
      j.write_fieldname("SNodeModificationRefuse");
      c.get<sn_ode_mod_refuse_s>().to_json(j);
      break;
    case types::sn_ode_release_reject:
      j.write_fieldname("SNodeReleaseReject");
      c.get<sn_ode_release_reject_s>().to_json(j);
      break;
    case types::sn_ode_change_refuse:
      j.write_fieldname("SNodeChangeRefuse");
      c.get<sn_ode_change_refuse_s>().to_json(j);
      break;
    case types::xn_removal_fail:
      j.write_fieldname("XnRemovalFailure");
      c.get<xn_removal_fail_s>().to_json(j);
      break;
    case types::xn_setup_fail:
      j.write_fieldname("XnSetupFailure");
      c.get<xn_setup_fail_s>().to_json(j);
      break;
    case types::ngran_node_cfg_upd_fail:
      j.write_fieldname("NGRANNodeConfigurationUpdateFailure");
      c.get<ngran_node_cfg_upd_fail_s>().to_json(j);
      break;
    case types::cell_activation_fail:
      j.write_fieldname("CellActivationFailure");
      c.get<cell_activation_fail_s>().to_json(j);
      break;
    case types::res_status_fail:
      j.write_fieldname("ResourceStatusFailure");
      c.get<res_status_fail_s>().to_json(j);
      break;
    case types::mob_change_fail:
      j.write_fieldname("MobilityChangeFailure");
      c.get<mob_change_fail_s>().to_json(j);
      break;
    case types::iab_transport_migration_management_reject:
      j.write_fieldname("IABTransportMigrationManagementReject");
      c.get<iab_transport_migration_management_reject_s>().to_json(j);
      break;
    case types::partial_ue_context_transfer_fail:
      j.write_fieldname("PartialUEContextTransferFailure");
      c.get<partial_ue_context_transfer_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_class_1_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_prep_fail:
      HANDLE_CODE(c.get<ho_prep_fail_s>().pack(bref));
      break;
    case types::retrieve_ue_context_fail:
      HANDLE_CODE(c.get<retrieve_ue_context_fail_s>().pack(bref));
      break;
    case types::sn_ode_addition_request_reject:
      HANDLE_CODE(c.get<sn_ode_addition_request_reject_s>().pack(bref));
      break;
    case types::sn_ode_mod_request_reject:
      HANDLE_CODE(c.get<sn_ode_mod_request_reject_s>().pack(bref));
      break;
    case types::sn_ode_mod_refuse:
      HANDLE_CODE(c.get<sn_ode_mod_refuse_s>().pack(bref));
      break;
    case types::sn_ode_release_reject:
      HANDLE_CODE(c.get<sn_ode_release_reject_s>().pack(bref));
      break;
    case types::sn_ode_change_refuse:
      HANDLE_CODE(c.get<sn_ode_change_refuse_s>().pack(bref));
      break;
    case types::xn_removal_fail:
      HANDLE_CODE(c.get<xn_removal_fail_s>().pack(bref));
      break;
    case types::xn_setup_fail:
      HANDLE_CODE(c.get<xn_setup_fail_s>().pack(bref));
      break;
    case types::ngran_node_cfg_upd_fail:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_fail_s>().pack(bref));
      break;
    case types::cell_activation_fail:
      HANDLE_CODE(c.get<cell_activation_fail_s>().pack(bref));
      break;
    case types::res_status_fail:
      HANDLE_CODE(c.get<res_status_fail_s>().pack(bref));
      break;
    case types::mob_change_fail:
      HANDLE_CODE(c.get<mob_change_fail_s>().pack(bref));
      break;
    case types::iab_transport_migration_management_reject:
      HANDLE_CODE(c.get<iab_transport_migration_management_reject_s>().pack(bref));
      break;
    case types::partial_ue_context_transfer_fail:
      HANDLE_CODE(c.get<partial_ue_context_transfer_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_class_1_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_prep_fail:
      HANDLE_CODE(c.get<ho_prep_fail_s>().unpack(bref));
      break;
    case types::retrieve_ue_context_fail:
      HANDLE_CODE(c.get<retrieve_ue_context_fail_s>().unpack(bref));
      break;
    case types::sn_ode_addition_request_reject:
      HANDLE_CODE(c.get<sn_ode_addition_request_reject_s>().unpack(bref));
      break;
    case types::sn_ode_mod_request_reject:
      HANDLE_CODE(c.get<sn_ode_mod_request_reject_s>().unpack(bref));
      break;
    case types::sn_ode_mod_refuse:
      HANDLE_CODE(c.get<sn_ode_mod_refuse_s>().unpack(bref));
      break;
    case types::sn_ode_release_reject:
      HANDLE_CODE(c.get<sn_ode_release_reject_s>().unpack(bref));
      break;
    case types::sn_ode_change_refuse:
      HANDLE_CODE(c.get<sn_ode_change_refuse_s>().unpack(bref));
      break;
    case types::xn_removal_fail:
      HANDLE_CODE(c.get<xn_removal_fail_s>().unpack(bref));
      break;
    case types::xn_setup_fail:
      HANDLE_CODE(c.get<xn_setup_fail_s>().unpack(bref));
      break;
    case types::ngran_node_cfg_upd_fail:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::cell_activation_fail:
      HANDLE_CODE(c.get<cell_activation_fail_s>().unpack(bref));
      break;
    case types::res_status_fail:
      HANDLE_CODE(c.get<res_status_fail_s>().unpack(bref));
      break;
    case types::mob_change_fail:
      HANDLE_CODE(c.get<mob_change_fail_s>().unpack(bref));
      break;
    case types::iab_transport_migration_management_reject:
      HANDLE_CODE(c.get<iab_transport_migration_management_reject_s>().unpack(bref));
      break;
    case types::partial_ue_context_transfer_fail:
      HANDLE_CODE(c.get<partial_ue_context_transfer_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_1_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"HandoverPreparationFailure",
                                "RetrieveUEContextFailure",
                                "SNodeAdditionRequestReject",
                                "SNodeModificationRequestReject",
                                "SNodeModificationRefuse",
                                "SNodeReleaseReject",
                                "SNodeChangeRefuse",
                                "XnRemovalFailure",
                                "XnSetupFailure",
                                "NGRANNodeConfigurationUpdateFailure",
                                "CellActivationFailure",
                                "ResourceStatusFailure",
                                "MobilityChangeFailure",
                                "IABTransportMigrationManagementReject",
                                "PartialUEContextTransferFailure"};
  return convert_enum_idx(names, 15, value, "xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types");
}

// XNAP-ELEMENTARY-PROCEDURES-CLASS-2 ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
uint16_t xnap_elem_procs_class_2_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {1,  2,  4,  5,  6,  8,  13, 15, 21, 22, 23, 24, 26, 27, 28,
                                   29, 30, 31, 32, 33, 35, 37, 38, 39, 40, 42, 43, 47, 48};
  return map_enum_number(names, 29, idx, "proc_code");
}
bool xnap_elem_procs_class_2_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {1,  2,  4,  5,  6,  8,  13, 15, 21, 22, 23, 24, 26, 27, 28,
                                   29, 30, 31, 32, 33, 35, 37, 38, 39, 40, 42, 43, 47, 48};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
xnap_elem_procs_class_2_o::init_msg_c xnap_elem_procs_class_2_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 1:
      ret.set(init_msg_c::types::sn_status_transfer);
      break;
    case 2:
      ret.set(init_msg_c::types::ho_cancel);
      break;
    case 4:
      ret.set(init_msg_c::types::ran_paging);
      break;
    case 5:
      ret.set(init_msg_c::types::xn_u_address_ind);
      break;
    case 6:
      ret.set(init_msg_c::types::ue_context_release);
      break;
    case 8:
      ret.set(init_msg_c::types::sn_ode_recfg_complete);
      break;
    case 13:
      ret.set(init_msg_c::types::sn_ode_counter_check_request);
      break;
    case 15:
      ret.set(init_msg_c::types::rrc_transfer);
      break;
    case 21:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 22:
      ret.set(init_msg_c::types::private_msg);
      break;
    case 23:
      ret.set(init_msg_c::types::notif_ctrl_ind);
      break;
    case 24:
      ret.set(init_msg_c::types::activity_notif);
      break;
    case 26:
      ret.set(init_msg_c::types::secondary_rat_data_usage_report);
      break;
    case 27:
      ret.set(init_msg_c::types::deactiv_trace);
      break;
    case 28:
      ret.set(init_msg_c::types::trace_start);
      break;
    case 29:
      ret.set(init_msg_c::types::ho_success);
      break;
    case 30:
      ret.set(init_msg_c::types::conditional_ho_cancel);
      break;
    case 31:
      ret.set(init_msg_c::types::early_status_transfer);
      break;
    case 32:
      ret.set(init_msg_c::types::fail_ind);
      break;
    case 33:
      ret.set(init_msg_c::types::ho_report);
      break;
    case 35:
      ret.set(init_msg_c::types::res_status_upd);
      break;
    case 37:
      ret.set(init_msg_c::types::access_and_mob_ind);
      break;
    case 38:
      ret.set(init_msg_c::types::cell_traffic_trace);
      break;
    case 39:
      ret.set(init_msg_c::types::ran_multicast_group_paging);
      break;
    case 40:
      ret.set(init_msg_c::types::scg_fail_info_report);
      break;
    case 42:
      ret.set(init_msg_c::types::scg_fail_transfer);
      break;
    case 43:
      ret.set(init_msg_c::types::f1_c_traffic_transfer);
      break;
    case 47:
      ret.set(init_msg_c::types::retrieve_ue_context_confirm);
      break;
    case 48:
      ret.set(init_msg_c::types::cp_c_cancel);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
xnap_elem_procs_class_2_o::successful_outcome_c
xnap_elem_procs_class_2_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
xnap_elem_procs_class_2_o::unsuccessful_outcome_c
xnap_elem_procs_class_2_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e xnap_elem_procs_class_2_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 4:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 6:
      return crit_e::reject;
    case 8:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 15:
      return crit_e::reject;
    case 21:
      return crit_e::ignore;
    case 22:
      return crit_e::ignore;
    case 23:
      return crit_e::ignore;
    case 24:
      return crit_e::ignore;
    case 26:
      return crit_e::reject;
    case 27:
      return crit_e::ignore;
    case 28:
      return crit_e::ignore;
    case 29:
      return crit_e::ignore;
    case 30:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 32:
      return crit_e::ignore;
    case 33:
      return crit_e::ignore;
    case 35:
      return crit_e::ignore;
    case 37:
      return crit_e::ignore;
    case 38:
      return crit_e::ignore;
    case 39:
      return crit_e::reject;
    case 40:
      return crit_e::ignore;
    case 42:
      return crit_e::ignore;
    case 43:
      return crit_e::reject;
    case 47:
      return crit_e::ignore;
    case 48:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void xnap_elem_procs_class_2_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::sn_status_transfer:
      c = sn_status_transfer_s{};
      break;
    case types::ho_cancel:
      c = ho_cancel_s{};
      break;
    case types::ran_paging:
      c = ran_paging_s{};
      break;
    case types::xn_u_address_ind:
      c = xn_u_address_ind_s{};
      break;
    case types::ue_context_release:
      c = ue_context_release_s{};
      break;
    case types::sn_ode_recfg_complete:
      c = sn_ode_recfg_complete_s{};
      break;
    case types::sn_ode_counter_check_request:
      c = sn_ode_counter_check_request_s{};
      break;
    case types::rrc_transfer:
      c = rrc_transfer_s{};
      break;
    case types::error_ind:
      c = error_ind_s{};
      break;
    case types::private_msg:
      c = private_msg_s{};
      break;
    case types::notif_ctrl_ind:
      c = notif_ctrl_ind_s{};
      break;
    case types::activity_notif:
      c = activity_notif_s{};
      break;
    case types::secondary_rat_data_usage_report:
      c = secondary_rat_data_usage_report_s{};
      break;
    case types::deactiv_trace:
      c = deactiv_trace_s{};
      break;
    case types::trace_start:
      c = trace_start_s{};
      break;
    case types::ho_success:
      c = ho_success_s{};
      break;
    case types::conditional_ho_cancel:
      c = conditional_ho_cancel_s{};
      break;
    case types::early_status_transfer:
      c = early_status_transfer_s{};
      break;
    case types::fail_ind:
      c = fail_ind_s{};
      break;
    case types::ho_report:
      c = ho_report_s{};
      break;
    case types::res_status_upd:
      c = res_status_upd_s{};
      break;
    case types::access_and_mob_ind:
      c = access_and_mob_ind_s{};
      break;
    case types::cell_traffic_trace:
      c = cell_traffic_trace_s{};
      break;
    case types::ran_multicast_group_paging:
      c = ran_multicast_group_paging_s{};
      break;
    case types::scg_fail_info_report:
      c = scg_fail_info_report_s{};
      break;
    case types::scg_fail_transfer:
      c = scg_fail_transfer_s{};
      break;
    case types::f1_c_traffic_transfer:
      c = f1_c_traffic_transfer_s{};
      break;
    case types::retrieve_ue_context_confirm:
      c = retrieve_ue_context_confirm_s{};
      break;
    case types::cp_c_cancel:
      c = cp_c_cancel_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_2_o::init_msg_c");
  }
}
sn_status_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::sn_status_transfer()
{
  assert_choice_type(types::sn_status_transfer, type_, "InitiatingMessage");
  return c.get<sn_status_transfer_s>();
}
ho_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::ho_cancel()
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
ran_paging_s& xnap_elem_procs_class_2_o::init_msg_c::ran_paging()
{
  assert_choice_type(types::ran_paging, type_, "InitiatingMessage");
  return c.get<ran_paging_s>();
}
xn_u_address_ind_s& xnap_elem_procs_class_2_o::init_msg_c::xn_u_address_ind()
{
  assert_choice_type(types::xn_u_address_ind, type_, "InitiatingMessage");
  return c.get<xn_u_address_ind_s>();
}
ue_context_release_s& xnap_elem_procs_class_2_o::init_msg_c::ue_context_release()
{
  assert_choice_type(types::ue_context_release, type_, "InitiatingMessage");
  return c.get<ue_context_release_s>();
}
sn_ode_recfg_complete_s& xnap_elem_procs_class_2_o::init_msg_c::sn_ode_recfg_complete()
{
  assert_choice_type(types::sn_ode_recfg_complete, type_, "InitiatingMessage");
  return c.get<sn_ode_recfg_complete_s>();
}
sn_ode_counter_check_request_s& xnap_elem_procs_class_2_o::init_msg_c::sn_ode_counter_check_request()
{
  assert_choice_type(types::sn_ode_counter_check_request, type_, "InitiatingMessage");
  return c.get<sn_ode_counter_check_request_s>();
}
rrc_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::rrc_transfer()
{
  assert_choice_type(types::rrc_transfer, type_, "InitiatingMessage");
  return c.get<rrc_transfer_s>();
}
error_ind_s& xnap_elem_procs_class_2_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
private_msg_s& xnap_elem_procs_class_2_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
notif_ctrl_ind_s& xnap_elem_procs_class_2_o::init_msg_c::notif_ctrl_ind()
{
  assert_choice_type(types::notif_ctrl_ind, type_, "InitiatingMessage");
  return c.get<notif_ctrl_ind_s>();
}
activity_notif_s& xnap_elem_procs_class_2_o::init_msg_c::activity_notif()
{
  assert_choice_type(types::activity_notif, type_, "InitiatingMessage");
  return c.get<activity_notif_s>();
}
secondary_rat_data_usage_report_s& xnap_elem_procs_class_2_o::init_msg_c::secondary_rat_data_usage_report()
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
deactiv_trace_s& xnap_elem_procs_class_2_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
trace_start_s& xnap_elem_procs_class_2_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
ho_success_s& xnap_elem_procs_class_2_o::init_msg_c::ho_success()
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
conditional_ho_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::conditional_ho_cancel()
{
  assert_choice_type(types::conditional_ho_cancel, type_, "InitiatingMessage");
  return c.get<conditional_ho_cancel_s>();
}
early_status_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::early_status_transfer()
{
  assert_choice_type(types::early_status_transfer, type_, "InitiatingMessage");
  return c.get<early_status_transfer_s>();
}
fail_ind_s& xnap_elem_procs_class_2_o::init_msg_c::fail_ind()
{
  assert_choice_type(types::fail_ind, type_, "InitiatingMessage");
  return c.get<fail_ind_s>();
}
ho_report_s& xnap_elem_procs_class_2_o::init_msg_c::ho_report()
{
  assert_choice_type(types::ho_report, type_, "InitiatingMessage");
  return c.get<ho_report_s>();
}
res_status_upd_s& xnap_elem_procs_class_2_o::init_msg_c::res_status_upd()
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
access_and_mob_ind_s& xnap_elem_procs_class_2_o::init_msg_c::access_and_mob_ind()
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
cell_traffic_trace_s& xnap_elem_procs_class_2_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
ran_multicast_group_paging_s& xnap_elem_procs_class_2_o::init_msg_c::ran_multicast_group_paging()
{
  assert_choice_type(types::ran_multicast_group_paging, type_, "InitiatingMessage");
  return c.get<ran_multicast_group_paging_s>();
}
scg_fail_info_report_s& xnap_elem_procs_class_2_o::init_msg_c::scg_fail_info_report()
{
  assert_choice_type(types::scg_fail_info_report, type_, "InitiatingMessage");
  return c.get<scg_fail_info_report_s>();
}
scg_fail_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::scg_fail_transfer()
{
  assert_choice_type(types::scg_fail_transfer, type_, "InitiatingMessage");
  return c.get<scg_fail_transfer_s>();
}
f1_c_traffic_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::f1_c_traffic_transfer()
{
  assert_choice_type(types::f1_c_traffic_transfer, type_, "InitiatingMessage");
  return c.get<f1_c_traffic_transfer_s>();
}
retrieve_ue_context_confirm_s& xnap_elem_procs_class_2_o::init_msg_c::retrieve_ue_context_confirm()
{
  assert_choice_type(types::retrieve_ue_context_confirm, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_confirm_s>();
}
cp_c_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::cp_c_cancel()
{
  assert_choice_type(types::cp_c_cancel, type_, "InitiatingMessage");
  return c.get<cp_c_cancel_s>();
}
const sn_status_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::sn_status_transfer() const
{
  assert_choice_type(types::sn_status_transfer, type_, "InitiatingMessage");
  return c.get<sn_status_transfer_s>();
}
const ho_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::ho_cancel() const
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
const ran_paging_s& xnap_elem_procs_class_2_o::init_msg_c::ran_paging() const
{
  assert_choice_type(types::ran_paging, type_, "InitiatingMessage");
  return c.get<ran_paging_s>();
}
const xn_u_address_ind_s& xnap_elem_procs_class_2_o::init_msg_c::xn_u_address_ind() const
{
  assert_choice_type(types::xn_u_address_ind, type_, "InitiatingMessage");
  return c.get<xn_u_address_ind_s>();
}
const ue_context_release_s& xnap_elem_procs_class_2_o::init_msg_c::ue_context_release() const
{
  assert_choice_type(types::ue_context_release, type_, "InitiatingMessage");
  return c.get<ue_context_release_s>();
}
const sn_ode_recfg_complete_s& xnap_elem_procs_class_2_o::init_msg_c::sn_ode_recfg_complete() const
{
  assert_choice_type(types::sn_ode_recfg_complete, type_, "InitiatingMessage");
  return c.get<sn_ode_recfg_complete_s>();
}
const sn_ode_counter_check_request_s& xnap_elem_procs_class_2_o::init_msg_c::sn_ode_counter_check_request() const
{
  assert_choice_type(types::sn_ode_counter_check_request, type_, "InitiatingMessage");
  return c.get<sn_ode_counter_check_request_s>();
}
const rrc_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::rrc_transfer() const
{
  assert_choice_type(types::rrc_transfer, type_, "InitiatingMessage");
  return c.get<rrc_transfer_s>();
}
const error_ind_s& xnap_elem_procs_class_2_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const private_msg_s& xnap_elem_procs_class_2_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const notif_ctrl_ind_s& xnap_elem_procs_class_2_o::init_msg_c::notif_ctrl_ind() const
{
  assert_choice_type(types::notif_ctrl_ind, type_, "InitiatingMessage");
  return c.get<notif_ctrl_ind_s>();
}
const activity_notif_s& xnap_elem_procs_class_2_o::init_msg_c::activity_notif() const
{
  assert_choice_type(types::activity_notif, type_, "InitiatingMessage");
  return c.get<activity_notif_s>();
}
const secondary_rat_data_usage_report_s& xnap_elem_procs_class_2_o::init_msg_c::secondary_rat_data_usage_report() const
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
const deactiv_trace_s& xnap_elem_procs_class_2_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const trace_start_s& xnap_elem_procs_class_2_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const ho_success_s& xnap_elem_procs_class_2_o::init_msg_c::ho_success() const
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
const conditional_ho_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::conditional_ho_cancel() const
{
  assert_choice_type(types::conditional_ho_cancel, type_, "InitiatingMessage");
  return c.get<conditional_ho_cancel_s>();
}
const early_status_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::early_status_transfer() const
{
  assert_choice_type(types::early_status_transfer, type_, "InitiatingMessage");
  return c.get<early_status_transfer_s>();
}
const fail_ind_s& xnap_elem_procs_class_2_o::init_msg_c::fail_ind() const
{
  assert_choice_type(types::fail_ind, type_, "InitiatingMessage");
  return c.get<fail_ind_s>();
}
const ho_report_s& xnap_elem_procs_class_2_o::init_msg_c::ho_report() const
{
  assert_choice_type(types::ho_report, type_, "InitiatingMessage");
  return c.get<ho_report_s>();
}
const res_status_upd_s& xnap_elem_procs_class_2_o::init_msg_c::res_status_upd() const
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
const access_and_mob_ind_s& xnap_elem_procs_class_2_o::init_msg_c::access_and_mob_ind() const
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
const cell_traffic_trace_s& xnap_elem_procs_class_2_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const ran_multicast_group_paging_s& xnap_elem_procs_class_2_o::init_msg_c::ran_multicast_group_paging() const
{
  assert_choice_type(types::ran_multicast_group_paging, type_, "InitiatingMessage");
  return c.get<ran_multicast_group_paging_s>();
}
const scg_fail_info_report_s& xnap_elem_procs_class_2_o::init_msg_c::scg_fail_info_report() const
{
  assert_choice_type(types::scg_fail_info_report, type_, "InitiatingMessage");
  return c.get<scg_fail_info_report_s>();
}
const scg_fail_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::scg_fail_transfer() const
{
  assert_choice_type(types::scg_fail_transfer, type_, "InitiatingMessage");
  return c.get<scg_fail_transfer_s>();
}
const f1_c_traffic_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::f1_c_traffic_transfer() const
{
  assert_choice_type(types::f1_c_traffic_transfer, type_, "InitiatingMessage");
  return c.get<f1_c_traffic_transfer_s>();
}
const retrieve_ue_context_confirm_s& xnap_elem_procs_class_2_o::init_msg_c::retrieve_ue_context_confirm() const
{
  assert_choice_type(types::retrieve_ue_context_confirm, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_confirm_s>();
}
const cp_c_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::cp_c_cancel() const
{
  assert_choice_type(types::cp_c_cancel, type_, "InitiatingMessage");
  return c.get<cp_c_cancel_s>();
}
void xnap_elem_procs_class_2_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sn_status_transfer:
      j.write_fieldname("SNStatusTransfer");
      c.get<sn_status_transfer_s>().to_json(j);
      break;
    case types::ho_cancel:
      j.write_fieldname("HandoverCancel");
      c.get<ho_cancel_s>().to_json(j);
      break;
    case types::ran_paging:
      j.write_fieldname("RANPaging");
      c.get<ran_paging_s>().to_json(j);
      break;
    case types::xn_u_address_ind:
      j.write_fieldname("XnUAddressIndication");
      c.get<xn_u_address_ind_s>().to_json(j);
      break;
    case types::ue_context_release:
      j.write_fieldname("UEContextRelease");
      c.get<ue_context_release_s>().to_json(j);
      break;
    case types::sn_ode_recfg_complete:
      j.write_fieldname("SNodeReconfigurationComplete");
      c.get<sn_ode_recfg_complete_s>().to_json(j);
      break;
    case types::sn_ode_counter_check_request:
      j.write_fieldname("SNodeCounterCheckRequest");
      c.get<sn_ode_counter_check_request_s>().to_json(j);
      break;
    case types::rrc_transfer:
      j.write_fieldname("RRCTransfer");
      c.get<rrc_transfer_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::private_msg:
      j.write_fieldname("PrivateMessage");
      c.get<private_msg_s>().to_json(j);
      break;
    case types::notif_ctrl_ind:
      j.write_fieldname("NotificationControlIndication");
      c.get<notif_ctrl_ind_s>().to_json(j);
      break;
    case types::activity_notif:
      j.write_fieldname("ActivityNotification");
      c.get<activity_notif_s>().to_json(j);
      break;
    case types::secondary_rat_data_usage_report:
      j.write_fieldname("SecondaryRATDataUsageReport");
      c.get<secondary_rat_data_usage_report_s>().to_json(j);
      break;
    case types::deactiv_trace:
      j.write_fieldname("DeactivateTrace");
      c.get<deactiv_trace_s>().to_json(j);
      break;
    case types::trace_start:
      j.write_fieldname("TraceStart");
      c.get<trace_start_s>().to_json(j);
      break;
    case types::ho_success:
      j.write_fieldname("HandoverSuccess");
      c.get<ho_success_s>().to_json(j);
      break;
    case types::conditional_ho_cancel:
      j.write_fieldname("ConditionalHandoverCancel");
      c.get<conditional_ho_cancel_s>().to_json(j);
      break;
    case types::early_status_transfer:
      j.write_fieldname("EarlyStatusTransfer");
      c.get<early_status_transfer_s>().to_json(j);
      break;
    case types::fail_ind:
      j.write_fieldname("FailureIndication");
      c.get<fail_ind_s>().to_json(j);
      break;
    case types::ho_report:
      j.write_fieldname("HandoverReport");
      c.get<ho_report_s>().to_json(j);
      break;
    case types::res_status_upd:
      j.write_fieldname("ResourceStatusUpdate");
      c.get<res_status_upd_s>().to_json(j);
      break;
    case types::access_and_mob_ind:
      j.write_fieldname("AccessAndMobilityIndication");
      c.get<access_and_mob_ind_s>().to_json(j);
      break;
    case types::cell_traffic_trace:
      j.write_fieldname("CellTrafficTrace");
      c.get<cell_traffic_trace_s>().to_json(j);
      break;
    case types::ran_multicast_group_paging:
      j.write_fieldname("RANMulticastGroupPaging");
      c.get<ran_multicast_group_paging_s>().to_json(j);
      break;
    case types::scg_fail_info_report:
      j.write_fieldname("ScgFailureInformationReport");
      c.get<scg_fail_info_report_s>().to_json(j);
      break;
    case types::scg_fail_transfer:
      j.write_fieldname("ScgFailureTransfer");
      c.get<scg_fail_transfer_s>().to_json(j);
      break;
    case types::f1_c_traffic_transfer:
      j.write_fieldname("F1CTrafficTransfer");
      c.get<f1_c_traffic_transfer_s>().to_json(j);
      break;
    case types::retrieve_ue_context_confirm:
      j.write_fieldname("RetrieveUEContextConfirm");
      c.get<retrieve_ue_context_confirm_s>().to_json(j);
      break;
    case types::cp_c_cancel:
      j.write_fieldname("CPCCancel");
      c.get<cp_c_cancel_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_2_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_class_2_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::sn_status_transfer:
      HANDLE_CODE(c.get<sn_status_transfer_s>().pack(bref));
      break;
    case types::ho_cancel:
      HANDLE_CODE(c.get<ho_cancel_s>().pack(bref));
      break;
    case types::ran_paging:
      HANDLE_CODE(c.get<ran_paging_s>().pack(bref));
      break;
    case types::xn_u_address_ind:
      HANDLE_CODE(c.get<xn_u_address_ind_s>().pack(bref));
      break;
    case types::ue_context_release:
      HANDLE_CODE(c.get<ue_context_release_s>().pack(bref));
      break;
    case types::sn_ode_recfg_complete:
      HANDLE_CODE(c.get<sn_ode_recfg_complete_s>().pack(bref));
      break;
    case types::sn_ode_counter_check_request:
      HANDLE_CODE(c.get<sn_ode_counter_check_request_s>().pack(bref));
      break;
    case types::rrc_transfer:
      HANDLE_CODE(c.get<rrc_transfer_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().pack(bref));
      break;
    case types::notif_ctrl_ind:
      HANDLE_CODE(c.get<notif_ctrl_ind_s>().pack(bref));
      break;
    case types::activity_notif:
      HANDLE_CODE(c.get<activity_notif_s>().pack(bref));
      break;
    case types::secondary_rat_data_usage_report:
      HANDLE_CODE(c.get<secondary_rat_data_usage_report_s>().pack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().pack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().pack(bref));
      break;
    case types::ho_success:
      HANDLE_CODE(c.get<ho_success_s>().pack(bref));
      break;
    case types::conditional_ho_cancel:
      HANDLE_CODE(c.get<conditional_ho_cancel_s>().pack(bref));
      break;
    case types::early_status_transfer:
      HANDLE_CODE(c.get<early_status_transfer_s>().pack(bref));
      break;
    case types::fail_ind:
      HANDLE_CODE(c.get<fail_ind_s>().pack(bref));
      break;
    case types::ho_report:
      HANDLE_CODE(c.get<ho_report_s>().pack(bref));
      break;
    case types::res_status_upd:
      HANDLE_CODE(c.get<res_status_upd_s>().pack(bref));
      break;
    case types::access_and_mob_ind:
      HANDLE_CODE(c.get<access_and_mob_ind_s>().pack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().pack(bref));
      break;
    case types::ran_multicast_group_paging:
      HANDLE_CODE(c.get<ran_multicast_group_paging_s>().pack(bref));
      break;
    case types::scg_fail_info_report:
      HANDLE_CODE(c.get<scg_fail_info_report_s>().pack(bref));
      break;
    case types::scg_fail_transfer:
      HANDLE_CODE(c.get<scg_fail_transfer_s>().pack(bref));
      break;
    case types::f1_c_traffic_transfer:
      HANDLE_CODE(c.get<f1_c_traffic_transfer_s>().pack(bref));
      break;
    case types::retrieve_ue_context_confirm:
      HANDLE_CODE(c.get<retrieve_ue_context_confirm_s>().pack(bref));
      break;
    case types::cp_c_cancel:
      HANDLE_CODE(c.get<cp_c_cancel_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_2_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_class_2_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::sn_status_transfer:
      HANDLE_CODE(c.get<sn_status_transfer_s>().unpack(bref));
      break;
    case types::ho_cancel:
      HANDLE_CODE(c.get<ho_cancel_s>().unpack(bref));
      break;
    case types::ran_paging:
      HANDLE_CODE(c.get<ran_paging_s>().unpack(bref));
      break;
    case types::xn_u_address_ind:
      HANDLE_CODE(c.get<xn_u_address_ind_s>().unpack(bref));
      break;
    case types::ue_context_release:
      HANDLE_CODE(c.get<ue_context_release_s>().unpack(bref));
      break;
    case types::sn_ode_recfg_complete:
      HANDLE_CODE(c.get<sn_ode_recfg_complete_s>().unpack(bref));
      break;
    case types::sn_ode_counter_check_request:
      HANDLE_CODE(c.get<sn_ode_counter_check_request_s>().unpack(bref));
      break;
    case types::rrc_transfer:
      HANDLE_CODE(c.get<rrc_transfer_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().unpack(bref));
      break;
    case types::notif_ctrl_ind:
      HANDLE_CODE(c.get<notif_ctrl_ind_s>().unpack(bref));
      break;
    case types::activity_notif:
      HANDLE_CODE(c.get<activity_notif_s>().unpack(bref));
      break;
    case types::secondary_rat_data_usage_report:
      HANDLE_CODE(c.get<secondary_rat_data_usage_report_s>().unpack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().unpack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().unpack(bref));
      break;
    case types::ho_success:
      HANDLE_CODE(c.get<ho_success_s>().unpack(bref));
      break;
    case types::conditional_ho_cancel:
      HANDLE_CODE(c.get<conditional_ho_cancel_s>().unpack(bref));
      break;
    case types::early_status_transfer:
      HANDLE_CODE(c.get<early_status_transfer_s>().unpack(bref));
      break;
    case types::fail_ind:
      HANDLE_CODE(c.get<fail_ind_s>().unpack(bref));
      break;
    case types::ho_report:
      HANDLE_CODE(c.get<ho_report_s>().unpack(bref));
      break;
    case types::res_status_upd:
      HANDLE_CODE(c.get<res_status_upd_s>().unpack(bref));
      break;
    case types::access_and_mob_ind:
      HANDLE_CODE(c.get<access_and_mob_ind_s>().unpack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().unpack(bref));
      break;
    case types::ran_multicast_group_paging:
      HANDLE_CODE(c.get<ran_multicast_group_paging_s>().unpack(bref));
      break;
    case types::scg_fail_info_report:
      HANDLE_CODE(c.get<scg_fail_info_report_s>().unpack(bref));
      break;
    case types::scg_fail_transfer:
      HANDLE_CODE(c.get<scg_fail_transfer_s>().unpack(bref));
      break;
    case types::f1_c_traffic_transfer:
      HANDLE_CODE(c.get<f1_c_traffic_transfer_s>().unpack(bref));
      break;
    case types::retrieve_ue_context_confirm:
      HANDLE_CODE(c.get<retrieve_ue_context_confirm_s>().unpack(bref));
      break;
    case types::cp_c_cancel:
      HANDLE_CODE(c.get<cp_c_cancel_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_class_2_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_class_2_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"SNStatusTransfer",
                                "HandoverCancel",
                                "RANPaging",
                                "XnUAddressIndication",
                                "UEContextRelease",
                                "SNodeReconfigurationComplete",
                                "SNodeCounterCheckRequest",
                                "RRCTransfer",
                                "ErrorIndication",
                                "PrivateMessage",
                                "NotificationControlIndication",
                                "ActivityNotification",
                                "SecondaryRATDataUsageReport",
                                "DeactivateTrace",
                                "TraceStart",
                                "HandoverSuccess",
                                "ConditionalHandoverCancel",
                                "EarlyStatusTransfer",
                                "FailureIndication",
                                "HandoverReport",
                                "ResourceStatusUpdate",
                                "AccessAndMobilityIndication",
                                "CellTrafficTrace",
                                "RANMulticastGroupPaging",
                                "ScgFailureInformationReport",
                                "ScgFailureTransfer",
                                "F1CTrafficTransfer",
                                "RetrieveUEContextConfirm",
                                "CPCCancel"};
  return convert_enum_idx(names, 29, value, "xnap_elem_procs_class_2_o::init_msg_c::types");
}
uint8_t xnap_elem_procs_class_2_o::init_msg_c::types_opts::to_number() const
{
  if (value == f1_c_traffic_transfer) {
    return 1;
  }
  invalid_enum_number(value, "xnap_elem_procs_class_2_o::init_msg_c::types");
  return 0;
}

// SuccessfulOutcome ::= OPEN TYPE
void xnap_elem_procs_class_2_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_class_2_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_class_2_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_class_2_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {};
  return convert_enum_idx(names, 0, value, "xnap_elem_procs_class_2_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void xnap_elem_procs_class_2_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_class_2_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_class_2_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {};
  return convert_enum_idx(names, 0, value, "xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types");
}

// XNAP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
uint16_t xnap_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {0,  3,  7,  9,  10, 11, 12, 14, 16, 17, 18, 25, 19, 20, 34, 36, 44,
                                   45, 46, 49, 1,  2,  4,  5,  6,  8,  13, 15, 21, 22, 23, 24, 26, 27,
                                   28, 29, 30, 31, 32, 33, 35, 37, 38, 39, 40, 42, 43, 47, 48};
  return map_enum_number(names, 49, idx, "proc_code");
}
bool xnap_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {0,  3,  7,  9,  10, 11, 12, 14, 16, 17, 18, 25, 19, 20, 34, 36, 44,
                                   45, 46, 49, 1,  2,  4,  5,  6,  8,  13, 15, 21, 22, 23, 24, 26, 27,
                                   28, 29, 30, 31, 32, 33, 35, 37, 38, 39, 40, 42, 43, 47, 48};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
xnap_elem_procs_o::init_msg_c xnap_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(init_msg_c::types::ho_request);
      break;
    case 3:
      ret.set(init_msg_c::types::retrieve_ue_context_request);
      break;
    case 7:
      ret.set(init_msg_c::types::sn_ode_addition_request);
      break;
    case 9:
      ret.set(init_msg_c::types::sn_ode_mod_request);
      break;
    case 10:
      ret.set(init_msg_c::types::sn_ode_mod_required);
      break;
    case 11:
      ret.set(init_msg_c::types::sn_ode_release_request);
      break;
    case 12:
      ret.set(init_msg_c::types::sn_ode_release_required);
      break;
    case 14:
      ret.set(init_msg_c::types::sn_ode_change_required);
      break;
    case 16:
      ret.set(init_msg_c::types::xn_removal_request);
      break;
    case 17:
      ret.set(init_msg_c::types::xn_setup_request);
      break;
    case 18:
      ret.set(init_msg_c::types::ngran_node_cfg_upd);
      break;
    case 25:
      ret.set(init_msg_c::types::e_utra_nr_cell_res_coordination_request);
      break;
    case 19:
      ret.set(init_msg_c::types::cell_activation_request);
      break;
    case 20:
      ret.set(init_msg_c::types::reset_request);
      break;
    case 34:
      ret.set(init_msg_c::types::res_status_request);
      break;
    case 36:
      ret.set(init_msg_c::types::mob_change_request);
      break;
    case 44:
      ret.set(init_msg_c::types::iab_transport_migration_management_request);
      break;
    case 45:
      ret.set(init_msg_c::types::iab_transport_migration_mod_request);
      break;
    case 46:
      ret.set(init_msg_c::types::iab_res_coordination_request);
      break;
    case 49:
      ret.set(init_msg_c::types::partial_ue_context_transfer);
      break;
    case 1:
      ret.set(init_msg_c::types::sn_status_transfer);
      break;
    case 2:
      ret.set(init_msg_c::types::ho_cancel);
      break;
    case 4:
      ret.set(init_msg_c::types::ran_paging);
      break;
    case 5:
      ret.set(init_msg_c::types::xn_u_address_ind);
      break;
    case 6:
      ret.set(init_msg_c::types::ue_context_release);
      break;
    case 8:
      ret.set(init_msg_c::types::sn_ode_recfg_complete);
      break;
    case 13:
      ret.set(init_msg_c::types::sn_ode_counter_check_request);
      break;
    case 15:
      ret.set(init_msg_c::types::rrc_transfer);
      break;
    case 21:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 22:
      ret.set(init_msg_c::types::private_msg);
      break;
    case 23:
      ret.set(init_msg_c::types::notif_ctrl_ind);
      break;
    case 24:
      ret.set(init_msg_c::types::activity_notif);
      break;
    case 26:
      ret.set(init_msg_c::types::secondary_rat_data_usage_report);
      break;
    case 27:
      ret.set(init_msg_c::types::deactiv_trace);
      break;
    case 28:
      ret.set(init_msg_c::types::trace_start);
      break;
    case 29:
      ret.set(init_msg_c::types::ho_success);
      break;
    case 30:
      ret.set(init_msg_c::types::conditional_ho_cancel);
      break;
    case 31:
      ret.set(init_msg_c::types::early_status_transfer);
      break;
    case 32:
      ret.set(init_msg_c::types::fail_ind);
      break;
    case 33:
      ret.set(init_msg_c::types::ho_report);
      break;
    case 35:
      ret.set(init_msg_c::types::res_status_upd);
      break;
    case 37:
      ret.set(init_msg_c::types::access_and_mob_ind);
      break;
    case 38:
      ret.set(init_msg_c::types::cell_traffic_trace);
      break;
    case 39:
      ret.set(init_msg_c::types::ran_multicast_group_paging);
      break;
    case 40:
      ret.set(init_msg_c::types::scg_fail_info_report);
      break;
    case 42:
      ret.set(init_msg_c::types::scg_fail_transfer);
      break;
    case 43:
      ret.set(init_msg_c::types::f1_c_traffic_transfer);
      break;
    case 47:
      ret.set(init_msg_c::types::retrieve_ue_context_confirm);
      break;
    case 48:
      ret.set(init_msg_c::types::cp_c_cancel);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
xnap_elem_procs_o::successful_outcome_c xnap_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(successful_outcome_c::types::ho_request_ack);
      break;
    case 3:
      ret.set(successful_outcome_c::types::retrieve_ue_context_resp);
      break;
    case 7:
      ret.set(successful_outcome_c::types::sn_ode_addition_request_ack);
      break;
    case 9:
      ret.set(successful_outcome_c::types::sn_ode_mod_request_ack);
      break;
    case 10:
      ret.set(successful_outcome_c::types::sn_ode_mod_confirm);
      break;
    case 11:
      ret.set(successful_outcome_c::types::sn_ode_release_request_ack);
      break;
    case 12:
      ret.set(successful_outcome_c::types::sn_ode_release_confirm);
      break;
    case 14:
      ret.set(successful_outcome_c::types::sn_ode_change_confirm);
      break;
    case 16:
      ret.set(successful_outcome_c::types::xn_removal_resp);
      break;
    case 17:
      ret.set(successful_outcome_c::types::xn_setup_resp);
      break;
    case 18:
      ret.set(successful_outcome_c::types::ngran_node_cfg_upd_ack);
      break;
    case 25:
      ret.set(successful_outcome_c::types::e_utra_nr_cell_res_coordination_resp);
      break;
    case 19:
      ret.set(successful_outcome_c::types::cell_activation_resp);
      break;
    case 20:
      ret.set(successful_outcome_c::types::reset_resp);
      break;
    case 34:
      ret.set(successful_outcome_c::types::res_status_resp);
      break;
    case 36:
      ret.set(successful_outcome_c::types::mob_change_ack);
      break;
    case 44:
      ret.set(successful_outcome_c::types::iab_transport_migration_management_resp);
      break;
    case 45:
      ret.set(successful_outcome_c::types::iab_transport_migration_mod_resp);
      break;
    case 46:
      ret.set(successful_outcome_c::types::iab_res_coordination_resp);
      break;
    case 49:
      ret.set(successful_outcome_c::types::partial_ue_context_transfer_ack);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
xnap_elem_procs_o::unsuccessful_outcome_c xnap_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(unsuccessful_outcome_c::types::ho_prep_fail);
      break;
    case 3:
      ret.set(unsuccessful_outcome_c::types::retrieve_ue_context_fail);
      break;
    case 7:
      ret.set(unsuccessful_outcome_c::types::sn_ode_addition_request_reject);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::sn_ode_mod_request_reject);
      break;
    case 10:
      ret.set(unsuccessful_outcome_c::types::sn_ode_mod_refuse);
      break;
    case 11:
      ret.set(unsuccessful_outcome_c::types::sn_ode_release_reject);
      break;
    case 14:
      ret.set(unsuccessful_outcome_c::types::sn_ode_change_refuse);
      break;
    case 16:
      ret.set(unsuccessful_outcome_c::types::xn_removal_fail);
      break;
    case 17:
      ret.set(unsuccessful_outcome_c::types::xn_setup_fail);
      break;
    case 18:
      ret.set(unsuccessful_outcome_c::types::ngran_node_cfg_upd_fail);
      break;
    case 19:
      ret.set(unsuccessful_outcome_c::types::cell_activation_fail);
      break;
    case 34:
      ret.set(unsuccessful_outcome_c::types::res_status_fail);
      break;
    case 36:
      ret.set(unsuccessful_outcome_c::types::mob_change_fail);
      break;
    case 44:
      ret.set(unsuccessful_outcome_c::types::iab_transport_migration_management_reject);
      break;
    case 49:
      ret.set(unsuccessful_outcome_c::types::partial_ue_context_transfer_fail);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e xnap_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 0:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 12:
      return crit_e::reject;
    case 14:
      return crit_e::reject;
    case 16:
      return crit_e::reject;
    case 17:
      return crit_e::reject;
    case 18:
      return crit_e::reject;
    case 25:
      return crit_e::reject;
    case 19:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 34:
      return crit_e::reject;
    case 36:
      return crit_e::reject;
    case 44:
      return crit_e::reject;
    case 45:
      return crit_e::reject;
    case 46:
      return crit_e::reject;
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 4:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 6:
      return crit_e::reject;
    case 8:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 15:
      return crit_e::reject;
    case 21:
      return crit_e::ignore;
    case 22:
      return crit_e::ignore;
    case 23:
      return crit_e::ignore;
    case 24:
      return crit_e::ignore;
    case 26:
      return crit_e::reject;
    case 27:
      return crit_e::ignore;
    case 28:
      return crit_e::ignore;
    case 29:
      return crit_e::ignore;
    case 30:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 32:
      return crit_e::ignore;
    case 33:
      return crit_e::ignore;
    case 35:
      return crit_e::ignore;
    case 37:
      return crit_e::ignore;
    case 38:
      return crit_e::ignore;
    case 39:
      return crit_e::reject;
    case 40:
      return crit_e::ignore;
    case 42:
      return crit_e::ignore;
    case 43:
      return crit_e::reject;
    case 47:
      return crit_e::ignore;
    case 48:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void xnap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ho_request:
      c = ho_request_s{};
      break;
    case types::retrieve_ue_context_request:
      c = retrieve_ue_context_request_s{};
      break;
    case types::sn_ode_addition_request:
      c = sn_ode_addition_request_s{};
      break;
    case types::sn_ode_mod_request:
      c = sn_ode_mod_request_s{};
      break;
    case types::sn_ode_mod_required:
      c = sn_ode_mod_required_s{};
      break;
    case types::sn_ode_release_request:
      c = sn_ode_release_request_s{};
      break;
    case types::sn_ode_release_required:
      c = sn_ode_release_required_s{};
      break;
    case types::sn_ode_change_required:
      c = sn_ode_change_required_s{};
      break;
    case types::xn_removal_request:
      c = xn_removal_request_s{};
      break;
    case types::xn_setup_request:
      c = xn_setup_request_s{};
      break;
    case types::ngran_node_cfg_upd:
      c = ngran_node_cfg_upd_s{};
      break;
    case types::e_utra_nr_cell_res_coordination_request:
      c = e_utra_nr_cell_res_coordination_request_s{};
      break;
    case types::cell_activation_request:
      c = cell_activation_request_s{};
      break;
    case types::reset_request:
      c = reset_request_s{};
      break;
    case types::res_status_request:
      c = res_status_request_s{};
      break;
    case types::mob_change_request:
      c = mob_change_request_s{};
      break;
    case types::iab_transport_migration_management_request:
      c = iab_transport_migration_management_request_s{};
      break;
    case types::iab_transport_migration_mod_request:
      c = iab_transport_migration_mod_request_s{};
      break;
    case types::iab_res_coordination_request:
      c = iab_res_coordination_request_s{};
      break;
    case types::partial_ue_context_transfer:
      c = partial_ue_context_transfer_s{};
      break;
    case types::sn_status_transfer:
      c = sn_status_transfer_s{};
      break;
    case types::ho_cancel:
      c = ho_cancel_s{};
      break;
    case types::ran_paging:
      c = ran_paging_s{};
      break;
    case types::xn_u_address_ind:
      c = xn_u_address_ind_s{};
      break;
    case types::ue_context_release:
      c = ue_context_release_s{};
      break;
    case types::sn_ode_recfg_complete:
      c = sn_ode_recfg_complete_s{};
      break;
    case types::sn_ode_counter_check_request:
      c = sn_ode_counter_check_request_s{};
      break;
    case types::rrc_transfer:
      c = rrc_transfer_s{};
      break;
    case types::error_ind:
      c = error_ind_s{};
      break;
    case types::private_msg:
      c = private_msg_s{};
      break;
    case types::notif_ctrl_ind:
      c = notif_ctrl_ind_s{};
      break;
    case types::activity_notif:
      c = activity_notif_s{};
      break;
    case types::secondary_rat_data_usage_report:
      c = secondary_rat_data_usage_report_s{};
      break;
    case types::deactiv_trace:
      c = deactiv_trace_s{};
      break;
    case types::trace_start:
      c = trace_start_s{};
      break;
    case types::ho_success:
      c = ho_success_s{};
      break;
    case types::conditional_ho_cancel:
      c = conditional_ho_cancel_s{};
      break;
    case types::early_status_transfer:
      c = early_status_transfer_s{};
      break;
    case types::fail_ind:
      c = fail_ind_s{};
      break;
    case types::ho_report:
      c = ho_report_s{};
      break;
    case types::res_status_upd:
      c = res_status_upd_s{};
      break;
    case types::access_and_mob_ind:
      c = access_and_mob_ind_s{};
      break;
    case types::cell_traffic_trace:
      c = cell_traffic_trace_s{};
      break;
    case types::ran_multicast_group_paging:
      c = ran_multicast_group_paging_s{};
      break;
    case types::scg_fail_info_report:
      c = scg_fail_info_report_s{};
      break;
    case types::scg_fail_transfer:
      c = scg_fail_transfer_s{};
      break;
    case types::f1_c_traffic_transfer:
      c = f1_c_traffic_transfer_s{};
      break;
    case types::retrieve_ue_context_confirm:
      c = retrieve_ue_context_confirm_s{};
      break;
    case types::cp_c_cancel:
      c = cp_c_cancel_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::init_msg_c");
  }
}
ho_request_s& xnap_elem_procs_o::init_msg_c::ho_request()
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
retrieve_ue_context_request_s& xnap_elem_procs_o::init_msg_c::retrieve_ue_context_request()
{
  assert_choice_type(types::retrieve_ue_context_request, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_request_s>();
}
sn_ode_addition_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_addition_request()
{
  assert_choice_type(types::sn_ode_addition_request, type_, "InitiatingMessage");
  return c.get<sn_ode_addition_request_s>();
}
sn_ode_mod_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_mod_request()
{
  assert_choice_type(types::sn_ode_mod_request, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_request_s>();
}
sn_ode_mod_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_mod_required()
{
  assert_choice_type(types::sn_ode_mod_required, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_required_s>();
}
sn_ode_release_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_release_request()
{
  assert_choice_type(types::sn_ode_release_request, type_, "InitiatingMessage");
  return c.get<sn_ode_release_request_s>();
}
sn_ode_release_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_release_required()
{
  assert_choice_type(types::sn_ode_release_required, type_, "InitiatingMessage");
  return c.get<sn_ode_release_required_s>();
}
sn_ode_change_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_change_required()
{
  assert_choice_type(types::sn_ode_change_required, type_, "InitiatingMessage");
  return c.get<sn_ode_change_required_s>();
}
xn_removal_request_s& xnap_elem_procs_o::init_msg_c::xn_removal_request()
{
  assert_choice_type(types::xn_removal_request, type_, "InitiatingMessage");
  return c.get<xn_removal_request_s>();
}
xn_setup_request_s& xnap_elem_procs_o::init_msg_c::xn_setup_request()
{
  assert_choice_type(types::xn_setup_request, type_, "InitiatingMessage");
  return c.get<xn_setup_request_s>();
}
ngran_node_cfg_upd_s& xnap_elem_procs_o::init_msg_c::ngran_node_cfg_upd()
{
  assert_choice_type(types::ngran_node_cfg_upd, type_, "InitiatingMessage");
  return c.get<ngran_node_cfg_upd_s>();
}
e_utra_nr_cell_res_coordination_request_s& xnap_elem_procs_o::init_msg_c::e_utra_nr_cell_res_coordination_request()
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_request, type_, "InitiatingMessage");
  return c.get<e_utra_nr_cell_res_coordination_request_s>();
}
cell_activation_request_s& xnap_elem_procs_o::init_msg_c::cell_activation_request()
{
  assert_choice_type(types::cell_activation_request, type_, "InitiatingMessage");
  return c.get<cell_activation_request_s>();
}
reset_request_s& xnap_elem_procs_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
res_status_request_s& xnap_elem_procs_o::init_msg_c::res_status_request()
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
mob_change_request_s& xnap_elem_procs_o::init_msg_c::mob_change_request()
{
  assert_choice_type(types::mob_change_request, type_, "InitiatingMessage");
  return c.get<mob_change_request_s>();
}
iab_transport_migration_management_request_s&
xnap_elem_procs_o::init_msg_c::iab_transport_migration_management_request()
{
  assert_choice_type(types::iab_transport_migration_management_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_management_request_s>();
}
iab_transport_migration_mod_request_s& xnap_elem_procs_o::init_msg_c::iab_transport_migration_mod_request()
{
  assert_choice_type(types::iab_transport_migration_mod_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_mod_request_s>();
}
iab_res_coordination_request_s& xnap_elem_procs_o::init_msg_c::iab_res_coordination_request()
{
  assert_choice_type(types::iab_res_coordination_request, type_, "InitiatingMessage");
  return c.get<iab_res_coordination_request_s>();
}
partial_ue_context_transfer_s& xnap_elem_procs_o::init_msg_c::partial_ue_context_transfer()
{
  assert_choice_type(types::partial_ue_context_transfer, type_, "InitiatingMessage");
  return c.get<partial_ue_context_transfer_s>();
}
sn_status_transfer_s& xnap_elem_procs_o::init_msg_c::sn_status_transfer()
{
  assert_choice_type(types::sn_status_transfer, type_, "InitiatingMessage");
  return c.get<sn_status_transfer_s>();
}
ho_cancel_s& xnap_elem_procs_o::init_msg_c::ho_cancel()
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
ran_paging_s& xnap_elem_procs_o::init_msg_c::ran_paging()
{
  assert_choice_type(types::ran_paging, type_, "InitiatingMessage");
  return c.get<ran_paging_s>();
}
xn_u_address_ind_s& xnap_elem_procs_o::init_msg_c::xn_u_address_ind()
{
  assert_choice_type(types::xn_u_address_ind, type_, "InitiatingMessage");
  return c.get<xn_u_address_ind_s>();
}
ue_context_release_s& xnap_elem_procs_o::init_msg_c::ue_context_release()
{
  assert_choice_type(types::ue_context_release, type_, "InitiatingMessage");
  return c.get<ue_context_release_s>();
}
sn_ode_recfg_complete_s& xnap_elem_procs_o::init_msg_c::sn_ode_recfg_complete()
{
  assert_choice_type(types::sn_ode_recfg_complete, type_, "InitiatingMessage");
  return c.get<sn_ode_recfg_complete_s>();
}
sn_ode_counter_check_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_counter_check_request()
{
  assert_choice_type(types::sn_ode_counter_check_request, type_, "InitiatingMessage");
  return c.get<sn_ode_counter_check_request_s>();
}
rrc_transfer_s& xnap_elem_procs_o::init_msg_c::rrc_transfer()
{
  assert_choice_type(types::rrc_transfer, type_, "InitiatingMessage");
  return c.get<rrc_transfer_s>();
}
error_ind_s& xnap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
private_msg_s& xnap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
notif_ctrl_ind_s& xnap_elem_procs_o::init_msg_c::notif_ctrl_ind()
{
  assert_choice_type(types::notif_ctrl_ind, type_, "InitiatingMessage");
  return c.get<notif_ctrl_ind_s>();
}
activity_notif_s& xnap_elem_procs_o::init_msg_c::activity_notif()
{
  assert_choice_type(types::activity_notif, type_, "InitiatingMessage");
  return c.get<activity_notif_s>();
}
secondary_rat_data_usage_report_s& xnap_elem_procs_o::init_msg_c::secondary_rat_data_usage_report()
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
deactiv_trace_s& xnap_elem_procs_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
trace_start_s& xnap_elem_procs_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
ho_success_s& xnap_elem_procs_o::init_msg_c::ho_success()
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
conditional_ho_cancel_s& xnap_elem_procs_o::init_msg_c::conditional_ho_cancel()
{
  assert_choice_type(types::conditional_ho_cancel, type_, "InitiatingMessage");
  return c.get<conditional_ho_cancel_s>();
}
early_status_transfer_s& xnap_elem_procs_o::init_msg_c::early_status_transfer()
{
  assert_choice_type(types::early_status_transfer, type_, "InitiatingMessage");
  return c.get<early_status_transfer_s>();
}
fail_ind_s& xnap_elem_procs_o::init_msg_c::fail_ind()
{
  assert_choice_type(types::fail_ind, type_, "InitiatingMessage");
  return c.get<fail_ind_s>();
}
ho_report_s& xnap_elem_procs_o::init_msg_c::ho_report()
{
  assert_choice_type(types::ho_report, type_, "InitiatingMessage");
  return c.get<ho_report_s>();
}
res_status_upd_s& xnap_elem_procs_o::init_msg_c::res_status_upd()
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
access_and_mob_ind_s& xnap_elem_procs_o::init_msg_c::access_and_mob_ind()
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
cell_traffic_trace_s& xnap_elem_procs_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
ran_multicast_group_paging_s& xnap_elem_procs_o::init_msg_c::ran_multicast_group_paging()
{
  assert_choice_type(types::ran_multicast_group_paging, type_, "InitiatingMessage");
  return c.get<ran_multicast_group_paging_s>();
}
scg_fail_info_report_s& xnap_elem_procs_o::init_msg_c::scg_fail_info_report()
{
  assert_choice_type(types::scg_fail_info_report, type_, "InitiatingMessage");
  return c.get<scg_fail_info_report_s>();
}
scg_fail_transfer_s& xnap_elem_procs_o::init_msg_c::scg_fail_transfer()
{
  assert_choice_type(types::scg_fail_transfer, type_, "InitiatingMessage");
  return c.get<scg_fail_transfer_s>();
}
f1_c_traffic_transfer_s& xnap_elem_procs_o::init_msg_c::f1_c_traffic_transfer()
{
  assert_choice_type(types::f1_c_traffic_transfer, type_, "InitiatingMessage");
  return c.get<f1_c_traffic_transfer_s>();
}
retrieve_ue_context_confirm_s& xnap_elem_procs_o::init_msg_c::retrieve_ue_context_confirm()
{
  assert_choice_type(types::retrieve_ue_context_confirm, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_confirm_s>();
}
cp_c_cancel_s& xnap_elem_procs_o::init_msg_c::cp_c_cancel()
{
  assert_choice_type(types::cp_c_cancel, type_, "InitiatingMessage");
  return c.get<cp_c_cancel_s>();
}
const ho_request_s& xnap_elem_procs_o::init_msg_c::ho_request() const
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
const retrieve_ue_context_request_s& xnap_elem_procs_o::init_msg_c::retrieve_ue_context_request() const
{
  assert_choice_type(types::retrieve_ue_context_request, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_request_s>();
}
const sn_ode_addition_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_addition_request() const
{
  assert_choice_type(types::sn_ode_addition_request, type_, "InitiatingMessage");
  return c.get<sn_ode_addition_request_s>();
}
const sn_ode_mod_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_mod_request() const
{
  assert_choice_type(types::sn_ode_mod_request, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_request_s>();
}
const sn_ode_mod_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_mod_required() const
{
  assert_choice_type(types::sn_ode_mod_required, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_required_s>();
}
const sn_ode_release_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_release_request() const
{
  assert_choice_type(types::sn_ode_release_request, type_, "InitiatingMessage");
  return c.get<sn_ode_release_request_s>();
}
const sn_ode_release_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_release_required() const
{
  assert_choice_type(types::sn_ode_release_required, type_, "InitiatingMessage");
  return c.get<sn_ode_release_required_s>();
}
const sn_ode_change_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_change_required() const
{
  assert_choice_type(types::sn_ode_change_required, type_, "InitiatingMessage");
  return c.get<sn_ode_change_required_s>();
}
const xn_removal_request_s& xnap_elem_procs_o::init_msg_c::xn_removal_request() const
{
  assert_choice_type(types::xn_removal_request, type_, "InitiatingMessage");
  return c.get<xn_removal_request_s>();
}
const xn_setup_request_s& xnap_elem_procs_o::init_msg_c::xn_setup_request() const
{
  assert_choice_type(types::xn_setup_request, type_, "InitiatingMessage");
  return c.get<xn_setup_request_s>();
}
const ngran_node_cfg_upd_s& xnap_elem_procs_o::init_msg_c::ngran_node_cfg_upd() const
{
  assert_choice_type(types::ngran_node_cfg_upd, type_, "InitiatingMessage");
  return c.get<ngran_node_cfg_upd_s>();
}
const e_utra_nr_cell_res_coordination_request_s&
xnap_elem_procs_o::init_msg_c::e_utra_nr_cell_res_coordination_request() const
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_request, type_, "InitiatingMessage");
  return c.get<e_utra_nr_cell_res_coordination_request_s>();
}
const cell_activation_request_s& xnap_elem_procs_o::init_msg_c::cell_activation_request() const
{
  assert_choice_type(types::cell_activation_request, type_, "InitiatingMessage");
  return c.get<cell_activation_request_s>();
}
const reset_request_s& xnap_elem_procs_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const res_status_request_s& xnap_elem_procs_o::init_msg_c::res_status_request() const
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
const mob_change_request_s& xnap_elem_procs_o::init_msg_c::mob_change_request() const
{
  assert_choice_type(types::mob_change_request, type_, "InitiatingMessage");
  return c.get<mob_change_request_s>();
}
const iab_transport_migration_management_request_s&
xnap_elem_procs_o::init_msg_c::iab_transport_migration_management_request() const
{
  assert_choice_type(types::iab_transport_migration_management_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_management_request_s>();
}
const iab_transport_migration_mod_request_s& xnap_elem_procs_o::init_msg_c::iab_transport_migration_mod_request() const
{
  assert_choice_type(types::iab_transport_migration_mod_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_mod_request_s>();
}
const iab_res_coordination_request_s& xnap_elem_procs_o::init_msg_c::iab_res_coordination_request() const
{
  assert_choice_type(types::iab_res_coordination_request, type_, "InitiatingMessage");
  return c.get<iab_res_coordination_request_s>();
}
const partial_ue_context_transfer_s& xnap_elem_procs_o::init_msg_c::partial_ue_context_transfer() const
{
  assert_choice_type(types::partial_ue_context_transfer, type_, "InitiatingMessage");
  return c.get<partial_ue_context_transfer_s>();
}
const sn_status_transfer_s& xnap_elem_procs_o::init_msg_c::sn_status_transfer() const
{
  assert_choice_type(types::sn_status_transfer, type_, "InitiatingMessage");
  return c.get<sn_status_transfer_s>();
}
const ho_cancel_s& xnap_elem_procs_o::init_msg_c::ho_cancel() const
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
const ran_paging_s& xnap_elem_procs_o::init_msg_c::ran_paging() const
{
  assert_choice_type(types::ran_paging, type_, "InitiatingMessage");
  return c.get<ran_paging_s>();
}
const xn_u_address_ind_s& xnap_elem_procs_o::init_msg_c::xn_u_address_ind() const
{
  assert_choice_type(types::xn_u_address_ind, type_, "InitiatingMessage");
  return c.get<xn_u_address_ind_s>();
}
const ue_context_release_s& xnap_elem_procs_o::init_msg_c::ue_context_release() const
{
  assert_choice_type(types::ue_context_release, type_, "InitiatingMessage");
  return c.get<ue_context_release_s>();
}
const sn_ode_recfg_complete_s& xnap_elem_procs_o::init_msg_c::sn_ode_recfg_complete() const
{
  assert_choice_type(types::sn_ode_recfg_complete, type_, "InitiatingMessage");
  return c.get<sn_ode_recfg_complete_s>();
}
const sn_ode_counter_check_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_counter_check_request() const
{
  assert_choice_type(types::sn_ode_counter_check_request, type_, "InitiatingMessage");
  return c.get<sn_ode_counter_check_request_s>();
}
const rrc_transfer_s& xnap_elem_procs_o::init_msg_c::rrc_transfer() const
{
  assert_choice_type(types::rrc_transfer, type_, "InitiatingMessage");
  return c.get<rrc_transfer_s>();
}
const error_ind_s& xnap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const private_msg_s& xnap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const notif_ctrl_ind_s& xnap_elem_procs_o::init_msg_c::notif_ctrl_ind() const
{
  assert_choice_type(types::notif_ctrl_ind, type_, "InitiatingMessage");
  return c.get<notif_ctrl_ind_s>();
}
const activity_notif_s& xnap_elem_procs_o::init_msg_c::activity_notif() const
{
  assert_choice_type(types::activity_notif, type_, "InitiatingMessage");
  return c.get<activity_notif_s>();
}
const secondary_rat_data_usage_report_s& xnap_elem_procs_o::init_msg_c::secondary_rat_data_usage_report() const
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
const deactiv_trace_s& xnap_elem_procs_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const trace_start_s& xnap_elem_procs_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const ho_success_s& xnap_elem_procs_o::init_msg_c::ho_success() const
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
const conditional_ho_cancel_s& xnap_elem_procs_o::init_msg_c::conditional_ho_cancel() const
{
  assert_choice_type(types::conditional_ho_cancel, type_, "InitiatingMessage");
  return c.get<conditional_ho_cancel_s>();
}
const early_status_transfer_s& xnap_elem_procs_o::init_msg_c::early_status_transfer() const
{
  assert_choice_type(types::early_status_transfer, type_, "InitiatingMessage");
  return c.get<early_status_transfer_s>();
}
const fail_ind_s& xnap_elem_procs_o::init_msg_c::fail_ind() const
{
  assert_choice_type(types::fail_ind, type_, "InitiatingMessage");
  return c.get<fail_ind_s>();
}
const ho_report_s& xnap_elem_procs_o::init_msg_c::ho_report() const
{
  assert_choice_type(types::ho_report, type_, "InitiatingMessage");
  return c.get<ho_report_s>();
}
const res_status_upd_s& xnap_elem_procs_o::init_msg_c::res_status_upd() const
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
const access_and_mob_ind_s& xnap_elem_procs_o::init_msg_c::access_and_mob_ind() const
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
const cell_traffic_trace_s& xnap_elem_procs_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const ran_multicast_group_paging_s& xnap_elem_procs_o::init_msg_c::ran_multicast_group_paging() const
{
  assert_choice_type(types::ran_multicast_group_paging, type_, "InitiatingMessage");
  return c.get<ran_multicast_group_paging_s>();
}
const scg_fail_info_report_s& xnap_elem_procs_o::init_msg_c::scg_fail_info_report() const
{
  assert_choice_type(types::scg_fail_info_report, type_, "InitiatingMessage");
  return c.get<scg_fail_info_report_s>();
}
const scg_fail_transfer_s& xnap_elem_procs_o::init_msg_c::scg_fail_transfer() const
{
  assert_choice_type(types::scg_fail_transfer, type_, "InitiatingMessage");
  return c.get<scg_fail_transfer_s>();
}
const f1_c_traffic_transfer_s& xnap_elem_procs_o::init_msg_c::f1_c_traffic_transfer() const
{
  assert_choice_type(types::f1_c_traffic_transfer, type_, "InitiatingMessage");
  return c.get<f1_c_traffic_transfer_s>();
}
const retrieve_ue_context_confirm_s& xnap_elem_procs_o::init_msg_c::retrieve_ue_context_confirm() const
{
  assert_choice_type(types::retrieve_ue_context_confirm, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_confirm_s>();
}
const cp_c_cancel_s& xnap_elem_procs_o::init_msg_c::cp_c_cancel() const
{
  assert_choice_type(types::cp_c_cancel, type_, "InitiatingMessage");
  return c.get<cp_c_cancel_s>();
}
void xnap_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_request:
      j.write_fieldname("HandoverRequest");
      c.get<ho_request_s>().to_json(j);
      break;
    case types::retrieve_ue_context_request:
      j.write_fieldname("RetrieveUEContextRequest");
      c.get<retrieve_ue_context_request_s>().to_json(j);
      break;
    case types::sn_ode_addition_request:
      j.write_fieldname("SNodeAdditionRequest");
      c.get<sn_ode_addition_request_s>().to_json(j);
      break;
    case types::sn_ode_mod_request:
      j.write_fieldname("SNodeModificationRequest");
      c.get<sn_ode_mod_request_s>().to_json(j);
      break;
    case types::sn_ode_mod_required:
      j.write_fieldname("SNodeModificationRequired");
      c.get<sn_ode_mod_required_s>().to_json(j);
      break;
    case types::sn_ode_release_request:
      j.write_fieldname("SNodeReleaseRequest");
      c.get<sn_ode_release_request_s>().to_json(j);
      break;
    case types::sn_ode_release_required:
      j.write_fieldname("SNodeReleaseRequired");
      c.get<sn_ode_release_required_s>().to_json(j);
      break;
    case types::sn_ode_change_required:
      j.write_fieldname("SNodeChangeRequired");
      c.get<sn_ode_change_required_s>().to_json(j);
      break;
    case types::xn_removal_request:
      j.write_fieldname("XnRemovalRequest");
      c.get<xn_removal_request_s>().to_json(j);
      break;
    case types::xn_setup_request:
      j.write_fieldname("XnSetupRequest");
      c.get<xn_setup_request_s>().to_json(j);
      break;
    case types::ngran_node_cfg_upd:
      j.write_fieldname("NGRANNodeConfigurationUpdate");
      c.get<ngran_node_cfg_upd_s>().to_json(j);
      break;
    case types::e_utra_nr_cell_res_coordination_request:
      j.write_fieldname("E-UTRA-NR-CellResourceCoordinationRequest");
      c.get<e_utra_nr_cell_res_coordination_request_s>().to_json(j);
      break;
    case types::cell_activation_request:
      j.write_fieldname("CellActivationRequest");
      c.get<cell_activation_request_s>().to_json(j);
      break;
    case types::reset_request:
      j.write_fieldname("ResetRequest");
      c.get<reset_request_s>().to_json(j);
      break;
    case types::res_status_request:
      j.write_fieldname("ResourceStatusRequest");
      c.get<res_status_request_s>().to_json(j);
      break;
    case types::mob_change_request:
      j.write_fieldname("MobilityChangeRequest");
      c.get<mob_change_request_s>().to_json(j);
      break;
    case types::iab_transport_migration_management_request:
      j.write_fieldname("IABTransportMigrationManagementRequest");
      c.get<iab_transport_migration_management_request_s>().to_json(j);
      break;
    case types::iab_transport_migration_mod_request:
      j.write_fieldname("IABTransportMigrationModificationRequest");
      c.get<iab_transport_migration_mod_request_s>().to_json(j);
      break;
    case types::iab_res_coordination_request:
      j.write_fieldname("IABResourceCoordinationRequest");
      c.get<iab_res_coordination_request_s>().to_json(j);
      break;
    case types::partial_ue_context_transfer:
      j.write_fieldname("PartialUEContextTransfer");
      c.get<partial_ue_context_transfer_s>().to_json(j);
      break;
    case types::sn_status_transfer:
      j.write_fieldname("SNStatusTransfer");
      c.get<sn_status_transfer_s>().to_json(j);
      break;
    case types::ho_cancel:
      j.write_fieldname("HandoverCancel");
      c.get<ho_cancel_s>().to_json(j);
      break;
    case types::ran_paging:
      j.write_fieldname("RANPaging");
      c.get<ran_paging_s>().to_json(j);
      break;
    case types::xn_u_address_ind:
      j.write_fieldname("XnUAddressIndication");
      c.get<xn_u_address_ind_s>().to_json(j);
      break;
    case types::ue_context_release:
      j.write_fieldname("UEContextRelease");
      c.get<ue_context_release_s>().to_json(j);
      break;
    case types::sn_ode_recfg_complete:
      j.write_fieldname("SNodeReconfigurationComplete");
      c.get<sn_ode_recfg_complete_s>().to_json(j);
      break;
    case types::sn_ode_counter_check_request:
      j.write_fieldname("SNodeCounterCheckRequest");
      c.get<sn_ode_counter_check_request_s>().to_json(j);
      break;
    case types::rrc_transfer:
      j.write_fieldname("RRCTransfer");
      c.get<rrc_transfer_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::private_msg:
      j.write_fieldname("PrivateMessage");
      c.get<private_msg_s>().to_json(j);
      break;
    case types::notif_ctrl_ind:
      j.write_fieldname("NotificationControlIndication");
      c.get<notif_ctrl_ind_s>().to_json(j);
      break;
    case types::activity_notif:
      j.write_fieldname("ActivityNotification");
      c.get<activity_notif_s>().to_json(j);
      break;
    case types::secondary_rat_data_usage_report:
      j.write_fieldname("SecondaryRATDataUsageReport");
      c.get<secondary_rat_data_usage_report_s>().to_json(j);
      break;
    case types::deactiv_trace:
      j.write_fieldname("DeactivateTrace");
      c.get<deactiv_trace_s>().to_json(j);
      break;
    case types::trace_start:
      j.write_fieldname("TraceStart");
      c.get<trace_start_s>().to_json(j);
      break;
    case types::ho_success:
      j.write_fieldname("HandoverSuccess");
      c.get<ho_success_s>().to_json(j);
      break;
    case types::conditional_ho_cancel:
      j.write_fieldname("ConditionalHandoverCancel");
      c.get<conditional_ho_cancel_s>().to_json(j);
      break;
    case types::early_status_transfer:
      j.write_fieldname("EarlyStatusTransfer");
      c.get<early_status_transfer_s>().to_json(j);
      break;
    case types::fail_ind:
      j.write_fieldname("FailureIndication");
      c.get<fail_ind_s>().to_json(j);
      break;
    case types::ho_report:
      j.write_fieldname("HandoverReport");
      c.get<ho_report_s>().to_json(j);
      break;
    case types::res_status_upd:
      j.write_fieldname("ResourceStatusUpdate");
      c.get<res_status_upd_s>().to_json(j);
      break;
    case types::access_and_mob_ind:
      j.write_fieldname("AccessAndMobilityIndication");
      c.get<access_and_mob_ind_s>().to_json(j);
      break;
    case types::cell_traffic_trace:
      j.write_fieldname("CellTrafficTrace");
      c.get<cell_traffic_trace_s>().to_json(j);
      break;
    case types::ran_multicast_group_paging:
      j.write_fieldname("RANMulticastGroupPaging");
      c.get<ran_multicast_group_paging_s>().to_json(j);
      break;
    case types::scg_fail_info_report:
      j.write_fieldname("ScgFailureInformationReport");
      c.get<scg_fail_info_report_s>().to_json(j);
      break;
    case types::scg_fail_transfer:
      j.write_fieldname("ScgFailureTransfer");
      c.get<scg_fail_transfer_s>().to_json(j);
      break;
    case types::f1_c_traffic_transfer:
      j.write_fieldname("F1CTrafficTransfer");
      c.get<f1_c_traffic_transfer_s>().to_json(j);
      break;
    case types::retrieve_ue_context_confirm:
      j.write_fieldname("RetrieveUEContextConfirm");
      c.get<retrieve_ue_context_confirm_s>().to_json(j);
      break;
    case types::cp_c_cancel:
      j.write_fieldname("CPCCancel");
      c.get<cp_c_cancel_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_request:
      HANDLE_CODE(c.get<ho_request_s>().pack(bref));
      break;
    case types::retrieve_ue_context_request:
      HANDLE_CODE(c.get<retrieve_ue_context_request_s>().pack(bref));
      break;
    case types::sn_ode_addition_request:
      HANDLE_CODE(c.get<sn_ode_addition_request_s>().pack(bref));
      break;
    case types::sn_ode_mod_request:
      HANDLE_CODE(c.get<sn_ode_mod_request_s>().pack(bref));
      break;
    case types::sn_ode_mod_required:
      HANDLE_CODE(c.get<sn_ode_mod_required_s>().pack(bref));
      break;
    case types::sn_ode_release_request:
      HANDLE_CODE(c.get<sn_ode_release_request_s>().pack(bref));
      break;
    case types::sn_ode_release_required:
      HANDLE_CODE(c.get<sn_ode_release_required_s>().pack(bref));
      break;
    case types::sn_ode_change_required:
      HANDLE_CODE(c.get<sn_ode_change_required_s>().pack(bref));
      break;
    case types::xn_removal_request:
      HANDLE_CODE(c.get<xn_removal_request_s>().pack(bref));
      break;
    case types::xn_setup_request:
      HANDLE_CODE(c.get<xn_setup_request_s>().pack(bref));
      break;
    case types::ngran_node_cfg_upd:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_s>().pack(bref));
      break;
    case types::e_utra_nr_cell_res_coordination_request:
      HANDLE_CODE(c.get<e_utra_nr_cell_res_coordination_request_s>().pack(bref));
      break;
    case types::cell_activation_request:
      HANDLE_CODE(c.get<cell_activation_request_s>().pack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().pack(bref));
      break;
    case types::res_status_request:
      HANDLE_CODE(c.get<res_status_request_s>().pack(bref));
      break;
    case types::mob_change_request:
      HANDLE_CODE(c.get<mob_change_request_s>().pack(bref));
      break;
    case types::iab_transport_migration_management_request:
      HANDLE_CODE(c.get<iab_transport_migration_management_request_s>().pack(bref));
      break;
    case types::iab_transport_migration_mod_request:
      HANDLE_CODE(c.get<iab_transport_migration_mod_request_s>().pack(bref));
      break;
    case types::iab_res_coordination_request:
      HANDLE_CODE(c.get<iab_res_coordination_request_s>().pack(bref));
      break;
    case types::partial_ue_context_transfer:
      HANDLE_CODE(c.get<partial_ue_context_transfer_s>().pack(bref));
      break;
    case types::sn_status_transfer:
      HANDLE_CODE(c.get<sn_status_transfer_s>().pack(bref));
      break;
    case types::ho_cancel:
      HANDLE_CODE(c.get<ho_cancel_s>().pack(bref));
      break;
    case types::ran_paging:
      HANDLE_CODE(c.get<ran_paging_s>().pack(bref));
      break;
    case types::xn_u_address_ind:
      HANDLE_CODE(c.get<xn_u_address_ind_s>().pack(bref));
      break;
    case types::ue_context_release:
      HANDLE_CODE(c.get<ue_context_release_s>().pack(bref));
      break;
    case types::sn_ode_recfg_complete:
      HANDLE_CODE(c.get<sn_ode_recfg_complete_s>().pack(bref));
      break;
    case types::sn_ode_counter_check_request:
      HANDLE_CODE(c.get<sn_ode_counter_check_request_s>().pack(bref));
      break;
    case types::rrc_transfer:
      HANDLE_CODE(c.get<rrc_transfer_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().pack(bref));
      break;
    case types::notif_ctrl_ind:
      HANDLE_CODE(c.get<notif_ctrl_ind_s>().pack(bref));
      break;
    case types::activity_notif:
      HANDLE_CODE(c.get<activity_notif_s>().pack(bref));
      break;
    case types::secondary_rat_data_usage_report:
      HANDLE_CODE(c.get<secondary_rat_data_usage_report_s>().pack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().pack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().pack(bref));
      break;
    case types::ho_success:
      HANDLE_CODE(c.get<ho_success_s>().pack(bref));
      break;
    case types::conditional_ho_cancel:
      HANDLE_CODE(c.get<conditional_ho_cancel_s>().pack(bref));
      break;
    case types::early_status_transfer:
      HANDLE_CODE(c.get<early_status_transfer_s>().pack(bref));
      break;
    case types::fail_ind:
      HANDLE_CODE(c.get<fail_ind_s>().pack(bref));
      break;
    case types::ho_report:
      HANDLE_CODE(c.get<ho_report_s>().pack(bref));
      break;
    case types::res_status_upd:
      HANDLE_CODE(c.get<res_status_upd_s>().pack(bref));
      break;
    case types::access_and_mob_ind:
      HANDLE_CODE(c.get<access_and_mob_ind_s>().pack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().pack(bref));
      break;
    case types::ran_multicast_group_paging:
      HANDLE_CODE(c.get<ran_multicast_group_paging_s>().pack(bref));
      break;
    case types::scg_fail_info_report:
      HANDLE_CODE(c.get<scg_fail_info_report_s>().pack(bref));
      break;
    case types::scg_fail_transfer:
      HANDLE_CODE(c.get<scg_fail_transfer_s>().pack(bref));
      break;
    case types::f1_c_traffic_transfer:
      HANDLE_CODE(c.get<f1_c_traffic_transfer_s>().pack(bref));
      break;
    case types::retrieve_ue_context_confirm:
      HANDLE_CODE(c.get<retrieve_ue_context_confirm_s>().pack(bref));
      break;
    case types::cp_c_cancel:
      HANDLE_CODE(c.get<cp_c_cancel_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_request:
      HANDLE_CODE(c.get<ho_request_s>().unpack(bref));
      break;
    case types::retrieve_ue_context_request:
      HANDLE_CODE(c.get<retrieve_ue_context_request_s>().unpack(bref));
      break;
    case types::sn_ode_addition_request:
      HANDLE_CODE(c.get<sn_ode_addition_request_s>().unpack(bref));
      break;
    case types::sn_ode_mod_request:
      HANDLE_CODE(c.get<sn_ode_mod_request_s>().unpack(bref));
      break;
    case types::sn_ode_mod_required:
      HANDLE_CODE(c.get<sn_ode_mod_required_s>().unpack(bref));
      break;
    case types::sn_ode_release_request:
      HANDLE_CODE(c.get<sn_ode_release_request_s>().unpack(bref));
      break;
    case types::sn_ode_release_required:
      HANDLE_CODE(c.get<sn_ode_release_required_s>().unpack(bref));
      break;
    case types::sn_ode_change_required:
      HANDLE_CODE(c.get<sn_ode_change_required_s>().unpack(bref));
      break;
    case types::xn_removal_request:
      HANDLE_CODE(c.get<xn_removal_request_s>().unpack(bref));
      break;
    case types::xn_setup_request:
      HANDLE_CODE(c.get<xn_setup_request_s>().unpack(bref));
      break;
    case types::ngran_node_cfg_upd:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_s>().unpack(bref));
      break;
    case types::e_utra_nr_cell_res_coordination_request:
      HANDLE_CODE(c.get<e_utra_nr_cell_res_coordination_request_s>().unpack(bref));
      break;
    case types::cell_activation_request:
      HANDLE_CODE(c.get<cell_activation_request_s>().unpack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().unpack(bref));
      break;
    case types::res_status_request:
      HANDLE_CODE(c.get<res_status_request_s>().unpack(bref));
      break;
    case types::mob_change_request:
      HANDLE_CODE(c.get<mob_change_request_s>().unpack(bref));
      break;
    case types::iab_transport_migration_management_request:
      HANDLE_CODE(c.get<iab_transport_migration_management_request_s>().unpack(bref));
      break;
    case types::iab_transport_migration_mod_request:
      HANDLE_CODE(c.get<iab_transport_migration_mod_request_s>().unpack(bref));
      break;
    case types::iab_res_coordination_request:
      HANDLE_CODE(c.get<iab_res_coordination_request_s>().unpack(bref));
      break;
    case types::partial_ue_context_transfer:
      HANDLE_CODE(c.get<partial_ue_context_transfer_s>().unpack(bref));
      break;
    case types::sn_status_transfer:
      HANDLE_CODE(c.get<sn_status_transfer_s>().unpack(bref));
      break;
    case types::ho_cancel:
      HANDLE_CODE(c.get<ho_cancel_s>().unpack(bref));
      break;
    case types::ran_paging:
      HANDLE_CODE(c.get<ran_paging_s>().unpack(bref));
      break;
    case types::xn_u_address_ind:
      HANDLE_CODE(c.get<xn_u_address_ind_s>().unpack(bref));
      break;
    case types::ue_context_release:
      HANDLE_CODE(c.get<ue_context_release_s>().unpack(bref));
      break;
    case types::sn_ode_recfg_complete:
      HANDLE_CODE(c.get<sn_ode_recfg_complete_s>().unpack(bref));
      break;
    case types::sn_ode_counter_check_request:
      HANDLE_CODE(c.get<sn_ode_counter_check_request_s>().unpack(bref));
      break;
    case types::rrc_transfer:
      HANDLE_CODE(c.get<rrc_transfer_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().unpack(bref));
      break;
    case types::notif_ctrl_ind:
      HANDLE_CODE(c.get<notif_ctrl_ind_s>().unpack(bref));
      break;
    case types::activity_notif:
      HANDLE_CODE(c.get<activity_notif_s>().unpack(bref));
      break;
    case types::secondary_rat_data_usage_report:
      HANDLE_CODE(c.get<secondary_rat_data_usage_report_s>().unpack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().unpack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().unpack(bref));
      break;
    case types::ho_success:
      HANDLE_CODE(c.get<ho_success_s>().unpack(bref));
      break;
    case types::conditional_ho_cancel:
      HANDLE_CODE(c.get<conditional_ho_cancel_s>().unpack(bref));
      break;
    case types::early_status_transfer:
      HANDLE_CODE(c.get<early_status_transfer_s>().unpack(bref));
      break;
    case types::fail_ind:
      HANDLE_CODE(c.get<fail_ind_s>().unpack(bref));
      break;
    case types::ho_report:
      HANDLE_CODE(c.get<ho_report_s>().unpack(bref));
      break;
    case types::res_status_upd:
      HANDLE_CODE(c.get<res_status_upd_s>().unpack(bref));
      break;
    case types::access_and_mob_ind:
      HANDLE_CODE(c.get<access_and_mob_ind_s>().unpack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().unpack(bref));
      break;
    case types::ran_multicast_group_paging:
      HANDLE_CODE(c.get<ran_multicast_group_paging_s>().unpack(bref));
      break;
    case types::scg_fail_info_report:
      HANDLE_CODE(c.get<scg_fail_info_report_s>().unpack(bref));
      break;
    case types::scg_fail_transfer:
      HANDLE_CODE(c.get<scg_fail_transfer_s>().unpack(bref));
      break;
    case types::f1_c_traffic_transfer:
      HANDLE_CODE(c.get<f1_c_traffic_transfer_s>().unpack(bref));
      break;
    case types::retrieve_ue_context_confirm:
      HANDLE_CODE(c.get<retrieve_ue_context_confirm_s>().unpack(bref));
      break;
    case types::cp_c_cancel:
      HANDLE_CODE(c.get<cp_c_cancel_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"HandoverRequest",
                                "RetrieveUEContextRequest",
                                "SNodeAdditionRequest",
                                "SNodeModificationRequest",
                                "SNodeModificationRequired",
                                "SNodeReleaseRequest",
                                "SNodeReleaseRequired",
                                "SNodeChangeRequired",
                                "XnRemovalRequest",
                                "XnSetupRequest",
                                "NGRANNodeConfigurationUpdate",
                                "E-UTRA-NR-CellResourceCoordinationRequest",
                                "CellActivationRequest",
                                "ResetRequest",
                                "ResourceStatusRequest",
                                "MobilityChangeRequest",
                                "IABTransportMigrationManagementRequest",
                                "IABTransportMigrationModificationRequest",
                                "IABResourceCoordinationRequest",
                                "PartialUEContextTransfer",
                                "SNStatusTransfer",
                                "HandoverCancel",
                                "RANPaging",
                                "XnUAddressIndication",
                                "UEContextRelease",
                                "SNodeReconfigurationComplete",
                                "SNodeCounterCheckRequest",
                                "RRCTransfer",
                                "ErrorIndication",
                                "PrivateMessage",
                                "NotificationControlIndication",
                                "ActivityNotification",
                                "SecondaryRATDataUsageReport",
                                "DeactivateTrace",
                                "TraceStart",
                                "HandoverSuccess",
                                "ConditionalHandoverCancel",
                                "EarlyStatusTransfer",
                                "FailureIndication",
                                "HandoverReport",
                                "ResourceStatusUpdate",
                                "AccessAndMobilityIndication",
                                "CellTrafficTrace",
                                "RANMulticastGroupPaging",
                                "ScgFailureInformationReport",
                                "ScgFailureTransfer",
                                "F1CTrafficTransfer",
                                "RetrieveUEContextConfirm",
                                "CPCCancel"};
  return convert_enum_idx(names, 49, value, "xnap_elem_procs_o::init_msg_c::types");
}
uint8_t xnap_elem_procs_o::init_msg_c::types_opts::to_number() const
{
  if (value == f1_c_traffic_transfer) {
    return 1;
  }
  invalid_enum_number(value, "xnap_elem_procs_o::init_msg_c::types");
  return 0;
}

// SuccessfulOutcome ::= OPEN TYPE
void xnap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ho_request_ack:
      c = ho_request_ack_s{};
      break;
    case types::retrieve_ue_context_resp:
      c = retrieve_ue_context_resp_s{};
      break;
    case types::sn_ode_addition_request_ack:
      c = sn_ode_addition_request_ack_s{};
      break;
    case types::sn_ode_mod_request_ack:
      c = sn_ode_mod_request_ack_s{};
      break;
    case types::sn_ode_mod_confirm:
      c = sn_ode_mod_confirm_s{};
      break;
    case types::sn_ode_release_request_ack:
      c = sn_ode_release_request_ack_s{};
      break;
    case types::sn_ode_release_confirm:
      c = sn_ode_release_confirm_s{};
      break;
    case types::sn_ode_change_confirm:
      c = sn_ode_change_confirm_s{};
      break;
    case types::xn_removal_resp:
      c = xn_removal_resp_s{};
      break;
    case types::xn_setup_resp:
      c = xn_setup_resp_s{};
      break;
    case types::ngran_node_cfg_upd_ack:
      c = ngran_node_cfg_upd_ack_s{};
      break;
    case types::e_utra_nr_cell_res_coordination_resp:
      c = e_utra_nr_cell_res_coordination_resp_s{};
      break;
    case types::cell_activation_resp:
      c = cell_activation_resp_s{};
      break;
    case types::reset_resp:
      c = reset_resp_s{};
      break;
    case types::res_status_resp:
      c = res_status_resp_s{};
      break;
    case types::mob_change_ack:
      c = mob_change_ack_s{};
      break;
    case types::iab_transport_migration_management_resp:
      c = iab_transport_migration_management_resp_s{};
      break;
    case types::iab_transport_migration_mod_resp:
      c = iab_transport_migration_mod_resp_s{};
      break;
    case types::iab_res_coordination_resp:
      c = iab_res_coordination_resp_s{};
      break;
    case types::partial_ue_context_transfer_ack:
      c = partial_ue_context_transfer_ack_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::successful_outcome_c");
  }
}
ho_request_ack_s& xnap_elem_procs_o::successful_outcome_c::ho_request_ack()
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
retrieve_ue_context_resp_s& xnap_elem_procs_o::successful_outcome_c::retrieve_ue_context_resp()
{
  assert_choice_type(types::retrieve_ue_context_resp, type_, "SuccessfulOutcome");
  return c.get<retrieve_ue_context_resp_s>();
}
sn_ode_addition_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_addition_request_ack()
{
  assert_choice_type(types::sn_ode_addition_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_addition_request_ack_s>();
}
sn_ode_mod_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_mod_request_ack()
{
  assert_choice_type(types::sn_ode_mod_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_request_ack_s>();
}
sn_ode_mod_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_mod_confirm()
{
  assert_choice_type(types::sn_ode_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_confirm_s>();
}
sn_ode_release_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_release_request_ack()
{
  assert_choice_type(types::sn_ode_release_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_request_ack_s>();
}
sn_ode_release_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_release_confirm()
{
  assert_choice_type(types::sn_ode_release_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_confirm_s>();
}
sn_ode_change_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_change_confirm()
{
  assert_choice_type(types::sn_ode_change_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_change_confirm_s>();
}
xn_removal_resp_s& xnap_elem_procs_o::successful_outcome_c::xn_removal_resp()
{
  assert_choice_type(types::xn_removal_resp, type_, "SuccessfulOutcome");
  return c.get<xn_removal_resp_s>();
}
xn_setup_resp_s& xnap_elem_procs_o::successful_outcome_c::xn_setup_resp()
{
  assert_choice_type(types::xn_setup_resp, type_, "SuccessfulOutcome");
  return c.get<xn_setup_resp_s>();
}
ngran_node_cfg_upd_ack_s& xnap_elem_procs_o::successful_outcome_c::ngran_node_cfg_upd_ack()
{
  assert_choice_type(types::ngran_node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_ack_s>();
}
e_utra_nr_cell_res_coordination_resp_s& xnap_elem_procs_o::successful_outcome_c::e_utra_nr_cell_res_coordination_resp()
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<e_utra_nr_cell_res_coordination_resp_s>();
}
cell_activation_resp_s& xnap_elem_procs_o::successful_outcome_c::cell_activation_resp()
{
  assert_choice_type(types::cell_activation_resp, type_, "SuccessfulOutcome");
  return c.get<cell_activation_resp_s>();
}
reset_resp_s& xnap_elem_procs_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
res_status_resp_s& xnap_elem_procs_o::successful_outcome_c::res_status_resp()
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
mob_change_ack_s& xnap_elem_procs_o::successful_outcome_c::mob_change_ack()
{
  assert_choice_type(types::mob_change_ack, type_, "SuccessfulOutcome");
  return c.get<mob_change_ack_s>();
}
iab_transport_migration_management_resp_s&
xnap_elem_procs_o::successful_outcome_c::iab_transport_migration_management_resp()
{
  assert_choice_type(types::iab_transport_migration_management_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_management_resp_s>();
}
iab_transport_migration_mod_resp_s& xnap_elem_procs_o::successful_outcome_c::iab_transport_migration_mod_resp()
{
  assert_choice_type(types::iab_transport_migration_mod_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_mod_resp_s>();
}
iab_res_coordination_resp_s& xnap_elem_procs_o::successful_outcome_c::iab_res_coordination_resp()
{
  assert_choice_type(types::iab_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<iab_res_coordination_resp_s>();
}
partial_ue_context_transfer_ack_s& xnap_elem_procs_o::successful_outcome_c::partial_ue_context_transfer_ack()
{
  assert_choice_type(types::partial_ue_context_transfer_ack, type_, "SuccessfulOutcome");
  return c.get<partial_ue_context_transfer_ack_s>();
}
const ho_request_ack_s& xnap_elem_procs_o::successful_outcome_c::ho_request_ack() const
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
const retrieve_ue_context_resp_s& xnap_elem_procs_o::successful_outcome_c::retrieve_ue_context_resp() const
{
  assert_choice_type(types::retrieve_ue_context_resp, type_, "SuccessfulOutcome");
  return c.get<retrieve_ue_context_resp_s>();
}
const sn_ode_addition_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_addition_request_ack() const
{
  assert_choice_type(types::sn_ode_addition_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_addition_request_ack_s>();
}
const sn_ode_mod_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_mod_request_ack() const
{
  assert_choice_type(types::sn_ode_mod_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_request_ack_s>();
}
const sn_ode_mod_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_mod_confirm() const
{
  assert_choice_type(types::sn_ode_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_confirm_s>();
}
const sn_ode_release_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_release_request_ack() const
{
  assert_choice_type(types::sn_ode_release_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_request_ack_s>();
}
const sn_ode_release_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_release_confirm() const
{
  assert_choice_type(types::sn_ode_release_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_confirm_s>();
}
const sn_ode_change_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_change_confirm() const
{
  assert_choice_type(types::sn_ode_change_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_change_confirm_s>();
}
const xn_removal_resp_s& xnap_elem_procs_o::successful_outcome_c::xn_removal_resp() const
{
  assert_choice_type(types::xn_removal_resp, type_, "SuccessfulOutcome");
  return c.get<xn_removal_resp_s>();
}
const xn_setup_resp_s& xnap_elem_procs_o::successful_outcome_c::xn_setup_resp() const
{
  assert_choice_type(types::xn_setup_resp, type_, "SuccessfulOutcome");
  return c.get<xn_setup_resp_s>();
}
const ngran_node_cfg_upd_ack_s& xnap_elem_procs_o::successful_outcome_c::ngran_node_cfg_upd_ack() const
{
  assert_choice_type(types::ngran_node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_ack_s>();
}
const e_utra_nr_cell_res_coordination_resp_s&
xnap_elem_procs_o::successful_outcome_c::e_utra_nr_cell_res_coordination_resp() const
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<e_utra_nr_cell_res_coordination_resp_s>();
}
const cell_activation_resp_s& xnap_elem_procs_o::successful_outcome_c::cell_activation_resp() const
{
  assert_choice_type(types::cell_activation_resp, type_, "SuccessfulOutcome");
  return c.get<cell_activation_resp_s>();
}
const reset_resp_s& xnap_elem_procs_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const res_status_resp_s& xnap_elem_procs_o::successful_outcome_c::res_status_resp() const
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
const mob_change_ack_s& xnap_elem_procs_o::successful_outcome_c::mob_change_ack() const
{
  assert_choice_type(types::mob_change_ack, type_, "SuccessfulOutcome");
  return c.get<mob_change_ack_s>();
}
const iab_transport_migration_management_resp_s&
xnap_elem_procs_o::successful_outcome_c::iab_transport_migration_management_resp() const
{
  assert_choice_type(types::iab_transport_migration_management_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_management_resp_s>();
}
const iab_transport_migration_mod_resp_s&
xnap_elem_procs_o::successful_outcome_c::iab_transport_migration_mod_resp() const
{
  assert_choice_type(types::iab_transport_migration_mod_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_mod_resp_s>();
}
const iab_res_coordination_resp_s& xnap_elem_procs_o::successful_outcome_c::iab_res_coordination_resp() const
{
  assert_choice_type(types::iab_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<iab_res_coordination_resp_s>();
}
const partial_ue_context_transfer_ack_s&
xnap_elem_procs_o::successful_outcome_c::partial_ue_context_transfer_ack() const
{
  assert_choice_type(types::partial_ue_context_transfer_ack, type_, "SuccessfulOutcome");
  return c.get<partial_ue_context_transfer_ack_s>();
}
void xnap_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_request_ack:
      j.write_fieldname("HandoverRequestAcknowledge");
      c.get<ho_request_ack_s>().to_json(j);
      break;
    case types::retrieve_ue_context_resp:
      j.write_fieldname("RetrieveUEContextResponse");
      c.get<retrieve_ue_context_resp_s>().to_json(j);
      break;
    case types::sn_ode_addition_request_ack:
      j.write_fieldname("SNodeAdditionRequestAcknowledge");
      c.get<sn_ode_addition_request_ack_s>().to_json(j);
      break;
    case types::sn_ode_mod_request_ack:
      j.write_fieldname("SNodeModificationRequestAcknowledge");
      c.get<sn_ode_mod_request_ack_s>().to_json(j);
      break;
    case types::sn_ode_mod_confirm:
      j.write_fieldname("SNodeModificationConfirm");
      c.get<sn_ode_mod_confirm_s>().to_json(j);
      break;
    case types::sn_ode_release_request_ack:
      j.write_fieldname("SNodeReleaseRequestAcknowledge");
      c.get<sn_ode_release_request_ack_s>().to_json(j);
      break;
    case types::sn_ode_release_confirm:
      j.write_fieldname("SNodeReleaseConfirm");
      c.get<sn_ode_release_confirm_s>().to_json(j);
      break;
    case types::sn_ode_change_confirm:
      j.write_fieldname("SNodeChangeConfirm");
      c.get<sn_ode_change_confirm_s>().to_json(j);
      break;
    case types::xn_removal_resp:
      j.write_fieldname("XnRemovalResponse");
      c.get<xn_removal_resp_s>().to_json(j);
      break;
    case types::xn_setup_resp:
      j.write_fieldname("XnSetupResponse");
      c.get<xn_setup_resp_s>().to_json(j);
      break;
    case types::ngran_node_cfg_upd_ack:
      j.write_fieldname("NGRANNodeConfigurationUpdateAcknowledge");
      c.get<ngran_node_cfg_upd_ack_s>().to_json(j);
      break;
    case types::e_utra_nr_cell_res_coordination_resp:
      j.write_fieldname("E-UTRA-NR-CellResourceCoordinationResponse");
      c.get<e_utra_nr_cell_res_coordination_resp_s>().to_json(j);
      break;
    case types::cell_activation_resp:
      j.write_fieldname("CellActivationResponse");
      c.get<cell_activation_resp_s>().to_json(j);
      break;
    case types::reset_resp:
      j.write_fieldname("ResetResponse");
      c.get<reset_resp_s>().to_json(j);
      break;
    case types::res_status_resp:
      j.write_fieldname("ResourceStatusResponse");
      c.get<res_status_resp_s>().to_json(j);
      break;
    case types::mob_change_ack:
      j.write_fieldname("MobilityChangeAcknowledge");
      c.get<mob_change_ack_s>().to_json(j);
      break;
    case types::iab_transport_migration_management_resp:
      j.write_fieldname("IABTransportMigrationManagementResponse");
      c.get<iab_transport_migration_management_resp_s>().to_json(j);
      break;
    case types::iab_transport_migration_mod_resp:
      j.write_fieldname("IABTransportMigrationModificationResponse");
      c.get<iab_transport_migration_mod_resp_s>().to_json(j);
      break;
    case types::iab_res_coordination_resp:
      j.write_fieldname("IABResourceCoordinationResponse");
      c.get<iab_res_coordination_resp_s>().to_json(j);
      break;
    case types::partial_ue_context_transfer_ack:
      j.write_fieldname("PartialUEContextTransferAcknowledge");
      c.get<partial_ue_context_transfer_ack_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_request_ack:
      HANDLE_CODE(c.get<ho_request_ack_s>().pack(bref));
      break;
    case types::retrieve_ue_context_resp:
      HANDLE_CODE(c.get<retrieve_ue_context_resp_s>().pack(bref));
      break;
    case types::sn_ode_addition_request_ack:
      HANDLE_CODE(c.get<sn_ode_addition_request_ack_s>().pack(bref));
      break;
    case types::sn_ode_mod_request_ack:
      HANDLE_CODE(c.get<sn_ode_mod_request_ack_s>().pack(bref));
      break;
    case types::sn_ode_mod_confirm:
      HANDLE_CODE(c.get<sn_ode_mod_confirm_s>().pack(bref));
      break;
    case types::sn_ode_release_request_ack:
      HANDLE_CODE(c.get<sn_ode_release_request_ack_s>().pack(bref));
      break;
    case types::sn_ode_release_confirm:
      HANDLE_CODE(c.get<sn_ode_release_confirm_s>().pack(bref));
      break;
    case types::sn_ode_change_confirm:
      HANDLE_CODE(c.get<sn_ode_change_confirm_s>().pack(bref));
      break;
    case types::xn_removal_resp:
      HANDLE_CODE(c.get<xn_removal_resp_s>().pack(bref));
      break;
    case types::xn_setup_resp:
      HANDLE_CODE(c.get<xn_setup_resp_s>().pack(bref));
      break;
    case types::ngran_node_cfg_upd_ack:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_ack_s>().pack(bref));
      break;
    case types::e_utra_nr_cell_res_coordination_resp:
      HANDLE_CODE(c.get<e_utra_nr_cell_res_coordination_resp_s>().pack(bref));
      break;
    case types::cell_activation_resp:
      HANDLE_CODE(c.get<cell_activation_resp_s>().pack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().pack(bref));
      break;
    case types::res_status_resp:
      HANDLE_CODE(c.get<res_status_resp_s>().pack(bref));
      break;
    case types::mob_change_ack:
      HANDLE_CODE(c.get<mob_change_ack_s>().pack(bref));
      break;
    case types::iab_transport_migration_management_resp:
      HANDLE_CODE(c.get<iab_transport_migration_management_resp_s>().pack(bref));
      break;
    case types::iab_transport_migration_mod_resp:
      HANDLE_CODE(c.get<iab_transport_migration_mod_resp_s>().pack(bref));
      break;
    case types::iab_res_coordination_resp:
      HANDLE_CODE(c.get<iab_res_coordination_resp_s>().pack(bref));
      break;
    case types::partial_ue_context_transfer_ack:
      HANDLE_CODE(c.get<partial_ue_context_transfer_ack_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_request_ack:
      HANDLE_CODE(c.get<ho_request_ack_s>().unpack(bref));
      break;
    case types::retrieve_ue_context_resp:
      HANDLE_CODE(c.get<retrieve_ue_context_resp_s>().unpack(bref));
      break;
    case types::sn_ode_addition_request_ack:
      HANDLE_CODE(c.get<sn_ode_addition_request_ack_s>().unpack(bref));
      break;
    case types::sn_ode_mod_request_ack:
      HANDLE_CODE(c.get<sn_ode_mod_request_ack_s>().unpack(bref));
      break;
    case types::sn_ode_mod_confirm:
      HANDLE_CODE(c.get<sn_ode_mod_confirm_s>().unpack(bref));
      break;
    case types::sn_ode_release_request_ack:
      HANDLE_CODE(c.get<sn_ode_release_request_ack_s>().unpack(bref));
      break;
    case types::sn_ode_release_confirm:
      HANDLE_CODE(c.get<sn_ode_release_confirm_s>().unpack(bref));
      break;
    case types::sn_ode_change_confirm:
      HANDLE_CODE(c.get<sn_ode_change_confirm_s>().unpack(bref));
      break;
    case types::xn_removal_resp:
      HANDLE_CODE(c.get<xn_removal_resp_s>().unpack(bref));
      break;
    case types::xn_setup_resp:
      HANDLE_CODE(c.get<xn_setup_resp_s>().unpack(bref));
      break;
    case types::ngran_node_cfg_upd_ack:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::e_utra_nr_cell_res_coordination_resp:
      HANDLE_CODE(c.get<e_utra_nr_cell_res_coordination_resp_s>().unpack(bref));
      break;
    case types::cell_activation_resp:
      HANDLE_CODE(c.get<cell_activation_resp_s>().unpack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().unpack(bref));
      break;
    case types::res_status_resp:
      HANDLE_CODE(c.get<res_status_resp_s>().unpack(bref));
      break;
    case types::mob_change_ack:
      HANDLE_CODE(c.get<mob_change_ack_s>().unpack(bref));
      break;
    case types::iab_transport_migration_management_resp:
      HANDLE_CODE(c.get<iab_transport_migration_management_resp_s>().unpack(bref));
      break;
    case types::iab_transport_migration_mod_resp:
      HANDLE_CODE(c.get<iab_transport_migration_mod_resp_s>().unpack(bref));
      break;
    case types::iab_res_coordination_resp:
      HANDLE_CODE(c.get<iab_res_coordination_resp_s>().unpack(bref));
      break;
    case types::partial_ue_context_transfer_ack:
      HANDLE_CODE(c.get<partial_ue_context_transfer_ack_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"HandoverRequestAcknowledge",
                                "RetrieveUEContextResponse",
                                "SNodeAdditionRequestAcknowledge",
                                "SNodeModificationRequestAcknowledge",
                                "SNodeModificationConfirm",
                                "SNodeReleaseRequestAcknowledge",
                                "SNodeReleaseConfirm",
                                "SNodeChangeConfirm",
                                "XnRemovalResponse",
                                "XnSetupResponse",
                                "NGRANNodeConfigurationUpdateAcknowledge",
                                "E-UTRA-NR-CellResourceCoordinationResponse",
                                "CellActivationResponse",
                                "ResetResponse",
                                "ResourceStatusResponse",
                                "MobilityChangeAcknowledge",
                                "IABTransportMigrationManagementResponse",
                                "IABTransportMigrationModificationResponse",
                                "IABResourceCoordinationResponse",
                                "PartialUEContextTransferAcknowledge"};
  return convert_enum_idx(names, 20, value, "xnap_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void xnap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ho_prep_fail:
      c = ho_prep_fail_s{};
      break;
    case types::retrieve_ue_context_fail:
      c = retrieve_ue_context_fail_s{};
      break;
    case types::sn_ode_addition_request_reject:
      c = sn_ode_addition_request_reject_s{};
      break;
    case types::sn_ode_mod_request_reject:
      c = sn_ode_mod_request_reject_s{};
      break;
    case types::sn_ode_mod_refuse:
      c = sn_ode_mod_refuse_s{};
      break;
    case types::sn_ode_release_reject:
      c = sn_ode_release_reject_s{};
      break;
    case types::sn_ode_change_refuse:
      c = sn_ode_change_refuse_s{};
      break;
    case types::xn_removal_fail:
      c = xn_removal_fail_s{};
      break;
    case types::xn_setup_fail:
      c = xn_setup_fail_s{};
      break;
    case types::ngran_node_cfg_upd_fail:
      c = ngran_node_cfg_upd_fail_s{};
      break;
    case types::cell_activation_fail:
      c = cell_activation_fail_s{};
      break;
    case types::res_status_fail:
      c = res_status_fail_s{};
      break;
    case types::mob_change_fail:
      c = mob_change_fail_s{};
      break;
    case types::iab_transport_migration_management_reject:
      c = iab_transport_migration_management_reject_s{};
      break;
    case types::partial_ue_context_transfer_fail:
      c = partial_ue_context_transfer_fail_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::unsuccessful_outcome_c");
  }
}
ho_prep_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::ho_prep_fail()
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
retrieve_ue_context_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::retrieve_ue_context_fail()
{
  assert_choice_type(types::retrieve_ue_context_fail, type_, "UnsuccessfulOutcome");
  return c.get<retrieve_ue_context_fail_s>();
}
sn_ode_addition_request_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_addition_request_reject()
{
  assert_choice_type(types::sn_ode_addition_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_addition_request_reject_s>();
}
sn_ode_mod_request_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_mod_request_reject()
{
  assert_choice_type(types::sn_ode_mod_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_request_reject_s>();
}
sn_ode_mod_refuse_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_mod_refuse()
{
  assert_choice_type(types::sn_ode_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_refuse_s>();
}
sn_ode_release_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_release_reject()
{
  assert_choice_type(types::sn_ode_release_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_release_reject_s>();
}
sn_ode_change_refuse_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_change_refuse()
{
  assert_choice_type(types::sn_ode_change_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_change_refuse_s>();
}
xn_removal_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::xn_removal_fail()
{
  assert_choice_type(types::xn_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_removal_fail_s>();
}
xn_setup_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::xn_setup_fail()
{
  assert_choice_type(types::xn_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_setup_fail_s>();
}
ngran_node_cfg_upd_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::ngran_node_cfg_upd_fail()
{
  assert_choice_type(types::ngran_node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_fail_s>();
}
cell_activation_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::cell_activation_fail()
{
  assert_choice_type(types::cell_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<cell_activation_fail_s>();
}
res_status_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::res_status_fail()
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
mob_change_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::mob_change_fail()
{
  assert_choice_type(types::mob_change_fail, type_, "UnsuccessfulOutcome");
  return c.get<mob_change_fail_s>();
}
iab_transport_migration_management_reject_s&
xnap_elem_procs_o::unsuccessful_outcome_c::iab_transport_migration_management_reject()
{
  assert_choice_type(types::iab_transport_migration_management_reject, type_, "UnsuccessfulOutcome");
  return c.get<iab_transport_migration_management_reject_s>();
}
partial_ue_context_transfer_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::partial_ue_context_transfer_fail()
{
  assert_choice_type(types::partial_ue_context_transfer_fail, type_, "UnsuccessfulOutcome");
  return c.get<partial_ue_context_transfer_fail_s>();
}
const ho_prep_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::ho_prep_fail() const
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
const retrieve_ue_context_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::retrieve_ue_context_fail() const
{
  assert_choice_type(types::retrieve_ue_context_fail, type_, "UnsuccessfulOutcome");
  return c.get<retrieve_ue_context_fail_s>();
}
const sn_ode_addition_request_reject_s&
xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_addition_request_reject() const
{
  assert_choice_type(types::sn_ode_addition_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_addition_request_reject_s>();
}
const sn_ode_mod_request_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_mod_request_reject() const
{
  assert_choice_type(types::sn_ode_mod_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_request_reject_s>();
}
const sn_ode_mod_refuse_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_mod_refuse() const
{
  assert_choice_type(types::sn_ode_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_refuse_s>();
}
const sn_ode_release_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_release_reject() const
{
  assert_choice_type(types::sn_ode_release_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_release_reject_s>();
}
const sn_ode_change_refuse_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_change_refuse() const
{
  assert_choice_type(types::sn_ode_change_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_change_refuse_s>();
}
const xn_removal_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::xn_removal_fail() const
{
  assert_choice_type(types::xn_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_removal_fail_s>();
}
const xn_setup_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::xn_setup_fail() const
{
  assert_choice_type(types::xn_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_setup_fail_s>();
}
const ngran_node_cfg_upd_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::ngran_node_cfg_upd_fail() const
{
  assert_choice_type(types::ngran_node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_fail_s>();
}
const cell_activation_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::cell_activation_fail() const
{
  assert_choice_type(types::cell_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<cell_activation_fail_s>();
}
const res_status_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::res_status_fail() const
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
const mob_change_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::mob_change_fail() const
{
  assert_choice_type(types::mob_change_fail, type_, "UnsuccessfulOutcome");
  return c.get<mob_change_fail_s>();
}
const iab_transport_migration_management_reject_s&
xnap_elem_procs_o::unsuccessful_outcome_c::iab_transport_migration_management_reject() const
{
  assert_choice_type(types::iab_transport_migration_management_reject, type_, "UnsuccessfulOutcome");
  return c.get<iab_transport_migration_management_reject_s>();
}
const partial_ue_context_transfer_fail_s&
xnap_elem_procs_o::unsuccessful_outcome_c::partial_ue_context_transfer_fail() const
{
  assert_choice_type(types::partial_ue_context_transfer_fail, type_, "UnsuccessfulOutcome");
  return c.get<partial_ue_context_transfer_fail_s>();
}
void xnap_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_prep_fail:
      j.write_fieldname("HandoverPreparationFailure");
      c.get<ho_prep_fail_s>().to_json(j);
      break;
    case types::retrieve_ue_context_fail:
      j.write_fieldname("RetrieveUEContextFailure");
      c.get<retrieve_ue_context_fail_s>().to_json(j);
      break;
    case types::sn_ode_addition_request_reject:
      j.write_fieldname("SNodeAdditionRequestReject");
      c.get<sn_ode_addition_request_reject_s>().to_json(j);
      break;
    case types::sn_ode_mod_request_reject:
      j.write_fieldname("SNodeModificationRequestReject");
      c.get<sn_ode_mod_request_reject_s>().to_json(j);
      break;
    case types::sn_ode_mod_refuse:
      j.write_fieldname("SNodeModificationRefuse");
      c.get<sn_ode_mod_refuse_s>().to_json(j);
      break;
    case types::sn_ode_release_reject:
      j.write_fieldname("SNodeReleaseReject");
      c.get<sn_ode_release_reject_s>().to_json(j);
      break;
    case types::sn_ode_change_refuse:
      j.write_fieldname("SNodeChangeRefuse");
      c.get<sn_ode_change_refuse_s>().to_json(j);
      break;
    case types::xn_removal_fail:
      j.write_fieldname("XnRemovalFailure");
      c.get<xn_removal_fail_s>().to_json(j);
      break;
    case types::xn_setup_fail:
      j.write_fieldname("XnSetupFailure");
      c.get<xn_setup_fail_s>().to_json(j);
      break;
    case types::ngran_node_cfg_upd_fail:
      j.write_fieldname("NGRANNodeConfigurationUpdateFailure");
      c.get<ngran_node_cfg_upd_fail_s>().to_json(j);
      break;
    case types::cell_activation_fail:
      j.write_fieldname("CellActivationFailure");
      c.get<cell_activation_fail_s>().to_json(j);
      break;
    case types::res_status_fail:
      j.write_fieldname("ResourceStatusFailure");
      c.get<res_status_fail_s>().to_json(j);
      break;
    case types::mob_change_fail:
      j.write_fieldname("MobilityChangeFailure");
      c.get<mob_change_fail_s>().to_json(j);
      break;
    case types::iab_transport_migration_management_reject:
      j.write_fieldname("IABTransportMigrationManagementReject");
      c.get<iab_transport_migration_management_reject_s>().to_json(j);
      break;
    case types::partial_ue_context_transfer_fail:
      j.write_fieldname("PartialUEContextTransferFailure");
      c.get<partial_ue_context_transfer_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE xnap_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_prep_fail:
      HANDLE_CODE(c.get<ho_prep_fail_s>().pack(bref));
      break;
    case types::retrieve_ue_context_fail:
      HANDLE_CODE(c.get<retrieve_ue_context_fail_s>().pack(bref));
      break;
    case types::sn_ode_addition_request_reject:
      HANDLE_CODE(c.get<sn_ode_addition_request_reject_s>().pack(bref));
      break;
    case types::sn_ode_mod_request_reject:
      HANDLE_CODE(c.get<sn_ode_mod_request_reject_s>().pack(bref));
      break;
    case types::sn_ode_mod_refuse:
      HANDLE_CODE(c.get<sn_ode_mod_refuse_s>().pack(bref));
      break;
    case types::sn_ode_release_reject:
      HANDLE_CODE(c.get<sn_ode_release_reject_s>().pack(bref));
      break;
    case types::sn_ode_change_refuse:
      HANDLE_CODE(c.get<sn_ode_change_refuse_s>().pack(bref));
      break;
    case types::xn_removal_fail:
      HANDLE_CODE(c.get<xn_removal_fail_s>().pack(bref));
      break;
    case types::xn_setup_fail:
      HANDLE_CODE(c.get<xn_setup_fail_s>().pack(bref));
      break;
    case types::ngran_node_cfg_upd_fail:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_fail_s>().pack(bref));
      break;
    case types::cell_activation_fail:
      HANDLE_CODE(c.get<cell_activation_fail_s>().pack(bref));
      break;
    case types::res_status_fail:
      HANDLE_CODE(c.get<res_status_fail_s>().pack(bref));
      break;
    case types::mob_change_fail:
      HANDLE_CODE(c.get<mob_change_fail_s>().pack(bref));
      break;
    case types::iab_transport_migration_management_reject:
      HANDLE_CODE(c.get<iab_transport_migration_management_reject_s>().pack(bref));
      break;
    case types::partial_ue_context_transfer_fail:
      HANDLE_CODE(c.get<partial_ue_context_transfer_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE xnap_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ho_prep_fail:
      HANDLE_CODE(c.get<ho_prep_fail_s>().unpack(bref));
      break;
    case types::retrieve_ue_context_fail:
      HANDLE_CODE(c.get<retrieve_ue_context_fail_s>().unpack(bref));
      break;
    case types::sn_ode_addition_request_reject:
      HANDLE_CODE(c.get<sn_ode_addition_request_reject_s>().unpack(bref));
      break;
    case types::sn_ode_mod_request_reject:
      HANDLE_CODE(c.get<sn_ode_mod_request_reject_s>().unpack(bref));
      break;
    case types::sn_ode_mod_refuse:
      HANDLE_CODE(c.get<sn_ode_mod_refuse_s>().unpack(bref));
      break;
    case types::sn_ode_release_reject:
      HANDLE_CODE(c.get<sn_ode_release_reject_s>().unpack(bref));
      break;
    case types::sn_ode_change_refuse:
      HANDLE_CODE(c.get<sn_ode_change_refuse_s>().unpack(bref));
      break;
    case types::xn_removal_fail:
      HANDLE_CODE(c.get<xn_removal_fail_s>().unpack(bref));
      break;
    case types::xn_setup_fail:
      HANDLE_CODE(c.get<xn_setup_fail_s>().unpack(bref));
      break;
    case types::ngran_node_cfg_upd_fail:
      HANDLE_CODE(c.get<ngran_node_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::cell_activation_fail:
      HANDLE_CODE(c.get<cell_activation_fail_s>().unpack(bref));
      break;
    case types::res_status_fail:
      HANDLE_CODE(c.get<res_status_fail_s>().unpack(bref));
      break;
    case types::mob_change_fail:
      HANDLE_CODE(c.get<mob_change_fail_s>().unpack(bref));
      break;
    case types::iab_transport_migration_management_reject:
      HANDLE_CODE(c.get<iab_transport_migration_management_reject_s>().unpack(bref));
      break;
    case types::partial_ue_context_transfer_fail:
      HANDLE_CODE(c.get<partial_ue_context_transfer_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "xnap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* xnap_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"HandoverPreparationFailure",
                                "RetrieveUEContextFailure",
                                "SNodeAdditionRequestReject",
                                "SNodeModificationRequestReject",
                                "SNodeModificationRefuse",
                                "SNodeReleaseReject",
                                "SNodeChangeRefuse",
                                "XnRemovalFailure",
                                "XnSetupFailure",
                                "NGRANNodeConfigurationUpdateFailure",
                                "CellActivationFailure",
                                "ResourceStatusFailure",
                                "MobilityChangeFailure",
                                "IABTransportMigrationManagementReject",
                                "PartialUEContextTransferFailure"};
  return convert_enum_idx(names, 15, value, "xnap_elem_procs_o::unsuccessful_outcome_c::types");
}

// InitiatingMessage ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != xnap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = xnap_elem_procs_o::get_init_msg(proc_code);
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
  if (not xnap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = xnap_elem_procs_o::get_crit(proc_code);
  value     = xnap_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != xnap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != xnap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = xnap_elem_procs_o::get_successful_outcome(proc_code);
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
  if (not xnap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = xnap_elem_procs_o::get_crit(proc_code);
  value     = xnap_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != xnap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != xnap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = xnap_elem_procs_o::get_unsuccessful_outcome(proc_code);
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
  if (not xnap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = xnap_elem_procs_o::get_crit(proc_code);
  value     = xnap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != xnap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// XnAP-PDU ::= CHOICE
void xn_ap_pdu_c::destroy_()
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
void xn_ap_pdu_c::set(types::options e)
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
      log_invalid_choice_id(type_, "xn_ap_pdu_c");
  }
}
xn_ap_pdu_c::xn_ap_pdu_c(const xn_ap_pdu_c& other)
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
      log_invalid_choice_id(type_, "xn_ap_pdu_c");
  }
}
xn_ap_pdu_c& xn_ap_pdu_c::operator=(const xn_ap_pdu_c& other)
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
      log_invalid_choice_id(type_, "xn_ap_pdu_c");
  }

  return *this;
}
init_msg_s& xn_ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& xn_ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& xn_ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
void xn_ap_pdu_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "xn_ap_pdu_c");
  }
  j.end_obj();
}
SRSASN_CODE xn_ap_pdu_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "xn_ap_pdu_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE xn_ap_pdu_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "xn_ap_pdu_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* xn_ap_pdu_c::types_opts::to_string() const
{
  static const char* names[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome"};
  return convert_enum_idx(names, 3, value, "xn_ap_pdu_c::types");
}
