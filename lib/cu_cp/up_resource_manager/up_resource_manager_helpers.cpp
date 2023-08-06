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

#include "up_resource_manager_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

// Verifies if any of the PDU sessions to be setup/modified already contains a DRB with the given ID.
bool contains_drb(const up_config_update& config_update, drb_id_t drb_id)
{
  for (const auto& setup_item : config_update.pdu_sessions_to_setup_list) {
    for (const auto& drb_item : setup_item.second.drb_to_add) {
      if (drb_item.first == drb_id) {
        return true;
      }
    }
  }

  for (const auto& modify_item : config_update.pdu_sessions_to_modify_list) {
    for (const auto& drb_item : modify_item.second.drb_to_add) {
      if (drb_item.first == drb_id) {
        return true;
      }
    }
  }
  return false;
}

bool contains_drb(const up_context& context, drb_id_t new_drb_id)
{
  for (const auto& drb : context.drb_map) {
    if (drb.first == new_drb_id) {
      return true;
    }
  }
  return false;
}

bool contains_drb(const up_pdu_session_context_update& new_session_context, drb_id_t new_drb_id)
{
  return (new_session_context.drb_to_add.find(new_drb_id) != new_session_context.drb_to_add.end());
}

drb_id_t srsran::srs_cu_cp::allocate_drb_id(const up_pdu_session_context_update& new_session_context,
                                            const up_context&                    context,
                                            const up_config_update&              config_update,
                                            const srslog::basic_logger&          logger)
{
  if (context.drb_map.size() >= MAX_NOF_DRBS) {
    logger.error("No more DRBs available");
    return drb_id_t::invalid;
  }

  drb_id_t new_drb_id = drb_id_t::drb1;
  // The new DRB ID must not be allocated already.
  while (contains_drb(context, new_drb_id) || contains_drb(config_update, new_drb_id) ||
         contains_drb(new_session_context, new_drb_id)) {
    /// try next
    new_drb_id = uint_to_drb_id(drb_id_to_uint(new_drb_id) + 1);
    if (new_drb_id == drb_id_t::invalid) {
      logger.error("No more DRBs available");
      return drb_id_t::invalid;
    }
  }

  return new_drb_id;
}

bool srsran::srs_cu_cp::is_valid(five_qi_t                      five_qi,
                                 const up_resource_manager_cfg& cfg,
                                 const srslog::basic_logger&    logger)
{
  if (cfg.five_qi_config.find(five_qi) == cfg.five_qi_config.end()) {
    logger.warning("No config for 5QI={} present", five_qi);
    logger.warning("Currently configured 5QIs:");
    for (const auto& qos : cfg.five_qi_config) {
      logger.warning(" - 5QI={}: PDCP={}", qos.first, qos.second.pdcp);
    }
    return false;
  }
  return true;
}

bool srsran::srs_cu_cp::is_valid(
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
    const up_context&                                                            context,
    const up_resource_manager_cfg&                                               cfg,
    const srslog::basic_logger&                                                  logger)
{
  // Reject empty setup requests.
  if (setup_items.empty()) {
    logger.error("Received empty setup list");
    return false;
  }

  // Reject request if PDU session with same ID already exists.
  for (const auto& pdu_session : setup_items) {
    if (context.pdu_sessions.find(pdu_session.pdu_session_id) != context.pdu_sessions.end()) {
      logger.error("PDU session ID {} already exists", pdu_session.pdu_session_id);
      return false;
    }

    // Reject request if OoS flow requirements can't be met.
    for (const auto& qos_flow : pdu_session.qos_flow_setup_request_items) {
      if (get_five_qi(qos_flow, cfg, logger) == five_qi_t::invalid) {
        logger.error("Configuration for {} can't be derived", qos_flow.qos_flow_id);
        return false;
      }
    }
  }

  return true;
}

