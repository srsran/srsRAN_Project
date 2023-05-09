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

#include "srsran/cu_cp/up_resource_manager.h"
#include <map>

namespace srsran {

namespace srs_cu_cp {

/// UP resource manager implementation
class up_resource_manager_impl final : public up_resource_manager
{
public:
  up_resource_manager_impl(const up_resource_manager_cfg& cfg);
  ~up_resource_manager_impl() = default;

  up_config_update              calculate_update(const cu_cp_pdu_session_resource_setup_request& pdu) override;
  bool                          apply_config_update(const up_config_update& config) override;
  pdcp_config                   get_pdcp_config(const drb_id_t drb_id) override;
  sdap_config_t                 get_sdap_config(const drb_id_t drb_id) override;
  std::vector<qos_flow_id_t>    get_mapped_qos_flows(const drb_id_t drb_id) override;
  std::vector<qos_flow_id_t>    get_mapped_qos_flows(const pdu_session_id_t pdu_session_id) override;
  std::vector<drb_id_t>         get_drbs(const pdu_session_id_t pdu_session_id) override;
  pdu_session_id_t              get_pdu_session_id(const drb_id_t drb_id) override;
  s_nssai_t                     get_s_nssai(const drb_id_t drb_id) override;
  size_t                        get_nof_drbs() override;
  size_t                        get_nof_pdu_sessions() override;
  std::vector<pdu_session_id_t> get_pdu_sessions() override;
  bool                          valid_5qi(const five_qi_t five_qi) override;

private:
  up_resource_manager_cfg cfg;

  up_context context; // The currently active state.

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp

} // namespace srsran
