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

bool up_resource_manager_impl::validate_request(const cu_cp_pdu_session_resource_setup_request& pdu)
{
  return is_valid(pdu, context, cfg, logger);
}

bool up_resource_manager_impl::validate_request(const cu_cp_pdu_session_resource_modify_request& pdu)
{
  return is_valid(pdu, context, cfg, logger);
}

up_config_update up_resource_manager_impl::calculate_update(const cu_cp_pdu_session_resource_setup_request& pdu)
{
  srsran_assert(is_valid(pdu, context, cfg, logger), "Invalid PDU Session Resource Setup request.");
  return srsran::srs_cu_cp::calculate_update(pdu, context, cfg, logger);
}

up_config_update up_resource_manager_impl::calculate_update(const cu_cp_pdu_session_resource_modify_request& pdu)
{
  srsran_assert(is_valid(pdu, context, cfg, logger), "Invalid PDU Session Resource Modify request.");
  return srsran::srs_cu_cp::calculate_update(pdu, context, cfg, logger);
}

bool up_resource_manager_impl::apply_config_update(const up_config_update_result& result)
{
  // Apply config update in an additive way.
  for (const auto& session : result.pdu_sessions_added_list) {
    // Create new PDU session context.
    up_pdu_session_context new_ctxt(session.id);
    for (const auto& drb : session.drb_to_add) {
      new_ctxt.drbs.emplace(drb.first, drb.second);

      // Add to DRB-to-PDU session look-up table.
      context.drb_map.emplace(drb.first, session.id);

      // add FiveQI to map of existing QoS groups.
      context.five_qi_map.emplace(drb.second.qos_params.qos_characteristics.get_five_qi(), drb.first);
    }

    // Add PDU session to list of active sessions.
    context.pdu_sessions.emplace(new_ctxt.id, new_ctxt);
  }
  return true;
}

up_pdu_session_context up_resource_manager_impl::get_context(pdu_session_id_t psi)
{
  srsran_assert(context.pdu_sessions.find(psi) != context.pdu_sessions.end(), "PDU session {} not allocated", psi);
  return context.pdu_sessions.at(psi);
}

up_drb_context up_resource_manager_impl::get_context(drb_id_t drb_id)
{
  srsran_assert(context.drb_map.find(drb_id) != context.drb_map.end(), "DRB ID {} not allocated", drb_id);
  const auto& psi = context.drb_map.find(drb_id);
  srsran_assert(
      context.pdu_sessions.find(psi->second) != context.pdu_sessions.end(), "PDU session {} not allocated", psi->first);
  return context.pdu_sessions.at(psi->second).drbs.at(drb_id);
}

bool up_resource_manager_impl::has_pdu_session(pdu_session_id_t pdu_session_id)
{
  return context.pdu_sessions.find(pdu_session_id) != context.pdu_sessions.end();
}

up_drb_context up_resource_manager_impl::get_drb(drb_id_t drb_id)
{
  if (context.drb_map.find(drb_id) == context.drb_map.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
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

std::vector<pdu_session_id_t> up_resource_manager_impl::get_pdu_sessions()
{
  std::vector<pdu_session_id_t> pdu_session_ids;
  for (const auto& session : context.pdu_sessions) {
    pdu_session_ids.push_back(session.first);
  }

  return pdu_session_ids;
}