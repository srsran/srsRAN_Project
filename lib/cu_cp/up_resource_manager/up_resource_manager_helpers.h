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

#pragma once

#include "srsran/cu_cp/cu_cp_types.h" // for SDAP config
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/pdcp/pdcp_config.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Perform sanity check on incoming resource setup/modifcation/release requests.
bool is_valid(const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
              const up_context&                                                            context,
              const up_resource_manager_cfg&                                               cfg,
              const srslog::basic_logger&                                                  logger);
bool is_valid(const cu_cp_pdu_session_resource_modify_request& pdu,
              const up_context&                                context,
              const up_resource_manager_cfg&                   cfg,
              const srslog::basic_logger&                      logger);
bool is_valid(const cu_cp_pdu_session_resource_release_command& pdu,
              const up_context&                                 context,
              const up_resource_manager_cfg&                    cfg,
              const srslog::basic_logger&                       logger);

/// \brief Validates that a given FiveQI has a valid PDCP and SDAP config.
bool is_valid(five_qi_t five_qi, const up_resource_manager_cfg& cfg, const srslog::basic_logger& logger);

/// @brief Perform sanity check on requested QoS flow configuration.
five_qi_t get_five_qi(const cu_cp_qos_flow_add_or_mod_item& qos_flow,
                      const up_resource_manager_cfg&        cfg,
                      const srslog::basic_logger&           logger);

/// \brief Functions to calculate a configuration update based on the active config and an incoming PDU session
/// setup/modfication/release request/command. No configuration state is altered.
/// \param pdu The PDU session resource setup/modification request or release command.
/// \param  contest The currently active UP resource allocation.
/// \return The config update struct
up_config_update
calculate_update(const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
                 const up_context&                                                            context,
                 const up_resource_manager_cfg&                                               cfg,
                 const srslog::basic_logger&                                                  logger);
up_config_update calculate_update(const cu_cp_pdu_session_resource_modify_request& pdu,
                                  const up_context&                                context,
                                  const up_resource_manager_cfg&                   cfg,
                                  const srslog::basic_logger&                      logger);
up_config_update calculate_update(const cu_cp_pdu_session_resource_release_command& pdu,
                                  const up_context&                                 context,
                                  const up_resource_manager_cfg&                    cfg,
                                  const srslog::basic_logger&                       logger);

// \brief Allocates a new DRB ID and returns it.
drb_id_t allocate_drb_id(const up_pdu_session_context_update& new_session_context,
                         const up_context&                    context,
                         const up_config_update&              config_update,
                         const srslog::basic_logger&          logger);

// \brief Returns valid RRC PDCP config for a given FiveQI
pdcp_config   set_rrc_pdcp_config(five_qi_t five_qi, const up_resource_manager_cfg& cfg);
sdap_config_t set_rrc_sdap_config(const up_drb_context& context);

} // namespace srs_cu_cp
} // namespace srsran
