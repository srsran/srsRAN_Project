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

#include "up_resource_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h" // for SDAP config
#include "srsran/pdcp/pdcp_config.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Perform sanity check on incoming resource setup/modification/release requests.
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
                         uint8_t                              max_nof_drbs_per_ue,
                         const srslog::basic_logger&          logger);

// \brief Returns valid RRC PDCP config for a given FiveQI
pdcp_config   set_rrc_pdcp_config(five_qi_t five_qi, const up_resource_manager_cfg& cfg);
sdap_config_t set_rrc_sdap_config(const up_drb_context& context);

} // namespace srs_cu_cp
} // namespace srsran
