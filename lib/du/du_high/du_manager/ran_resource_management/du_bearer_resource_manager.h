/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_ue_resource_config.h"

namespace srsran {
namespace srs_du {

struct du_ue_bearer_resource_update_request {
  span<const srb_id_t>           srbs_to_setup;
  span<const f1ap_drb_to_setup>  drbs_to_setup;
  span<const f1ap_drb_to_modify> drbs_to_mod;
  span<const drb_id_t>           drbs_to_rem;
};

struct du_ue_bearer_resource_update_response {
  std::vector<drb_id_t> drbs_failed_to_setup;
  std::vector<drb_id_t> drbs_failed_to_mod;
};

class du_bearer_resource_manager
{
public:
  du_bearer_resource_manager(const std::map<srb_id_t, du_srb_config>&  srbs,
                             const std::map<five_qi_t, du_qos_config>& qos,
                             srslog::basic_logger&                     logger);

  /// \brief Allocate bearer resources for a given UE. The resources are stored in the UE's DU UE resource config.
  /// \return true if allocation was successful.
  du_ue_bearer_resource_update_response update(du_ue_resource_config&                      ue_cfg,
                                               const du_ue_bearer_resource_update_request& request,
                                               const du_ue_resource_config*                reestablished_context);

private:
  void                  setup_srbs(du_ue_resource_config& ue_cfg, const du_ue_bearer_resource_update_request& request);
  std::vector<drb_id_t> setup_drbs(du_ue_resource_config& ue_cfg, const du_ue_bearer_resource_update_request& request);
  std::vector<drb_id_t> modify_drbs(du_ue_resource_config& ue_cfg, const du_ue_bearer_resource_update_request& request);
  void                  rem_drbs(du_ue_resource_config& ue_cfg, const du_ue_bearer_resource_update_request& request);

  const std::map<srb_id_t, du_srb_config>&  srb_config;
  const std::map<five_qi_t, du_qos_config>& qos_config;
  srslog::basic_logger&                     logger;
};

} // namespace srs_du
} // namespace srsran