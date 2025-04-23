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