/// \brief Validates an incoming PDU session modify request.
bool srsran::srs_cu_cp::is_valid(const cu_cp_pdu_session_resource_modify_request& pdu,
                                 const up_context&                                context,
                                 const up_resource_manager_cfg&                   cfg,
                                 const srslog::basic_logger&                      logger)
{
  // Reject empty modification requests.
  if (pdu.pdu_session_res_modify_items.empty()) {
    return false;
  }

  // Iterate over all modification items.
  for (const auto& pdu_session : pdu.pdu_session_res_modify_items) {
    // Reject request if PDU session with same ID does not already exists.
    if (context.pdu_sessions.find(pdu_session.pdu_session_id) == context.pdu_sessions.end()) {
      logger.error("Can't modify PDU session ID {} - session doesn't exist", pdu_session.pdu_session_id);
      return false;
    }

    // Reject request if OoS flow requirements can't be met.
    for (const auto& qos_flow : pdu_session.transfer.qos_flow_add_or_modify_request_list) {
      if (get_five_qi(qos_flow, cfg, logger) == five_qi_t::invalid) {
        logger.error("QoS flow configuration for flow ID {} can't be derived", qos_flow.qos_flow_id);
        return false;
      }
    }

    // Reject request if QoS flow to remove doesn't exist.
    for (const auto& qos_flow : pdu_session.transfer.qos_flow_to_release_list) {
      if (context.qos_flow_map.find(qos_flow.qos_flow_id) == context.qos_flow_map.end()) {
        logger.error("{} doesn't exist", qos_flow.qos_flow_id);
        return false;
      }
    }

    // Reject request if it removes all exisiting QoS flows.
    if (pdu_session.transfer.qos_flow_to_release_list.size() >= context.qos_flow_map.size()) {
      logger.error("Modification requests tries to remove {} from {} existing QoS flows.",
                   pdu_session.transfer.qos_flow_to_release_list.size(),
                   context.qos_flow_map.size());
      return false;
    }
  }

  return true;
}

/// \brief Validates an incoming PDU session release command.
bool srsran::srs_cu_cp::is_valid(const cu_cp_pdu_session_resource_release_command& pdu,
                                 const up_context&                                 context,
                                 const up_resource_manager_cfg&                    cfg,
                                 const srslog::basic_logger&                       logger)
{
  // Reject empty release requests.
  if (pdu.pdu_session_res_to_release_list_rel_cmd.empty()) {
    return false;
  }

  // Iterate over all release items.
  for (const auto& pdu_session : pdu.pdu_session_res_to_release_list_rel_cmd) {
    // Reject request if PDU session with same ID does not exist.
    if (context.pdu_sessions.find(pdu_session.pdu_session_id) == context.pdu_sessions.end()) {
      logger.error("Can't release PDU session ID {} - session doesn't exist", pdu_session.pdu_session_id);
      return false;
    }
  }

  return true;
}

/// \brief Allocates a QoS flow to a new DRB. Inserts it in PDU session object.
drb_id_t allocate_qos_flow(up_pdu_session_context_update&     new_session_context,
                           const qos_flow_setup_request_item& qos_flow,
                           const up_config_update&            config_update,
                           const up_context&                  full_context,
                           const up_resource_manager_cfg&     cfg,
                           const srslog::basic_logger&        logger)
{
  five_qi_t five_qi = get_five_qi(qos_flow, cfg, logger);
  srsran_assert(five_qi != five_qi_t::invalid, "5QI cannot be invalid.");

  // Note: We map QoS flows to DRBs in a 1:1 manner meaning that each flow gets it's own DRB.
  // potential optimization to support more QoS flows is to map non-GPB flows onto existing DRBs.
  drb_id_t drb_id = allocate_drb_id(new_session_context, full_context, config_update, logger);
  if (drb_id == drb_id_t::invalid) {
    logger.error("No more DRBs available");
    return drb_id;
  }

  up_drb_context drb_ctx;
  drb_ctx.drb_id         = drb_id;
  drb_ctx.pdu_session_id = new_session_context.id;
  drb_ctx.default_drb    = full_context.drb_map.empty() ? true : false; // make first DRB the default

  // Fill QoS (TODO: derive QoS params correctly)
  auto& qos_params = drb_ctx.qos_params;
  qos_params.qos_characteristics.non_dyn_5qi.emplace();
  qos_params.qos_characteristics.non_dyn_5qi.value().five_qi    = five_qi;
  qos_params.alloc_and_retention_prio.prio_level_arp            = 8;
  qos_params.alloc_and_retention_prio.pre_emption_cap           = "shall-not-trigger-pre-emption";
  qos_params.alloc_and_retention_prio.pre_emption_vulnerability = "not-pre-emptable";

  // Add flow
  up_qos_flow_context flow_ctx;
  flow_ctx.qfi        = qos_flow.qos_flow_id;
  flow_ctx.qos_params = qos_flow.qos_flow_level_qos_params;
  drb_ctx.qos_flows.emplace(flow_ctx.qfi, flow_ctx);

  // Set PDCP/SDAP config
  drb_ctx.pdcp_cfg = set_rrc_pdcp_config(five_qi, cfg);
  drb_ctx.sdap_cfg = set_rrc_sdap_config(drb_ctx);
  drb_ctx.rlc_mod  = (drb_ctx.pdcp_cfg.rlc_mode == pdcp_rlc_mode::am) ? rlc_mode::am : rlc_mode::um_bidir;

  // add new DRB to list
  new_session_context.drb_to_add.emplace(drb_id, drb_ctx);

  return drb_id;
}

