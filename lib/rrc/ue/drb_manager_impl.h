/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/rrc/drb_manager.h"
#include <map>

namespace srsran {

namespace srs_cu_cp {

struct drb_context {
  srsran::drb_id_t           drb_id         = drb_id_t::invalid;
  pdu_session_id_t           pdu_session_id = pdu_session_id_t::min;
  s_nssai_t                  s_nssai        = {};
  bool                       default_drb    = false;
  uint16_t                   five_qi        = 0;
  std::vector<qos_flow_id_t> mapped_qos_flows; // QoS flow IDs of all QoS flows mapped to this DRB

  pdcp_config_t pdcp_cfg;
  sdap_config_t sdap_cfg;
};

/// DRB manager implementation
class drb_manager_impl final : public drb_manager
{
public:
  drb_manager_impl(const drb_manager_cfg& cfg);
  ~drb_manager_impl() = default;

  std::vector<drb_id_t>      calculate_drb_to_add_list(const cu_cp_pdu_session_resource_setup_request& pdu) override;
  pdcp_config_t              get_pdcp_config(const drb_id_t drb_id) override;
  sdap_config_t              get_sdap_config(const drb_id_t drb_id) override;
  std::vector<qos_flow_id_t> get_mapped_qos_flows(const drb_id_t drb_id) override;
  std::vector<qos_flow_id_t> get_mapped_qos_flows(const pdu_session_id_t pdu_session_id) override;
  pdu_session_id_t           get_pdu_session_id(const drb_id_t drb_id) override;
  s_nssai_t                  get_s_nssai(const drb_id_t drb_id) override;
  size_t                     get_nof_drbs() override;

private:
  drb_id_t allocate_drb_id(); // allocates a new DRB ID and returns it

  // returns valid RRC PDCP config for a given FiveQI
  pdcp_config_t set_rrc_pdcp_config(uint16_t five_qi);
  sdap_config_t set_rrc_sdap_config(const drb_context& context);

  const drb_manager_cfg& cfg;
  srslog::basic_logger&  logger;

  std::map<drb_id_t, drb_context> drbs;        // Stores existing DRBs
  std::map<uint16_t, drb_id_t>    five_qi_map; // Maps QoS flow characteristics to existing DRBs
};

} // namespace srs_cu_cp

} // namespace srsran
