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
bool includes_drb_to_setup(const up_config_update& config_update, drb_id_t drb_id)
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

bool includes_drb(const up_context& context, drb_id_t new_drb_id)
{
  for (const auto& drb : context.drb_map) {
    if (drb.first == new_drb_id) {
      return true;
    }
  }
  return false;
}

drb_id_t srsran::srs_cu_cp::allocate_drb_id(const up_context&           context,
                                            const up_config_update&     config_update,
                                            const srslog::basic_logger& logger)
{
  if (context.drb_map.size() >= MAX_NOF_DRBS) {
    logger.error("No more DRBs available");
    return drb_id_t::invalid;
  }

  drb_id_t new_drb_id = drb_id_t::drb1;
  while (includes_drb(context, new_drb_id) || includes_drb_to_setup(config_update, new_drb_id)) {
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

bool srsran::srs_cu_cp::is_valid(const cu_cp_pdu_session_resource_setup_request& pdu,
                                 const up_context&                               context,
                                 const up_resource_manager_cfg&                  cfg,
                                 const srslog::basic_logger&                     logger)
{
  // Reject empty setup requests.
  if (pdu.pdu_session_res_setup_items.empty()) {
    logger.error("Received empty setup list");
    return false;
  }

  // Reject request if PDU session with same ID already exists.
  for (const auto& pdu_session : pdu.pdu_session_res_setup_items) {
    if (context.pdu_sessions.find(pdu_session.pdu_session_id) != context.pdu_sessions.end()) {
      logger.error("PDU session ID {} already exists", pdu_session.pdu_session_id);
      return false;
    }

    // Reject request if OoS flow requirements can't be met.
    for (const auto& qos_flow : pdu_session.qos_flow_setup_request_items) {
      if (get_five_qi(qos_flow, cfg, logger) == five_qi_t::invalid) {
        logger.error("OoS flow configuration for flow ID {} can't be derived", qos_flow.qos_flow_id);
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
        logger.error("OoS flow configuration for flow ID {} can't be derived", qos_flow.qos_flow_id);
        return false;
      }
    }
  }

  return true;
}

/// \brief Allocates a QoS to a DRB. Creates a new DRB if needed. Inserts it in PDU session object.
drb_id_t allocate_qos_flow(up_pdu_session_context_update&     session_context,
                           const qos_flow_setup_request_item& qos_flow,
                           const up_config_update&            config_update,
                           const up_context&                  context,
                           const up_resource_manager_cfg&     cfg,
                           const srslog::basic_logger&        logger)
{
  five_qi_t five_qi = get_five_qi(qos_flow, cfg, logger);
  srsran_assert(five_qi != five_qi_t::invalid, "5QI cannot be invalid.");
  drb_id_t drb_id = drb_id_t::invalid;

  // Check if other DRB with same FiveQI exists.
  // Note : the current assumption is that there is no need to have a second DRB with the same 5QI.
  if (context.five_qi_map.find(five_qi) == context.five_qi_map.end()) {
    // no existing DRB with same FiveQI, create new DRB
    drb_id = allocate_drb_id(context, config_update, logger);
    if (drb_id == drb_id_t::invalid) {
      logger.error("No more DRBs available");
      return drb_id;
    }

    up_drb_context drb_ctx;
    drb_ctx.drb_id         = drb_id;
    drb_ctx.pdu_session_id = session_context.id;
    drb_ctx.default_drb    = context.drb_map.empty() ? true : false; // make first DRB the default

    // Fill QoS (TODO: derive QoS params correctly)
    auto& qos_params = drb_ctx.qos_params;
    qos_params.qos_characteristics.non_dyn_5qi.emplace();
    qos_params.qos_characteristics.non_dyn_5qi.value().five_qi    = five_qi;
    qos_params.alloc_and_retention_prio.prio_level_arp            = 8;
    qos_params.alloc_and_retention_prio.pre_emption_cap           = "not-pre-emptable";
    qos_params.alloc_and_retention_prio.pre_emption_vulnerability = "not-pre-emptable";

    // Add flow
    drb_ctx.qos_flows.push_back(qos_flow.qos_flow_id);

    // Set PDCP/SDAP config
    drb_ctx.pdcp_cfg = set_rrc_pdcp_config(five_qi, cfg);
    drb_ctx.sdap_cfg = set_rrc_sdap_config(drb_ctx);

    // add new DRB to list
    session_context.drb_to_add.emplace(drb_id, drb_ctx);
  } else {
    // lookup existing DRB and add flow
    drb_id = context.five_qi_map.at(five_qi);
    logger.error("Addition of QoS flow to DRB not implemented.");
  }

  return drb_id;
}

up_config_update srsran::srs_cu_cp::calculate_update(const cu_cp_pdu_session_resource_setup_request& pdu,
                                                     const up_context&                               context,
                                                     const up_resource_manager_cfg&                  cfg,
                                                     const srslog::basic_logger&                     logger)
{
  up_config_update config;

  config.initial_context_creation = context.pdu_sessions.empty();

  // look for existing DRB using the same FiveQI (does it need to be the same PDU session?)
  for (const auto& pdu_session : pdu.pdu_session_res_setup_items) {
    srsran_assert(context.pdu_sessions.find(pdu_session.pdu_session_id) == context.pdu_sessions.end(),
                  "PDU session already exists.");
    // Create new PDU session context.
    up_pdu_session_context_update new_ctxt(pdu_session.pdu_session_id);
    for (const auto& flow_item : pdu_session.qos_flow_setup_request_items) {
      auto drb_id = allocate_qos_flow(new_ctxt, flow_item, config, context, cfg, logger);
      logger.debug("Allocated QoS flow ID {} to DRB ID {} with 5QI {}",
                   flow_item.qos_flow_id,
                   drb_id,
                   new_ctxt.drb_to_add.at(drb_id).qos_params.qos_characteristics.get_five_qi());
    }
    config.pdu_sessions_to_setup_list.emplace(new_ctxt.id, new_ctxt);
  }

  return config;
}

/// \brief Determines the 5QI to use for QoS flow.
five_qi_t srsran::srs_cu_cp::get_five_qi(const qos_flow_add_or_mod_item& qos_flow,
                                         const up_resource_manager_cfg&  cfg,
                                         const srslog::basic_logger&     logger)
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
    logger.error("No valid config for 5QI {}", five_qi_to_uint(five_qi));
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

  logger.debug("PDU session modifcation with {} items.", pdu.pdu_session_res_modify_items.size());
  for (const auto& modify_item : pdu.pdu_session_res_modify_items) {
    srsran_assert(context.pdu_sessions.find(modify_item.pdu_session_id) != context.pdu_sessions.end(),
                  "PDU session does not exist.");

    up_pdu_session_context_update ctxt(modify_item.pdu_session_id);
    logger.debug("modify_item.transfer.qos_flow_add_or_modify_request_list with {} items.",
                 modify_item.transfer.qos_flow_add_or_modify_request_list.size());
    for (const auto& flow_item : modify_item.transfer.qos_flow_add_or_modify_request_list) {
      auto drb_id = allocate_qos_flow(ctxt, flow_item, update, context, cfg, logger);
      logger.debug("Allocated QoS flow ID {} to DRB ID {} with 5QI {}",
                   flow_item.qos_flow_id,
                   drb_id,
                   ctxt.drb_to_add.at(drb_id).qos_params.qos_characteristics.get_five_qi());
    }

    update.pdu_sessions_to_modify_list.emplace(ctxt.id, ctxt);
  }

  return update;
}

sdap_config_t srsran::srs_cu_cp::set_rrc_sdap_config(const up_drb_context& context)
{
  sdap_config_t sdap_cfg;
  sdap_cfg.pdu_session = context.pdu_session_id;
  sdap_cfg.default_drb = context.default_drb;
  sdap_cfg.sdap_hdr_dl = "absent";
  sdap_cfg.sdap_hdr_ul = "absent";
  for (const auto& qos_flow : context.qos_flows) {
    sdap_cfg.mapped_qos_flows_to_add.push_back(qos_flow);
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
