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

#include "../up_resource_manager/up_resource_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/ue_configuration.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/rrc/rrc_types.h"

namespace srsran {
namespace srs_cu_cp {

bool verify_and_log_cell_group_config(const byte_buffer& packed_cell_group_cfg, const srslog::basic_logger& logger);

/// \brief Fill thew DRB setup mod item for the DU.
/// \param[out] drb_setup_mod_item The DRB setup item to fill.
/// \param[in] response_flow_list The list of QoS flows to be added to the NGAP response.
/// \param[in] psi The PDU session ID.
/// \param[in] drb_id The DRB ID.
/// \param[in] next_drb_config The DRB config to be updated.
/// \param[in] e1ap_drb_item The DRB setup response from CU-UP.
/// \param[in] ngap_qos_flow_setup_items The initial QoS flow setup request from AMF.
/// \param[in] logger The logger.
bool fill_f1ap_drb_setup_mod_item(f1ap_drb_to_setup& drb_setup_mod_item, // Request to setup DRB at DU.
                                  slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow>* response_flow_list,
                                  pdu_session_id_t                                             psi,
                                  drb_id_t                                                     drb_id,
                                  up_drb_context& next_drb_config,                 // DRB config (info is written back).
                                  const e1ap_drb_setup_item_ng_ran& e1ap_drb_item, // Response from CU-UP.
                                  const slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item>&
                                                              ngap_qos_flow_setup_items, // Initial request from AMF.
                                  const srslog::basic_logger& logger);

void fill_e1ap_drb_pdcp_config(e1ap_pdcp_config& e1ap_pdcp_cfg, const pdcp_config& cu_cp_pdcp_cfg);
void fill_e1ap_qos_flow_param_item(e1ap_qos_flow_qos_param_item&      e1ap_qos_item,
                                   const srslog::basic_logger&        logger,
                                   const qos_flow_setup_request_item& request_item);
void fill_e1ap_bearer_context_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item>& e1ap_list,
    const std::vector<f1ap_drb_setupmod>&                                     drb_setup_items,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>&          pdu_sessions_update_list);
void fill_e1ap_pdu_session_res_to_setup_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item>&     pdu_session_res_to_setup_list,
    const srslog::basic_logger&                                                  logger,
    const up_config_update&                                                      next_config,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
    const ue_configuration&                                                      ue_cfg,
    const security_indication_t&                                                 default_security_indication);
void fill_drb_to_setup_list(slotted_id_vector<drb_id_t, e1ap_drb_to_setup_item_ng_ran>&          e1ap_drb_to_setup_list,
                            const slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item>& qos_flow_list,
                            const std::map<drb_id_t, up_drb_context>&                            drb_to_add_list,
                            const srslog::basic_logger&                                          logger);
void fill_drb_to_modify_list(slotted_id_vector<drb_id_t, e1ap_drb_to_modify_item_ng_ran>& e1ap_drb_to_modify_list,
                             const slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_add_or_mod_item>& qos_flow_list,
                             const std::map<drb_id_t, up_drb_context>&                               drb_to_modify_list,
                             const srslog::basic_logger&                                             logger);
void fill_drb_to_remove_list(std::vector<drb_id_t>&       e1ap_drb_to_remove_list,
                             const std::vector<drb_id_t>& drb_to_remove_list);

/// \brief Fill RRC Reconfiguration message content.
/// \param[out] rrc_reconfig_args The RRC Reconfiguration Arguments struct to fill.
/// \param[in] srbs_to_be_setup_mod_list SRBs to be setup (only needed if default DRB is being setup).
/// \param[in] pdu_sessions The PDU sessions to add/mod in the reconfiguration.
/// \param[in] drb_to_remove DRB to remove from the configurations.
/// \param[in] du_to_cu_rrc_info RRC container forwarded from the DU to the CU.
/// \param[in] nas_pdus NAS PDUs to forward to the UE as received by the AMF.
/// \param[in] rrc_meas_cfg Optional measurement config to include in Reconfiguration.
/// \param[in] reestablish_srbs Whether to request SRB reestablishment.
/// \param[in] reestablish_drbs Whether to request DRB reestablishment.
/// \param[in] sib1 Packed SIB1 content for direct delivery to UE (optional).
/// \param[in] logger Logger reference.
/// \return True on success, false otherwise.
bool fill_rrc_reconfig_args(rrc_reconfiguration_procedure_request&                           rrc_reconfig_args,
                            const std::vector<f1ap_srb_to_setup>&                            srbs_to_be_setup_mod_list,
                            const std::map<pdu_session_id_t, up_pdu_session_context_update>& pdu_sessions,
                            const std::vector<drb_id_t>&                                     drb_to_remove,
                            const f1ap_du_to_cu_rrc_info&                                    du_to_cu_rrc_info,
                            const std::vector<byte_buffer>&                                  nas_pdus,
                            const std::optional<rrc_meas_cfg>&                               rrc_meas_cfg,
                            bool                                                             reestablish_srbs,
                            bool                                                             reestablish_drbs,
                            std::optional<uint8_t>                                           ncc,
                            byte_buffer                                                      sib1,
                            std::optional<security::sec_selected_algos>                      selected_algos,
                            const srslog::basic_logger&                                      logger);

/// \brief Processes the response of a UE Context Setup Request.
/// \param[out] bearer_ctxt_mod_request Reference to the resulting Bearer Context Modification Request response.
/// \param[out] drb_setup_mod_list Reference to the successful DRB setup list.
/// \param[out] next_config Const reference to the calculated config update.
/// \param[in] logger Reference to the logger.
/// \return True on success, false otherwise.
bool update_setup_list_with_ue_ctxt_setup_response(e1ap_bearer_context_modification_request& bearer_ctxt_mod_request,
                                                   const std::vector<f1ap_drb_setupmod>&     drb_setup_mod_list,
                                                   const up_config_update&                   next_config,
                                                   const srslog::basic_logger&               logger);

} // namespace srs_cu_cp
} // namespace srsran