up_config_update srsran::srs_cu_cp::calculate_update(
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
    const up_context&                                                            context,
    const up_resource_manager_cfg&                                               cfg,
    const srslog::basic_logger&                                                  logger)
{
  up_config_update config;

  config.initial_context_creation = context.pdu_sessions.empty();

  // look for existing DRB using the same FiveQI (does it need to be the same PDU session?)
  for (const auto& pdu_session : setup_items) {
    srsran_assert(context.pdu_sessions.find(pdu_session.pdu_session_id) == context.pdu_sessions.end(),
                  "PDU session already exists.");
    // Create new PDU session context.
    up_pdu_session_context_update new_ctxt(pdu_session.pdu_session_id);
    for (const auto& flow_item : pdu_session.qos_flow_setup_request_items) {
      auto drb_id = allocate_qos_flow(new_ctxt, flow_item, config, context, cfg, logger);
      logger.debug("Allocated {} to {} with {}",
                   flow_item.qos_flow_id,
                   drb_id,
                   new_ctxt.drb_to_add.at(drb_id).qos_params.qos_characteristics.get_five_qi());
    }
    config.pdu_sessions_to_setup_list.emplace(new_ctxt.id, new_ctxt);
  }

  return config;
}

/// \brief Determines the 5QI to use for QoS flow.
five_qi_t srsran::srs_cu_cp::get_five_qi(const cu_cp_qos_flow_add_or_mod_item& qos_flow,
                                         const up_resource_manager_cfg&        cfg,
                                         const srslog::basic_logger&           logger)
{
  five_qi_t   five_qi    = five_qi_t::invalid;
  const auto& qos_params = qos_flow.qos_flow_level_qos_params;

  if (qos_params.qos_characteristics.dyn_5qi.has_value()) {
    if (qos_params.qos_characteristics.dyn_5qi.value().five_qi.has_value()) {
      five_qi = qos_params.qos_characteristics.dyn_5qi.value().five_qi.value();
    } else {
      logger.error("Dynamic 5QI without 5QI not supported");
      return five_qi_t::invalid;
    }
  } else if (qos_params.qos_characteristics.non_dyn_5qi.has_value()) {
    five_qi = qos_params.qos_characteristics.non_dyn_5qi.value().five_qi;
  } else {
    logger.error("Invalid QoS characteristics. Either dynamic or non-dynamic 5QI must be set");
    return five_qi_t::invalid;
  }

  // Check if we have a valid config for the selected 5QI.
  if (!is_valid(five_qi, cfg, logger)) {
    logger.error("No valid config for {}", five_qi);
    return five_qi_t::invalid;
  }

  return five_qi;
}

