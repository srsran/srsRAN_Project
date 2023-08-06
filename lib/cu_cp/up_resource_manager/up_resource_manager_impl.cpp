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

#include "up_resource_manager_impl.h"
#include "up_resource_manager_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

up_resource_manager_impl::up_resource_manager_impl(const up_resource_manager_cfg& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

bool up_resource_manager_impl::validate_request(
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items)
{
  return is_valid(setup_items, context, cfg, logger);
}

bool up_resource_manager_impl::validate_request(const cu_cp_pdu_session_resource_modify_request& pdu)
{
  return is_valid(pdu, context, cfg, logger);
}

bool up_resource_manager_impl::validate_request(const cu_cp_pdu_session_resource_release_command& pdu)
{
  return is_valid(pdu, context, cfg, logger);
}

up_config_update up_resource_manager_impl::calculate_update(
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items)
{
  srsran_assert(is_valid(setup_items, context, cfg, logger), "Invalid PDU Session Resource Setup items.");
  return srsran::srs_cu_cp::calculate_update(setup_items, context, cfg, logger);
}

up_config_update up_resource_manager_impl::calculate_update(const cu_cp_pdu_session_resource_modify_request& pdu)
{
  srsran_assert(is_valid(pdu, context, cfg, logger), "Invalid PDU Session Resource Modify request.");
  return srsran::srs_cu_cp::calculate_update(pdu, context, cfg, logger);
}

up_config_update up_resource_manager_impl::calculate_update(const cu_cp_pdu_session_resource_release_command& pdu)
{
  srsran_assert(is_valid(pdu, context, cfg, logger), "Invalid PDU Session Resource Release command.");
  return srsran::srs_cu_cp::calculate_update(pdu, context, cfg, logger);
}

inline void apply_update_for_new_drbs(up_pdu_session_context&                   pdu_session_context,
                                      up_context&                               context,
                                      const std::map<drb_id_t, up_drb_context>& drb_to_add)
{
  for (const auto& drb : drb_to_add) {
    pdu_session_context.drbs.emplace(drb.first, drb.second);

    // Add to DRB-to-PDU session look-up table.
    context.drb_map.emplace(drb.first, pdu_session_context.id);

    // add QoS flows of the DRB to the map.
    for (const auto& flow : drb.second.qos_flows) {
      context.qos_flow_map.insert({flow.first, drb.first});
    }
  }
}

inline void apply_update_for_removed_drbs(up_pdu_session_context&      pdu_session_context,
                                          up_context&                  context,
                                          const std::vector<drb_id_t>& drb_to_remove)
{
  // Remove DRB and all mapped flows.
  for (const auto& drb_id : drb_to_remove) {
    // First remove all QoS flows from map.
    for (const auto& flow : pdu_session_context.drbs.at(drb_id).qos_flows) {
      context.qos_flow_map.erase(flow.first);
    }

    // Now remove DRB from PDU session and DRB map.
    pdu_session_context.drbs.erase(drb_id);
    context.drb_map.erase(drb_id);
  }
}

bool up_resource_manager_impl::apply_config_update(const up_config_update_result& result)
{
  // Apply config update in an additive way.
  for (const auto& session : result.pdu_sessions_added_list) {
    // Create new PDU session context.
    up_pdu_session_context new_ctxt(session.id);

    // Add new DRBs.
    apply_update_for_new_drbs(new_ctxt, context, session.drb_to_add);

    // Add PDU session to list of active sessions.
    context.pdu_sessions.emplace(new_ctxt.id, new_ctxt);
  }

  for (const auto& mod_session : result.pdu_sessions_modified_list) {
    srsran_assert(context.pdu_sessions.find(mod_session.id) != context.pdu_sessions.end(),
                  "PDU session {} not allocated",
                  mod_session.id);
    auto& session_context = context.pdu_sessions.at(mod_session.id);

    // Add new DRBs.
    apply_update_for_new_drbs(session_context, context, mod_session.drb_to_add);

    // Remove DRBs.
    apply_update_for_removed_drbs(session_context, context, mod_session.drb_to_remove);
  }

  for (const auto& rem_session : result.pdu_sessions_removed_list) {
    srsran_assert(context.pdu_sessions.find(rem_session) != context.pdu_sessions.end(),
                  "PDU session {} not allocated",
                  rem_session);

    auto& session_context = context.pdu_sessions.at(rem_session);

    // Remove allocated DRBs from map.
    for (const auto& drb : session_context.drbs) {
      // Remove all QoS flows.
      for (const auto& qos_flow : drb.second.qos_flows) {
        context.qos_flow_map.erase(qos_flow.first);
      }
      context.drb_map.erase(drb.first);
    }

    context.pdu_sessions.erase(rem_session);
  }

  return true;
}

const up_pdu_session_context& up_resource_manager_impl::get_pdu_session_context(pdu_session_id_t psi)
{
  srsran_assert(context.pdu_sessions.find(psi) != context.pdu_sessions.end(), "PDU session {} not allocated", psi);
  return context.pdu_sessions.at(psi);
}

bool up_resource_manager_impl::has_pdu_session(pdu_session_id_t pdu_session_id)
{
  return context.pdu_sessions.find(pdu_session_id) != context.pdu_sessions.end();
}

const up_drb_context& up_resource_manager_impl::get_drb_context(drb_id_t drb_id)
{
  srsran_assert(context.drb_map.find(drb_id) != context.drb_map.end(), "DRB ID {} not allocated", drb_id);
  const auto& psi = context.drb_map.at(drb_id);
  srsran_assert(context.pdu_sessions.find(psi) != context.pdu_sessions.end(), "Couldn't find PDU session ID {}", psi);
  srsran_assert(context.pdu_sessions.at(psi).drbs.find(drb_id) != context.pdu_sessions.at(psi).drbs.end(),
                "Couldn't find DRB id {} in PDU session {}",
                drb_id,
                psi);
  return context.pdu_sessions.at(psi).drbs.at(drb_id);
}

size_t up_resource_manager_impl::get_nof_drbs()
{
  return context.drb_map.size();
}

size_t up_resource_manager_impl::get_nof_pdu_sessions()
{
  return context.pdu_sessions.size();
}

size_t up_resource_manager_impl::get_nof_qos_flows(pdu_session_id_t psi)
{
  size_t nof_qos_flows = 0;

  // Look up specified session
  if (!has_pdu_session(psi)) {
    logger.error("PDU session {} not allocated", psi);
  } else {
    // Return number of active flows for specified session only.
    const auto& session = get_pdu_session_context(psi);
    for (const auto& drb : session.drbs) {
      nof_qos_flows += drb.second.qos_flows.size();
    }
  }

  return nof_qos_flows;
}

size_t up_resource_manager_impl::get_total_nof_qos_flows()
{
  // Return number of active QoS flows in all active sessions.
  return context.qos_flow_map.size();
}

std::vector<drb_id_t> up_resource_manager_impl::get_drbs()
{
  std::vector<drb_id_t> drb_ids;
  for (const auto& drb : context.drb_map) {
    drb_ids.push_back(drb.first);
  }

  return drb_ids;
}

const std::map<pdu_session_id_t, up_pdu_session_context>& up_resource_manager_impl::get_pdu_sessions_map()
{
  return context.pdu_sessions;
}

std::vector<pdu_session_id_t> up_resource_manager_impl::get_pdu_sessions()
{
  std::vector<pdu_session_id_t> pdu_session_ids;
  for (const auto& session : context.pdu_sessions) {
    pdu_session_ids.push_back(session.first);
  }

  return pdu_session_ids;
}

up_context up_resource_manager_impl::get_up_context()
{
  return context;
}

void up_resource_manager_impl::set_up_context(const up_context& ctx)
{
  context = ctx;
}
