/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "drb_manager_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

drb_manager_impl::drb_manager_impl(const drb_manager_cfg& cfg_) : cfg(cfg_), logger(srslog::fetch_basic_logger("RRC"))
{
}

up_config_update drb_manager_impl::calculate_update(const cu_cp_pdu_session_resource_setup_request& pdu)
{
  up_config_update config;

  config.initial_context_creation = pdu_sessions.empty();

  // look for existing DRB using the same FiveQI (does it need to be the same PDU session?)
  for (const auto& pdu_session : pdu.pdu_session_res_setup_items) {
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

      // check if other DRB with same FiveQI exists
      if (five_qi_map.find(five_qi) == five_qi_map.end()) {
        // no existing DRB with same FiveQI, create new DRB
        drb_id_t id = allocate_drb_id();
        if (id == drb_id_t::invalid) {
          logger.error("No more DRBs available");
          return config;
        }

        drb_context drb_ctx;
        drb_ctx.drb_id         = id;
        drb_ctx.pdu_session_id = pdu_session.pdu_session_id;
        drb_ctx.default_drb    = drbs.empty() ? true : false; // make first DRB the default
        drb_ctx.five_qi        = five_qi;
        drb_ctx.pdcp_cfg       = set_rrc_pdcp_config(drb_ctx.five_qi);
        drb_ctx.mapped_qos_flows.push_back(qos_flow.qos_flow_id);
        drb_ctx.sdap_cfg = set_rrc_sdap_config(drb_ctx);

        // add new DRB to list
        drbs.emplace(id, drb_ctx);

        // add FiveQI to map
        five_qi_map.emplace(drb_ctx.five_qi, id);

        // enqueue as DRB to be added
        config.drb_to_add_list.push_back(id);
      }
    }

    pdu_sessions.emplace(pdu_session.pdu_session_id, true);
  }

  return config;
}

drb_id_t drb_manager_impl::allocate_drb_id()
{
  if (drbs.size() >= MAX_NOF_DRBS) {
    logger.error("No more DRBs available");
    return drb_id_t::invalid;
    ;
  }

  drb_id_t new_drb_id = drb_id_t::drb1;
  for (const auto& drb : drbs) {
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

sdap_config_t drb_manager_impl::set_rrc_sdap_config(const drb_context& context)
{
  sdap_config_t sdap_cfg;
  sdap_cfg.pdu_session = context.pdu_session_id;
  sdap_cfg.default_drb = context.default_drb;
  sdap_cfg.sdap_hdr_dl = "absent";
  sdap_cfg.sdap_hdr_ul = "absent";
  for (const auto& qos_flow : context.mapped_qos_flows) {
    sdap_cfg.mapped_qos_flows_to_add.push_back(qos_flow);
  }
  return sdap_cfg;
}

pdcp_config drb_manager_impl::set_rrc_pdcp_config(five_qi_t five_qi)
{
  srsran_assert(cfg.five_qi_config.find(five_qi) != cfg.five_qi_config.end(),
                "Could not find PDCP configuration. 5QI {}",
                five_qi);

  return cfg.five_qi_config.at(five_qi).pdcp;
}

pdu_session_id_t drb_manager_impl::get_pdu_session_id(drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return pdu_session_id_t::invalid;
  }
  return drbs[drb_id].pdu_session_id;
}

std::vector<qos_flow_id_t> drb_manager_impl::get_mapped_qos_flows(drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return drbs[drb_id].mapped_qos_flows;
}

std::vector<qos_flow_id_t> drb_manager_impl::get_mapped_qos_flows(pdu_session_id_t pdu_session_id)
{
  std::vector<qos_flow_id_t> mapped_flows;

  for (const auto& drb : drbs) {
    if (drb.second.pdu_session_id == pdu_session_id) {
      for (const auto& flow : drb.second.mapped_qos_flows) {
        mapped_flows.push_back(flow);
      }
    }
  }
  return mapped_flows;
}

std::vector<drb_id_t> drb_manager_impl::get_drbs(pdu_session_id_t pdu_session_id)
{
  std::vector<drb_id_t> mapped_drbs;

  for (const auto& drb : drbs) {
    if (drb.second.pdu_session_id == pdu_session_id) {
      mapped_drbs.push_back(drb.first);
    }
  }
  return mapped_drbs;
}

pdcp_config drb_manager_impl::get_pdcp_config(drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return drbs[drb_id].pdcp_cfg;
}

sdap_config_t drb_manager_impl::get_sdap_config(drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return drbs[drb_id].sdap_cfg;
}

s_nssai_t drb_manager_impl::get_s_nssai(drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return drbs[drb_id].s_nssai;
}

size_t drb_manager_impl::get_nof_drbs()
{
  return drbs.size();
}

size_t drb_manager_impl::get_nof_pdu_sessions()
{
  return pdu_sessions.size();
}

std::vector<pdu_session_id_t> drb_manager_impl::get_pdu_sessions()
{
  std::vector<pdu_session_id_t> pdu_session_ids;
  for (const auto& session : pdu_sessions) {
    pdu_session_ids.push_back(session.first);
  }

  return pdu_session_ids;
}

bool drb_manager_impl::valid_5qi(five_qi_t five_qi)
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