up_config_update srsran::srs_cu_cp::calculate_update(const cu_cp_pdu_session_resource_modify_request& pdu,
                                                     const up_context&                                context,
                                                     const up_resource_manager_cfg&                   cfg,
                                                     const srslog::basic_logger&                      logger)
{
  up_config_update update;
  update.initial_context_creation = false;

  for (const auto& modify_item : pdu.pdu_session_res_modify_items) {
    srsran_assert(context.pdu_sessions.find(modify_item.pdu_session_id) != context.pdu_sessions.end(),
                  "PDU session does not exist.");

    up_pdu_session_context_update ctxt_update(modify_item.pdu_session_id);
    for (const auto& flow_item : modify_item.transfer.qos_flow_add_or_modify_request_list) {
      auto drb_id = allocate_qos_flow(ctxt_update, flow_item, update, context, cfg, logger);
      if (drb_id == drb_id_t::invalid) {
        logger.error("Couldn't allocate {}", flow_item.qos_flow_id);
        update.pdu_sessions_failed_to_modify_list.push_back(modify_item.pdu_session_id);
      } else {
        srsran_assert(context.drb_map.find(drb_id) != context.drb_map.end() ||
                          ctxt_update.drb_to_add.find(drb_id) != ctxt_update.drb_to_add.end(),
                      "{} has to exist in current PDU session context or in context update",
                      drb_id);
        logger.debug("Allocated {} to {}", flow_item.qos_flow_id, drb_id);
      }
    }

    for (const auto& flow_item : modify_item.transfer.qos_flow_to_release_list) {
      auto drb_id = context.qos_flow_map.at(flow_item.qos_flow_id);
      logger.debug("Releasing {} on {}", flow_item.qos_flow_id, drb_id);

      // Release DRB if this is the only flow mapped to it.
      const auto& pdu_session_ctxt = context.pdu_sessions.at(modify_item.pdu_session_id);
      srsran_assert(pdu_session_ctxt.drbs.at(drb_id).qos_flows.find(flow_item.qos_flow_id) !=
                        pdu_session_ctxt.drbs.at(drb_id).qos_flows.end(),
                    "{} not mapped on {}",
                    flow_item.qos_flow_id,
                    drb_id);
      if (pdu_session_ctxt.drbs.at(drb_id).qos_flows.size() == 1) {
        // Flow is the only active flow on this DRB - release it.
        logger.debug("Releasing {}", drb_id);
        ctxt_update.drb_to_remove.push_back(drb_id);
      }
    }

    update.pdu_sessions_to_modify_list.emplace(ctxt_update.id, ctxt_update);
  }

  return update;
}

up_config_update srsran::srs_cu_cp::calculate_update(const cu_cp_pdu_session_resource_release_command& pdu,
                                                     const up_context&                                 context,
                                                     const up_resource_manager_cfg&                    cfg,
                                                     const srslog::basic_logger&                       logger)
{
  up_config_update update;

  for (const auto& release_item : pdu.pdu_session_res_to_release_list_rel_cmd) {
    // Release all DRBs.
    const auto& session_context = context.pdu_sessions.at(release_item.pdu_session_id);
    for (const auto& drb : session_context.drbs) {
      update.drb_to_remove_list.push_back(drb.first);
    }
    update.pdu_sessions_to_remove_list.push_back(release_item.pdu_session_id);
  }

  // Request context release if all active PDU session are going to be released.
  if (update.pdu_sessions_to_remove_list.size() == context.pdu_sessions.size()) {
    logger.debug("UE context removal required as all PDU sessions get released");
    update.context_removal_required = true;
  }

  return update;
}

sdap_config_t srsran::srs_cu_cp::set_rrc_sdap_config(const up_drb_context& context)
{
  sdap_config_t sdap_cfg;
  sdap_cfg.pdu_session = context.pdu_session_id;
  sdap_cfg.default_drb = context.default_drb;
  sdap_cfg.sdap_hdr_dl = sdap_hdr_dl_cfg::absent;
  sdap_cfg.sdap_hdr_ul = sdap_hdr_ul_cfg::absent;
  for (const auto& qos_flow : context.qos_flows) {
    sdap_cfg.mapped_qos_flows_to_add.push_back(qos_flow.first);
  }
  return sdap_cfg;
}

pdcp_config srsran::srs_cu_cp::set_rrc_pdcp_config(five_qi_t five_qi, const up_resource_manager_cfg& cfg)
{
  srsran_assert(cfg.five_qi_config.find(five_qi) != cfg.five_qi_config.end(),
                "Could not find PDCP configuration. 5QI {}",
                five_qi);

  return cfg.five_qi_config.at(five_qi).pdcp;
}

up_config_update srsran::srs_cu_cp::to_config_update(const up_context& old_context)
{
  up_config_update config;

  for (const auto& pdu_session : old_context.pdu_sessions) {
    // Create new PDU session context.
    up_pdu_session_context_update new_ctxt(pdu_session.first);
    for (const auto& drb : pdu_session.second.drbs) {
      // Add all existing DRBs.
      new_ctxt.drb_to_add.emplace(drb.first, drb.second);
    }
    config.pdu_sessions_to_setup_list.emplace(new_ctxt.id, new_ctxt);
  }

  return config;
}