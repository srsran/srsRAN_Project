/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

/// Verifies if any of the PDU sessions to be setup/modified already contains a DRB with the given ID.
static bool contains_drb(const up_config_update& config_update, drb_id_t drb_id)
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

static bool contains_drb(const up_context& context, drb_id_t new_drb_id)
{
  for (const auto& drb : context.drb_map) {
    if (drb.first == new_drb_id) {
      return true;
    }
  }
  return false;
}

static bool contains_drb(const up_pdu_session_context_update& new_session_context, drb_id_t new_drb_id)
{
  return (new_session_context.drb_to_add.find(new_drb_id) != new_session_context.drb_to_add.end());
}

drb_id_t srsran::srs_cu_cp::allocate_drb_id(const up_pdu_session_context_update& new_session_context,
                                            const up_context&                    context,
                                            const up_config_update&              config_update,
                                            uint8_t                              max_nof_drbs_per_ue,
                                            const srslog::basic_logger&          logger)
{
  if (context.drb_map.size() >= max_nof_drbs_per_ue) {
    logger.warning("DRB creation failed. Cause: Maximum number of DRBs per UE already created ({}). To increase the "
                   "number of allowed DRBs per UE change the \"--max_nof_drbs_per_ue\" in the CU-CP configuration\n",
                   max_nof_drbs_per_ue);
    return drb_id_t::invalid;
  }

  drb_id_t new_drb_id = drb_id_t::drb1;
  // The new DRB ID must not be allocated already.
  while (contains_drb(context, new_drb_id) || contains_drb(config_update, new_drb_id) ||
         contains_drb(new_session_context, new_drb_id)) {
    // Try next.
    new_drb_id = uint_to_drb_id(drb_id_to_uint(new_drb_id) + 1);

    if (drb_id_to_uint(new_drb_id) > max_nof_drbs_per_ue) {
      logger.warning("DRB creation failed. Cause: Maximum number of DRBs per UE already created ({}). To increase the "
                     "number of allowed DRBs per UE change the \"--max_nof_drbs_per_ue\" in the CU-CP configuration\n",
                     max_nof_drbs_per_ue);
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
    logger.warning("No config for {} present", five_qi);
    logger.warning("Currently configured 5QIs:");
    for (const auto& qos : cfg.five_qi_config) {
      logger.warning(" - {}: {}", qos.first, qos.second.pdcp);
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
    logger.warning("Received empty setup list");
    return false;
  }

  // Reject request if PDU session with same ID already exists.
  for (const auto& pdu_session : setup_items) {
    if (context.pdu_sessions.find(pdu_session.pdu_session_id) != context.pdu_sessions.end()) {
      logger.info("PDU session ID {} already exists", pdu_session.pdu_session_id);
      return false;
    }

    // Reject request if OoS flow requirements can't be met.
    for (const auto& qos_flow : pdu_session.qos_flow_setup_request_items) {
      if (get_five_qi(qos_flow, cfg, logger) == five_qi_t::invalid) {
        logger.warning("Configuration for {} can't be derived", qos_flow.qos_flow_id);
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
      logger.warning("Can't modify PDU session ID {} - session doesn't exist", pdu_session.pdu_session_id);
      return false;
    }

    // Reject request if QoS flow requirements can't be met.
    for (const auto& qos_flow : pdu_session.transfer.qos_flow_add_or_modify_request_list) {
      if (get_five_qi(qos_flow, cfg, logger) == five_qi_t::invalid) {
        logger.warning("QoS flow configuration for {} can't be derived", qos_flow.qos_flow_id);
        return false;
      }
    }

    // Reject request if QoS flow to remove doesn't exist.
    for (const auto& qos_flow : pdu_session.transfer.qos_flow_to_release_list) {
      if (context.qos_flow_map.find(qos_flow.qos_flow_id) == context.qos_flow_map.end()) {
        logger.warning("{} doesn't exist", qos_flow.qos_flow_id);
        return false;
      }
    }

    // Reject request if it removes all exisiting QoS flows.
    if (pdu_session.transfer.qos_flow_to_release_list.size() >= context.qos_flow_map.size()) {
      logger.warning("Modification request tries to remove {} from {} existing QoS flows",
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
      logger.warning("Can't release PDU session ID {} - session doesn't exist", pdu_session.pdu_session_id);
      return false;
    }
  }

  return true;
}

/// \brief Allocates a QoS flow to a new DRB. Inserts it in PDU session object.
static drb_id_t allocate_qos_flow(up_pdu_session_context_update&     new_session_context,
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
  if (full_context.drb_map.size() >= cfg.max_nof_drbs_per_ue) {
    logger.warning("DRB creation failed. Cause: Maximum number of DRBs per UE already created ({}). To increase the "
                   "number of allowed DRBs per UE change the \"--max_nof_drbs_per_ue\" in the CU-CP configuration",
                   cfg.max_nof_drbs_per_ue);
    return drb_id_t::invalid;
  }

  drb_id_t drb_id = allocate_drb_id(new_session_context, full_context, config_update, cfg.max_nof_drbs_per_ue, logger);
  if (drb_id == drb_id_t::invalid) {
    logger.warning("No more DRBs available");
    return drb_id;
  }

  up_drb_context drb_ctx;
  drb_ctx.drb_id         = drb_id;
  drb_ctx.pdu_session_id = new_session_context.id;
  drb_ctx.default_drb    = full_context.drb_map.empty(); // make first DRB the default

  // Fill QoS (TODO: derive QoS params correctly).
  // As we currently map QoS flows to DRBs in a 1:1 manner, we can use the same values for both.
  drb_ctx.qos_params = qos_flow.qos_flow_level_qos_params;

  // Add flow.
  up_qos_flow_context flow_ctx;
  flow_ctx.qfi        = qos_flow.qos_flow_id;
  flow_ctx.qos_params = qos_flow.qos_flow_level_qos_params;
  drb_ctx.qos_flows.emplace(flow_ctx.qfi, flow_ctx);

  // Set PDCP/SDAP config.
  drb_ctx.pdcp_cfg = set_rrc_pdcp_config(five_qi, cfg);
  drb_ctx.sdap_cfg = set_rrc_sdap_config(drb_ctx);
  drb_ctx.rlc_mod  = (drb_ctx.pdcp_cfg.rlc_mode == pdcp_rlc_mode::am) ? rlc_mode::am : rlc_mode::um_bidir;

  // Add new DRB to list.
  new_session_context.drb_to_add.emplace(drb_id, drb_ctx);

  return drb_id;
}

/// \brief Modifies a QoS flow for an existing DRB. Inserts it in PDU session object.
static drb_id_t modify_qos_flow(up_pdu_session_context_update&     new_session_context,
                                const qos_flow_setup_request_item& qos_flow,
                                const up_config_update&            config_update,
                                const up_context&                  full_context,
                                const up_resource_manager_cfg&     cfg,
                                const srslog::basic_logger&        logger)
{
  // Note: We map QoS flows to DRBs in a 1:1 manner meaning that each flow gets it's own DRB.
  // potential optimization to support more QoS flows is to map non-GPB flows onto existing DRBs.
  if (full_context.qos_flow_map.find(qos_flow.qos_flow_id) == full_context.qos_flow_map.end()) {
    logger.warning("{} does not exist", qos_flow.qos_flow_id);
    return drb_id_t::invalid;
  }

  drb_id_t drb_id = full_context.qos_flow_map.at(qos_flow.qos_flow_id);

  up_drb_context drb_ctx;
  drb_ctx.drb_id         = drb_id;
  drb_ctx.pdu_session_id = new_session_context.id;
  drb_ctx.qos_params     = qos_flow.qos_flow_level_qos_params;

  // Add flow.
  up_qos_flow_context flow_ctx;
  flow_ctx.qfi        = qos_flow.qos_flow_id;
  flow_ctx.qos_params = qos_flow.qos_flow_level_qos_params;
  drb_ctx.qos_flows.emplace(flow_ctx.qfi, flow_ctx);

  // Set PDCP/SDAP config.
  drb_ctx.pdcp_cfg = set_rrc_pdcp_config(qos_flow.qos_flow_level_qos_params.qos_desc.get_5qi(), cfg);
  drb_ctx.sdap_cfg = set_rrc_sdap_config(drb_ctx);
  drb_ctx.rlc_mod  = (drb_ctx.pdcp_cfg.rlc_mode == pdcp_rlc_mode::am) ? rlc_mode::am : rlc_mode::um_bidir;

  // Add modified DRB to list.
  new_session_context.drb_to_modify.emplace(drb_id, drb_ctx);

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

  // Look for existing DRB using the same FiveQI (does it need to be the same PDU session?).
  for (const auto& pdu_session : setup_items) {
    srsran_assert(context.pdu_sessions.find(pdu_session.pdu_session_id) == context.pdu_sessions.end(),
                  "PDU session already exists");
    // Create new PDU session context.
    up_pdu_session_context_update new_ctxt(pdu_session.pdu_session_id);
    for (const auto& flow_item : pdu_session.qos_flow_setup_request_items) {
      auto drb_id = allocate_qos_flow(new_ctxt, flow_item, config, context, cfg, logger);
      if (drb_id == drb_id_t::invalid) {
        logger.warning("Couldn't allocate {}", flow_item.qos_flow_id);
        continue;
      }
      // S-NSSAI
      new_ctxt.drb_to_add.at(drb_id).s_nssai = pdu_session.s_nssai;
      logger.debug("Allocated {} to {} with {}",
                   flow_item.qos_flow_id,
                   drb_id,
                   new_ctxt.drb_to_add.at(drb_id).qos_params.qos_desc.get_5qi());
    }
    if (!new_ctxt.drb_to_add.empty()) {
      config.pdu_sessions_to_setup_list.emplace(new_ctxt.id, new_ctxt);
    }
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

  five_qi = qos_params.qos_desc.get_5qi();
  if (five_qi == five_qi_t::invalid) {
    logger.warning("Dynamic 5QI without 5QI not supported");
    return five_qi_t::invalid;
  }

  // Check if we have a valid config for the selected 5QI.
  if (!is_valid(five_qi, cfg, logger)) {
    logger.warning("No valid config for {}", five_qi);
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
      // If QoS flow already exists, modify it.
      if (context.qos_flow_map.find(flow_item.qos_flow_id) != context.qos_flow_map.end()) {
        auto drb_id = modify_qos_flow(ctxt_update, flow_item, update, context, cfg, logger);
        if (drb_id == drb_id_t::invalid) {
          logger.warning("Couldn't modify {}", flow_item.qos_flow_id);
          update.pdu_sessions_failed_to_modify_list.push_back(modify_item.pdu_session_id);
        } else {
          srsran_assert(context.drb_map.find(drb_id) != context.drb_map.end() ||
                            ctxt_update.drb_to_modify.find(drb_id) != ctxt_update.drb_to_modify.end(),
                        "{} has to exist in current PDU session context or in context update",
                        drb_id);
          logger.debug("Modified {} on {}", flow_item.qos_flow_id, drb_id);
        }
      } else {
        auto drb_id = allocate_qos_flow(ctxt_update, flow_item, update, context, cfg, logger);
        if (drb_id == drb_id_t::invalid) {
          logger.warning("Couldn't allocate {}", flow_item.qos_flow_id);
          update.pdu_sessions_failed_to_modify_list.push_back(modify_item.pdu_session_id);
        } else {
          srsran_assert(context.drb_map.find(drb_id) != context.drb_map.end() ||
                            ctxt_update.drb_to_add.find(drb_id) != ctxt_update.drb_to_add.end(),
                        "{} has to exist in current PDU session context or in context update",
                        drb_id);
          logger.debug("Allocated {} to {}", flow_item.qos_flow_id, drb_id);
        }
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
      logger.debug("Removing {}", drb.first);
      update.drb_to_remove_list.push_back(drb.first);
    }
    update.pdu_sessions_to_remove_list.push_back(release_item.pdu_session_id);
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
