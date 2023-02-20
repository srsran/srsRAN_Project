/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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

std::vector<drb_id_t> drb_manager_impl::calculate_drb_to_add_list(const cu_cp_pdu_session_resource_setup_request& pdu)
{
  std::vector<drb_id_t> drb_to_add_list;

  // look for existing DRB using the same FiveQI (does it need to be the same PDU session?)
  for (const auto& pdu_session : pdu.pdu_session_res_setup_items) {
    for (const auto& qos_flow : pdu_session.qos_flow_setup_request_items) {
      // TODO: check if FiveQI has configuration

      // check if other DRB with same FiveQI exists
      if (five_qi_map.find(qos_flow.qos_characteristics.five_qi) == five_qi_map.end()) {
        // no existing DRB with same FiveQI, create new DRB
        drb_id_t id = allocate_drb_id();
        if (id == drb_id_t::invalid) {
          logger.error("No more DRBs available");
          return drb_to_add_list;
        }

        drb_context drb_ctx;
        drb_ctx.drb_id         = id;
        drb_ctx.pdu_session_id = pdu_session.pdu_session_id;
        drb_ctx.default_drb    = drbs.empty() ? true : false; // make first DRB the default
        drb_ctx.five_qi        = qos_flow.qos_characteristics.five_qi;
        drb_ctx.pdcp_cfg       = set_rrc_pdcp_config(drb_ctx.five_qi);
        drb_ctx.mapped_qos_flows.push_back(qos_flow.qos_flow_id);
        drb_ctx.sdap_cfg = set_rrc_sdap_config(drb_ctx);

        // add new DRB to list
        drbs.emplace(id, drb_ctx);

        // add FiveQI to map
        five_qi_map.emplace(drb_ctx.five_qi, id);

        // enqueue as DRB to be added
        drb_to_add_list.push_back(id);
      }
    }
  }

  return drb_to_add_list;
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

pdcp_config_t drb_manager_impl::set_rrc_pdcp_config(uint16_t five_qi)
{
  // TODO lookup PDCP config for 5QI in config
  (void)cfg;
  pdcp_config_t pdcp_cfg;
  pdcp_cfg.ciphering_disabled_present = true;

  drb_t drb;
  drb.pdcp_sn_size_dl = pdcp_sn_size::size18bits;
  drb.pdcp_sn_size_ul = pdcp_sn_size::size18bits;
  drb.discard_timer   = 0;

  pdcp_cfg.drb = drb;

  pdcp_cfg.t_reordering = 0;
  return pdcp_cfg;
}

pdu_session_id_t drb_manager_impl::get_pdu_session_id(const drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return pdu_session_id_t::invalid;
  }
  return drbs[drb_id].pdu_session_id;
}

std::vector<qos_flow_id_t> drb_manager_impl::get_mapped_qos_flows(const drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return drbs[drb_id].mapped_qos_flows;
}

std::vector<qos_flow_id_t> drb_manager_impl::get_mapped_qos_flows(const pdu_session_id_t pdu_session_id)
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

pdcp_config_t drb_manager_impl::get_pdcp_config(const drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return drbs[drb_id].pdcp_cfg;
}

sdap_config_t drb_manager_impl::get_sdap_config(const drb_id_t drb_id)
{
  if (drbs.find(drb_id) == drbs.end()) {
    logger.error("DRB {} not found", drb_id);
    return {};
  }
  return drbs[drb_id].sdap_cfg;
}

s_nssai_t drb_manager_impl::get_s_nssai(const drb_id_t drb_id)
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
