/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "up_resource_manager_impl.h"
#include "up_resource_manager_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

up_resource_manager_impl::up_resource_manager_impl(const up_resource_manager_cfg& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("RRC"))
{
}

up_config_update up_resource_manager_impl::calculate_update(const cu_cp_pdu_session_resource_setup_request& pdu)
{
  return srsran::srs_cu_cp::calculate_update(pdu, context, cfg, logger);
}

bool up_resource_manager_impl::apply_config_update(const up_config_update& config)
{
  // add FiveQI to map
  // context.drb_map.five_qi_map.emplace(drb_ctx.five_qi, id);
  return true;
}

pdu_session_id_t up_resource_manager_impl::get_pdu_session_id(drb_id_t drb_id)
{
  if (context.drb_map.find(drb_id) == context.drb_map.end()) {
    logger.error("DRB {} not found", drb_id);
    return pdu_session_id_t::invalid;
  }
  return context.drb_map.at(drb_id).pdu_session_id;
}

std::vector<qos_flow_id_t> up_resource_manager_impl::get_mapped_qos_flows(drb_id_t drb_id)
{
  if (context.drb_map.find(drb_id) == context.drb_map.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return context.drb_map.at(drb_id).mapped_qos_flows;
}

std::vector<qos_flow_id_t> up_resource_manager_impl::get_mapped_qos_flows(pdu_session_id_t pdu_session_id)
{
  std::vector<qos_flow_id_t> mapped_flows;

  for (const auto& drb : context.drb_map) {
    if (drb.second.pdu_session_id == pdu_session_id) {
      for (const auto& flow : drb.second.mapped_qos_flows) {
        mapped_flows.push_back(flow);
      }
    }
  }
  return mapped_flows;
}

std::vector<drb_id_t> up_resource_manager_impl::get_drbs(pdu_session_id_t pdu_session_id)
{
  std::vector<drb_id_t> mapped_drbs;

  for (const auto& drb : context.drb_map) {
    if (drb.second.pdu_session_id == pdu_session_id) {
      mapped_drbs.push_back(drb.first);
    }
  }
  return mapped_drbs;
}

pdcp_config up_resource_manager_impl::get_pdcp_config(drb_id_t drb_id)
{
  if (context.drb_map.find(drb_id) == context.drb_map.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return context.drb_map.at(drb_id).pdcp_cfg;
}

sdap_config_t up_resource_manager_impl::get_sdap_config(drb_id_t drb_id)
{
  if (context.drb_map.find(drb_id) == context.drb_map.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return context.drb_map.at(drb_id).sdap_cfg;
}

s_nssai_t up_resource_manager_impl::get_s_nssai(drb_id_t drb_id)
{
  if (context.drb_map.find(drb_id) == context.drb_map.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return context.drb_map.at(drb_id).s_nssai;
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
    pdu_session_ids.push_back(session.id);
  }

  return pdu_session_ids;
}

bool up_resource_manager_impl::valid_5qi(five_qi_t five_qi)
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
