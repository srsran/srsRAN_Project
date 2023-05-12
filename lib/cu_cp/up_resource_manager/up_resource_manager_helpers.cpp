/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "up_resource_manager_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

drb_id_t srsran::srs_cu_cp::allocate_drb_id(const up_context& context, srslog::basic_logger& logger)
{
  if (context.drb_map.size() >= MAX_NOF_DRBS) {
    logger.error("No more DRBs available");
    return drb_id_t::invalid;
  }

  drb_id_t new_drb_id = drb_id_t::drb1;
  for (const auto& drb : context.drb_map) {
    if (drb.first != new_drb_id) {
      return new_drb_id;
    } else {
      /// try next
      new_drb_id = uint_to_drb_id(drb_id_to_uint(new_drb_id) + 1);
      if (new_drb_id == drb_id_t::invalid) {
        logger.error("No more DRBs available");
        return drb_id_t::invalid;
      }
    }
  }
  return new_drb_id;
}

bool srsran::srs_cu_cp::is_valid(const cu_cp_pdu_session_resource_setup_request& pdu, const up_context& context)
{
  // Reject request if PDU session with same ID already exists.
  for (const auto& pdu_session : pdu.pdu_session_res_setup_items) {
    if (context.pdu_sessions.find(pdu_session.pdu_session_id) != context.pdu_sessions.end()) {
      return false;
    }
  }
  return true;
}

up_config_update srsran::srs_cu_cp::calculate_update(const cu_cp_pdu_session_resource_setup_request& pdu,
                                                     const up_context&                               context,
                                                     const up_resource_manager_cfg&                  cfg,
                                                     srslog::basic_logger&                           logger)
{
  up_config_update config;

  config.initial_context_creation = context.pdu_sessions.empty();

  // look for existing DRB using the same FiveQI (does it need to be the same PDU session?)
  for (const auto& pdu_session : pdu.pdu_session_res_setup_items) {
    srsran_assert(context.pdu_sessions.find(pdu_session.pdu_session_id) == context.pdu_sessions.end(),
                  "PDU session already exists.");
    // Create new PDU session context.
    up_pdu_session_context new_ctxt;
    new_ctxt.id = pdu_session.pdu_session_id;
    for (const auto& qos_flow : pdu_session.qos_flow_setup_request_items) {
      // TODO: check if FiveQI has configuration

      five_qi_t five_qi;
      if (qos_flow.qos_flow_level_qos_params.qos_characteristics.dyn_5qi.has_value()) {
        if (qos_flow.qos_flow_level_qos_params.qos_characteristics.dyn_5qi.value().five_qi.has_value()) {
          five_qi = qos_flow.qos_flow_level_qos_params.qos_characteristics.dyn_5qi.value().five_qi.value();
        } else {
          report_fatal_error("Dynamic 5qi without 5qi not supported");
        }
      } else if (qos_flow.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.has_value()) {
        five_qi = qos_flow.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value().five_qi;
      } else {
        report_fatal_error("Invalid QoS characteristics. Either dynamic or non-dynamic 5qi must be set");
      }

      // Check if other DRB with same FiveQI exists.
      // Note: the current assumption is that there is no need to have a second DRB with the same 5QI.
      if (context.five_qi_map.find(five_qi) == context.five_qi_map.end()) {
        // no existing DRB with same FiveQI, create new DRB
        drb_id_t id = allocate_drb_id(context, logger);
        if (id == drb_id_t::invalid) {
          logger.error("No more DRBs available");
          return config;
        }

        up_drb_context drb_ctx;
        drb_ctx.drb_id         = id;
        drb_ctx.pdu_session_id = pdu_session.pdu_session_id;
        drb_ctx.default_drb    = context.drb_map.empty() ? true : false; // make first DRB the default
        drb_ctx.five_qi        = five_qi;
        drb_ctx.pdcp_cfg       = set_rrc_pdcp_config(drb_ctx.five_qi, cfg);
        drb_ctx.qos_flows.push_back(qos_flow.qos_flow_id);
        drb_ctx.sdap_cfg = set_rrc_sdap_config(drb_ctx);

        // add new DRB to list
        new_ctxt.drbs.emplace(id, drb_ctx);
      } else {
        logger.error("Setup of new QoS flow not supported yet.");
      }
    }
    config.pdu_sessions_to_setup_list.emplace(new_ctxt.id, new_ctxt);
  }

  return config;
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
