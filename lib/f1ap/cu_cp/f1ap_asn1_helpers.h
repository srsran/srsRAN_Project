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

#include "f1ap_asn1_converters.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ran/lcid.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Convert the UE Context Modification Request from common type to ASN.1.
/// \param[out] asn1_request The ASN.1 struct to store the result.
/// \param[in] msg The common type UE Context Modification Request.
inline void fill_f1ap_ue_context_modification_request(asn1::f1ap::ue_context_mod_request_s&        asn1_request,
                                                      const cu_cp_ue_context_modification_request& msg)
{
  // sp cell id
  if (msg.sp_cell_id.has_value()) {
    asn1_request->sp_cell_id_present = true;
    asn1_request->sp_cell_id.value.nr_cell_id.from_number(msg.sp_cell_id.value().nci);
    asn1_request->sp_cell_id.value.plmn_id.from_string(msg.sp_cell_id.value().plmn_hex);
  }

  // serv cell idx
  if (msg.serv_cell_idx.has_value()) {
    asn1_request->serv_cell_idx_present = true;
    asn1_request->serv_cell_idx.value   = msg.serv_cell_idx.value();
  }

  // sp cell ul cfg
  if (msg.sp_cell_ul_cfg.has_value()) {
    asn1_request->sp_cell_ul_cfg_present = true;
    asn1::string_to_enum(asn1_request->sp_cell_ul_cfg.value, msg.sp_cell_ul_cfg.value());
  }

  // drx cycle
  if (msg.drx_cycle.has_value()) {
    asn1_request->drx_cycle_present = true;
    asn1::number_to_enum(asn1_request->drx_cycle.value.long_drx_cycle_len, msg.drx_cycle.value().long_drx_cycle_len);

    if (msg.drx_cycle.value().short_drx_cycle_len.has_value()) {
      asn1_request->drx_cycle.value.short_drx_cycle_len_present = true;
      asn1::number_to_enum(asn1_request->drx_cycle.value.short_drx_cycle_len,
                           msg.drx_cycle.value().short_drx_cycle_len.value());
    }

    if (msg.drx_cycle.value().short_drx_cycle_timer.has_value()) {
      asn1_request->drx_cycle.value.short_drx_cycle_timer_present = true;
      asn1_request->drx_cycle.value.short_drx_cycle_timer         = msg.drx_cycle.value().short_drx_cycle_timer.value();
    }
  }

  // cu to du rrc info
  if (msg.cu_to_du_rrc_info.has_value()) {
    asn1_request->cu_to_du_rrc_info_present           = true;
    asn1_request->cu_to_du_rrc_info.value.cg_cfg_info = msg.cu_to_du_rrc_info.value().cg_cfg_info.copy();
    asn1_request->cu_to_du_rrc_info.value.ue_cap_rat_container_list =
        msg.cu_to_du_rrc_info.value().ue_cap_rat_container_list.copy();
    asn1_request->cu_to_du_rrc_info.value.meas_cfg = msg.cu_to_du_rrc_info.value().meas_cfg.copy();
  }

  // tx action ind
  if (msg.tx_action_ind.has_value()) {
    asn1_request->tx_action_ind_present = true;
    asn1::string_to_enum(asn1_request->tx_action_ind.value, msg.tx_action_ind.value());
  }

  // res coordination transfer container
  if (!msg.res_coordination_transfer_container.empty()) {
    asn1_request->res_coordination_transfer_container_present = true;
    asn1_request->res_coordination_transfer_container.value   = msg.res_coordination_transfer_container.copy();
  }

  // rrc recfg complete ind
  if (msg.rrc_recfg_complete_ind.has_value()) {
    asn1_request->rrc_recfg_complete_ind_present = true;
    asn1::string_to_enum(asn1_request->rrc_recfg_complete_ind.value, msg.rrc_recfg_complete_ind.value());
  }

  // rrc container
  if (!msg.rrc_container.empty()) {
    asn1_request->rrc_container_present = true;
    asn1_request->rrc_container.value   = msg.rrc_container.copy();
  }

  // scell to be setup mod list
  if (!msg.scell_to_be_setup_mod_list.empty()) {
    asn1_request->scell_to_be_setup_mod_list_present = true;
    for (const auto& scell_to_be_setup_mod_item : msg.scell_to_be_setup_mod_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_mod_item_ies_o>
          asn1_scell_to_be_setup_mod_item_container;
      asn1_scell_to_be_setup_mod_item_container.load_info_obj(ASN1_F1AP_ID_SCELL_TO_BE_SETUP_MOD_ITEM);

      auto& asn1_scell_to_be_setup_mod_item =
          asn1_scell_to_be_setup_mod_item_container.value().scell_to_be_setup_mod_item();
      // scell id
      asn1_scell_to_be_setup_mod_item.scell_id.nr_cell_id.from_number(scell_to_be_setup_mod_item.scell_id.nci);
      asn1_scell_to_be_setup_mod_item.scell_id.plmn_id.from_string(scell_to_be_setup_mod_item.scell_id.plmn_hex);

      // scell idx
      asn1_scell_to_be_setup_mod_item.scell_idx = scell_to_be_setup_mod_item.scell_idx;

      // scell ul cfg
      if (scell_to_be_setup_mod_item.scell_ul_cfg.has_value()) {
        asn1_scell_to_be_setup_mod_item.scell_ul_cfg_present = true;
        asn1::string_to_enum(asn1_scell_to_be_setup_mod_item.scell_ul_cfg,
                             scell_to_be_setup_mod_item.scell_ul_cfg.value());
      }

      asn1_request->scell_to_be_setup_mod_list.value.push_back(asn1_scell_to_be_setup_mod_item_container);
    }
  }

  // scell to be remd list
  if (!msg.scell_to_be_remd_list.empty()) {
    asn1_request->scell_to_be_remd_list_present = true;
    for (const auto& scell_to_be_remd_item : msg.scell_to_be_remd_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_remd_item_ies_o>
          asn1_scell_to_be_remd_item_container;
      asn1_scell_to_be_remd_item_container.load_info_obj(ASN1_F1AP_ID_SCELL_TO_BE_REMD_ITEM);

      auto& asn1_scell_to_be_remd_item = asn1_scell_to_be_remd_item_container.value().scell_to_be_remd_item();
      asn1_scell_to_be_remd_item.scell_id.nr_cell_id.from_number(scell_to_be_remd_item.scell_id.nci);
      asn1_scell_to_be_remd_item.scell_id.plmn_id.from_string(scell_to_be_remd_item.scell_id.plmn_hex);

      asn1_request->scell_to_be_remd_list.value.push_back(asn1_scell_to_be_remd_item_container);
    }
  }

  // srbs to be setup mod list
  if (!msg.srbs_to_be_setup_mod_list.empty()) {
    asn1_request->srbs_to_be_setup_mod_list_present = true;
    for (const auto& srb_to_be_setup_mod_item : msg.srbs_to_be_setup_mod_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_mod_item_ies_o>
          asn1_srb_to_be_setup_mod_item_container;
      asn1_srb_to_be_setup_mod_item_container.load_info_obj(ASN1_F1AP_ID_SRBS_TO_BE_SETUP_MOD_ITEM);

      auto& asn1_srb_to_be_setup_mod_item = asn1_srb_to_be_setup_mod_item_container.value().srbs_to_be_setup_mod_item();
      asn1_srb_to_be_setup_mod_item.srb_id = srb_id_to_uint(srb_to_be_setup_mod_item.srb_id);
      if (srb_to_be_setup_mod_item.dupl_ind.has_value()) {
        asn1_srb_to_be_setup_mod_item.dupl_ind_present = true;
        asn1::string_to_enum(asn1_srb_to_be_setup_mod_item.dupl_ind, srb_to_be_setup_mod_item.dupl_ind.value());
      }

      asn1_request->srbs_to_be_setup_mod_list.value.push_back(asn1_srb_to_be_setup_mod_item_container);
    }
  }

  // drbs to be setup mod list
  if (!msg.drbs_to_be_setup_mod_list.empty()) {
    asn1_request->drbs_to_be_setup_mod_list_present = true;
    for (const auto& drb_to_be_setup_mod_item : msg.drbs_to_be_setup_mod_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o>
          asn1_drb_to_be_setup_mod_item_container;
      asn1_drb_to_be_setup_mod_item_container.load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_SETUP_MOD_ITEM);

      auto& asn1_drb_to_be_setup_mod_item = asn1_drb_to_be_setup_mod_item_container.value().drbs_to_be_setup_mod_item();
      asn1_drb_to_be_setup_mod_item.drb_id = drb_id_to_uint(drb_to_be_setup_mod_item.drb_id);

      // qos info
      asn1_drb_to_be_setup_mod_item.qos_info = qos_info_to_f1ap_asn1(drb_to_be_setup_mod_item.qos_info);

      // ul up tnl info to be setup list
      for (const auto& ul_up_tnl_info_item : drb_to_be_setup_mod_item.ul_up_tnl_info_to_be_setup_list) {
        asn1::f1ap::ul_up_tnl_info_to_be_setup_item_s item;
        up_transport_layer_info_to_asn1(item.ul_up_tnl_info, ul_up_tnl_info_item);
        asn1_drb_to_be_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(item);
      }

      // rlc mode
      asn1_drb_to_be_setup_mod_item.rlc_mode = rlc_mode_to_f1ap_asn1(drb_to_be_setup_mod_item.rlc_mod);

      // ul cfg
      if (drb_to_be_setup_mod_item.ul_cfg.has_value()) {
        asn1_drb_to_be_setup_mod_item.ul_cfg_present = true;
        asn1::string_to_enum(asn1_drb_to_be_setup_mod_item.ul_cfg.ul_ue_cfg,
                             drb_to_be_setup_mod_item.ul_cfg.value().ul_ue_cfg);
      }

      // dupl activation
      if (drb_to_be_setup_mod_item.dupl_activation.has_value()) {
        asn1_drb_to_be_setup_mod_item.dupl_activation_present = true;
        asn1::string_to_enum(asn1_drb_to_be_setup_mod_item.dupl_activation,
                             drb_to_be_setup_mod_item.dupl_activation.value());
      }

      asn1_request->drbs_to_be_setup_mod_list.value.push_back(asn1_drb_to_be_setup_mod_item_container);
    }
  }

  // drbs to be modified list
  if (!msg.drbs_to_be_modified_list.empty()) {
    asn1_request->drbs_to_be_modified_list_present = true;

    for (const auto& drb_to_be_modified_item : msg.drbs_to_be_modified_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_modified_item_ies_o>
          asn1_drb_to_be_modified_item_container;
      asn1_drb_to_be_modified_item_container.load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_MODIFIED_ITEM);

      auto& asn1_drb_to_be_modified_item  = asn1_drb_to_be_modified_item_container.value().drbs_to_be_modified_item();
      asn1_drb_to_be_modified_item.drb_id = drb_id_to_uint(drb_to_be_modified_item.drb_id);

      // qos info
      if (drb_to_be_modified_item.qos_info.has_value()) {
        asn1_drb_to_be_modified_item.qos_info_present = true;
        asn1_drb_to_be_modified_item.qos_info         = qos_info_to_f1ap_asn1(drb_to_be_modified_item.qos_info.value());
      }

      // ul up tnl info to be setup list
      for (const auto& ul_up_tnl_info_item : drb_to_be_modified_item.ul_up_tnl_info_to_be_setup_list) {
        asn1::f1ap::ul_up_tnl_info_to_be_setup_item_s item;
        up_transport_layer_info_to_asn1(item.ul_up_tnl_info, ul_up_tnl_info_item);
        asn1_drb_to_be_modified_item.ul_up_tnl_info_to_be_setup_list.push_back(item);
      }

      // ul cfg
      if (drb_to_be_modified_item.ul_cfg.has_value()) {
        asn1_drb_to_be_modified_item.ul_cfg_present = true;
        asn1::string_to_enum(asn1_drb_to_be_modified_item.ul_cfg.ul_ue_cfg,
                             drb_to_be_modified_item.ul_cfg.value().ul_ue_cfg);
      }

      asn1_request->drbs_to_be_modified_list.value.push_back(asn1_drb_to_be_modified_item_container);
    }
  }

  // srbs to be released list
  if (!msg.srbs_to_be_released_list.empty()) {
    asn1_request->srbs_to_be_released_list_present = true;
    for (const auto& srb_id : msg.srbs_to_be_released_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_released_item_ies_o>
          asn1_srb_to_be_released_item_container;
      asn1_srb_to_be_released_item_container.load_info_obj(ASN1_F1AP_ID_SRBS_TO_BE_RELEASED_ITEM);

      auto& asn1_srb_to_be_released_item  = asn1_srb_to_be_released_item_container.value().srbs_to_be_released_item();
      asn1_srb_to_be_released_item.srb_id = srb_id_to_uint(srb_id);

      asn1_request->srbs_to_be_released_list.value.push_back(asn1_srb_to_be_released_item_container);
    }
  }

  // drbs to be released list
  if (!msg.drbs_to_be_released_list.empty()) {
    asn1_request->drbs_to_be_released_list_present = true;
    for (const auto& drb_id : msg.drbs_to_be_released_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_released_item_ies_o>
          asn1_drb_to_be_released_item_container;
      asn1_drb_to_be_released_item_container.load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_RELEASED_ITEM);

      auto& asn1_drb_to_be_released_item  = asn1_drb_to_be_released_item_container.value().drbs_to_be_released_item();
      asn1_drb_to_be_released_item.drb_id = drb_id_to_uint(drb_id);

      asn1_request->drbs_to_be_released_list.value.push_back(asn1_drb_to_be_released_item_container);
    }
  }

  // inactivity monitoring request
  if (msg.inactivity_monitoring_request.has_value()) {
    asn1_request->inactivity_monitoring_request_present = true;
    asn1::string_to_enum(asn1_request->inactivity_monitoring_request.value, msg.inactivity_monitoring_request.value());
  }

  // rat freq prio info
  if (msg.rat_freq_prio_info.has_value()) {
    asn1_request->rat_freq_prio_info_present = true;

    if (msg.rat_freq_prio_info.value().type == "nGRAN") {
      asn1_request->rat_freq_prio_info.value.set_ngran();
      asn1_request->rat_freq_prio_info.value.ngran() = msg.rat_freq_prio_info.value().rat_freq_prio_info;
    } else if (msg.rat_freq_prio_info.value().type == "eNDC") {
      asn1_request->rat_freq_prio_info.value.set_endc();
      asn1_request->rat_freq_prio_info.value.endc() = msg.rat_freq_prio_info.value().rat_freq_prio_info;
    } else {
      asn1_request->rat_freq_prio_info_present = false;
    }
  }

  // drx cfg ind
  if (msg.drx_cfg_ind.has_value()) {
    asn1_request->drx_cfg_ind_present = true;
    asn1::string_to_enum(asn1_request->drx_cfg_ind.value, msg.drx_cfg_ind.value());
  }

  // rlc fail ind
  if (msg.rlc_fail_ind.has_value()) {
    asn1_request->rlc_fail_ind_present              = true;
    asn1_request->rlc_fail_ind.value.assocated_lcid = msg.rlc_fail_ind.value().assocated_lcid;
  }

  // ul tx direct current list info
  if (!msg.ul_tx_direct_current_list_info.empty()) {
    asn1_request->ul_tx_direct_current_list_info_present = true;
    asn1_request->ul_tx_direct_current_list_info.value   = msg.ul_tx_direct_current_list_info.copy();
  }

  // gnb du cfg query
  if (msg.gnb_du_cfg_query.has_value()) {
    asn1_request->gnb_du_cfg_query_present = true;
    asn1::string_to_enum(asn1_request->gnb_du_cfg_query.value, msg.gnb_du_cfg_query.value());
  }

  // gnb du ue ambr ul
  if (msg.gnb_du_ue_ambr_ul.has_value()) {
    asn1_request->gnb_du_ue_ambr_ul_present = true;
    asn1_request->gnb_du_ue_ambr_ul.value   = msg.gnb_du_ue_ambr_ul.value();
  }

  // execute dupl
  if (msg.execute_dupl.has_value()) {
    asn1_request->execute_dupl_present = true;
    asn1::string_to_enum(asn1_request->execute_dupl.value, msg.execute_dupl.value());
  }

  // rrc delivery status request
  if (msg.rrc_delivery_status_request.has_value()) {
    asn1_request->rrc_delivery_status_request_present = true;
    asn1::string_to_enum(asn1_request->rrc_delivery_status_request.value, msg.rrc_delivery_status_request.value());
  }

  // res coordination transfer info
  if (msg.res_coordination_transfer_info.has_value()) {
    asn1_request->res_coordination_transfer_info_present                                        = true;
    asn1_request->res_coordination_transfer_info.value.res_coordination_eutra_cell_info_present = false;
    asn1_request->res_coordination_transfer_info.value.m_enb_cell_id.from_number(
        msg.res_coordination_transfer_info.value().m_enb_cell_id);
  }

  // serving cell mo
  if (msg.serving_cell_mo.has_value()) {
    asn1_request->serving_cell_mo_present = true;
    asn1_request->serving_cell_mo.value   = msg.serving_cell_mo.value();
  }

  // need for gap
  if (msg.need_for_gap.has_value()) {
    asn1_request->needfor_gap_present = true;
    asn1::string_to_enum(asn1_request->needfor_gap.value, msg.need_for_gap.value());
  }

  // full cfg
  if (msg.full_cfg.has_value()) {
    asn1_request->full_cfg_present = true;
    asn1::string_to_enum(asn1_request->full_cfg.value, msg.full_cfg.value());
  }
}

/// \brief Convert the UE Context Modification Response from ASN.1 to common type.
/// \param[out] res The common type struct to store the result.
/// \param[in] asn1_response The ASN.1 type UE Context Modification Response.
inline void fill_f1ap_ue_context_modification_response_message(cu_cp_ue_context_modification_response&  res,
                                                               const asn1::f1ap::ue_context_mod_resp_s& asn1_response)
{
  res.success = true;

  // DUtoCURRCInformation
  if (asn1_response->du_to_cu_rrc_info_present) {
    res.du_to_cu_rrc_info.cell_group_cfg      = asn1_response->du_to_cu_rrc_info->cell_group_cfg.copy();
    res.du_to_cu_rrc_info.meas_gap_cfg        = asn1_response->du_to_cu_rrc_info->meas_gap_cfg.copy();
    res.du_to_cu_rrc_info.requested_p_max_fr1 = asn1_response->du_to_cu_rrc_info->requested_p_max_fr1.copy();
  }

  // Add DRBs setup mod list
  if (asn1_response->drbs_setup_mod_list_present) {
    for (auto asn1_drb_setup_mod_list_item : asn1_response->drbs_setup_mod_list.value) {
      auto& asn1_drb_mod_item = asn1_drb_setup_mod_list_item.value().drbs_setup_mod_item();

      cu_cp_drbs_setup_modified_item drb_setup_mod_item;
      drb_setup_mod_item.drb_id = uint_to_drb_id(asn1_drb_mod_item.drb_id);

      // Add DL UP TNL to be setup list
      for (auto asn1_dl_up_tnl_info_to_be_setup_item : asn1_drb_mod_item.dl_up_tnl_info_to_be_setup_list) {
        cu_cp_dl_up_tnl_info_to_be_setup_item dl_up_tnl_info_to_be_setup_item;
        dl_up_tnl_info_to_be_setup_item.dl_up_tnl_info =
            asn1_to_up_transport_layer_info(asn1_dl_up_tnl_info_to_be_setup_item.dl_up_tnl_info);
        drb_setup_mod_item.dl_up_tnl_info_to_be_setup_list.push_back(dl_up_tnl_info_to_be_setup_item);
      }

      if (asn1_drb_mod_item.lcid_present) {
        drb_setup_mod_item.lcid = uint_to_lcid(asn1_drb_mod_item.lcid);
      }

      res.drbs_setup_mod_list.emplace(drb_setup_mod_item.drb_id, drb_setup_mod_item);
    }
  }

  // Add DRBs modified list
  if (asn1_response->drbs_modified_list_present) {
    for (auto asn1_drbs_modified_list_item : asn1_response->drbs_modified_list.value) {
      auto& asn1_drb_mod_item = asn1_drbs_modified_list_item.value().drbs_modified_item();

      cu_cp_drbs_setup_modified_item drb_setup_mod_item;
      drb_setup_mod_item.drb_id = uint_to_drb_id(asn1_drb_mod_item.drb_id);

      // Add DL UP TNL to be setup list
      for (auto asn1_dl_up_tnl_info_to_be_setup_item : asn1_drb_mod_item.dl_up_tnl_info_to_be_setup_list) {
        cu_cp_dl_up_tnl_info_to_be_setup_item dl_up_tnl_info_to_be_setup_item;
        dl_up_tnl_info_to_be_setup_item.dl_up_tnl_info =
            asn1_to_up_transport_layer_info(asn1_dl_up_tnl_info_to_be_setup_item.dl_up_tnl_info);
        drb_setup_mod_item.dl_up_tnl_info_to_be_setup_list.push_back(dl_up_tnl_info_to_be_setup_item);
      }

      if (asn1_drb_mod_item.lcid_present) {
        drb_setup_mod_item.lcid = uint_to_lcid(asn1_drb_mod_item.lcid);
      }

      res.drbs_modified_list.emplace(drb_setup_mod_item.drb_id, drb_setup_mod_item);
    }
  }

  // Add SRBs failed to be setup mod list
  if (asn1_response->srbs_failed_to_be_setup_mod_list_present) {
    for (auto asn1_srbs_failed_setup_mod_list_item : asn1_response->srbs_failed_to_be_setup_mod_list.value) {
      auto& asn1_srb_failed_item = asn1_srbs_failed_setup_mod_list_item.value().srbs_failed_to_be_setup_mod_item();

      cu_cp_srbs_failed_to_be_setup_mod_item srb_failed_item;
      srb_failed_item.srb_id = int_to_srb_id(asn1_srb_failed_item.srb_id);
      if (asn1_srb_failed_item.cause_present) {
        srb_failed_item.cause = f1ap_cause_to_cause(asn1_srb_failed_item.cause);
      }
      res.srbs_failed_to_be_setup_mod_list.emplace(srb_failed_item.srb_id, srb_failed_item);
    }
  }

  // Add DRBs failed to be setup mod list
  if (asn1_response->drbs_failed_to_be_setup_mod_list_present) {
    for (auto asn1_drbs_failed_setup_mod_list_item : asn1_response->drbs_failed_to_be_setup_mod_list.value) {
      auto& asn1_drb_failed_item = asn1_drbs_failed_setup_mod_list_item.value().drbs_failed_to_be_setup_mod_item();

      cu_cp_drbs_failed_to_be_setup_modified_item drb_failed_item;
      drb_failed_item.drb_id = uint_to_drb_id(asn1_drb_failed_item.drb_id);
      if (asn1_drb_failed_item.cause_present) {
        drb_failed_item.cause = f1ap_cause_to_cause(asn1_drb_failed_item.cause);
      }
      res.drbs_failed_to_be_setup_mod_list.emplace(drb_failed_item.drb_id, drb_failed_item);
    }
  }

  // Add SCell failed to be setup mod list
  if (asn1_response->scell_failedto_setup_mod_list_present) {
    for (auto asn1_scell_failed_setup_mod_list_item : asn1_response->scell_failedto_setup_mod_list.value) {
      auto& asn1_scell_failed_item = asn1_scell_failed_setup_mod_list_item.value().scell_failedto_setup_mod_item();

      cu_cp_scell_failed_to_setup_mod_item scell_failed_item;
      scell_failed_item.scell_id = f1ap_nrcgi_to_nr_cell_identity(asn1_scell_failed_item.scell_id);
      if (asn1_scell_failed_item.cause_present) {
        scell_failed_item.cause = f1ap_cause_to_cause(asn1_scell_failed_item.cause);
      }
      res.scell_failed_to_setup_mod_list.push_back(scell_failed_item);
    }
  }

  // Add DRBs failed to be modified list
  if (asn1_response->drbs_failed_to_be_modified_list_present) {
    for (auto asn1_drbs_failed_modified_list_item : asn1_response->drbs_failed_to_be_modified_list.value) {
      auto& asn1_drb_failed_item = asn1_drbs_failed_modified_list_item.value().drbs_failed_to_be_modified_item();

      cu_cp_drbs_failed_to_be_setup_modified_item drb_failed_item;
      drb_failed_item.drb_id = uint_to_drb_id(asn1_drb_failed_item.drb_id);
      if (asn1_drb_failed_item.cause_present) {
        drb_failed_item.cause = f1ap_cause_to_cause(asn1_drb_failed_item.cause);
      }
      res.drbs_failed_to_be_modified_list.emplace(drb_failed_item.drb_id, drb_failed_item);
    }
  }

  // Add inactivity monitoring response
  if (asn1_response->inactivity_monitoring_resp_present) {
    res.inactivity_monitoring_resp = asn1_response->inactivity_monitoring_resp.value.to_string();
  }

  // Add C-RNTI
  if (asn1_response->c_rnti_present) {
    res.c_rnti = to_rnti(asn1_response->c_rnti.value);
  }

  // Add associated SCell list
  if (asn1_response->associated_scell_list_present) {
    for (auto asn1_associated_scell_list_item : asn1_response->associated_scell_list.value) {
      auto& asn1_associated_scell_item = asn1_associated_scell_list_item.value().associated_scell_item();

      cu_cp_associated_scell_item associated_scell_item;
      associated_scell_item.scell_id = f1ap_nrcgi_to_nr_cell_identity(asn1_associated_scell_item.scell_id);

      res.associated_scell_list.push_back(associated_scell_item);
    }
  }

  // Add SRBs setup mod list
  if (asn1_response->srbs_setup_mod_list_present) {
    for (auto asn1_srbs_setup_mod_list_item : asn1_response->srbs_setup_mod_list.value) {
      auto& asn1_srbs_setup_mod_item = asn1_srbs_setup_mod_list_item.value().srbs_setup_mod_item();

      cu_cp_srbs_setup_modified_item srbs_setup_mod_item;
      srbs_setup_mod_item.srb_id = int_to_srb_id(asn1_srbs_setup_mod_item.srb_id);
      srbs_setup_mod_item.lcid   = uint_to_lcid(asn1_srbs_setup_mod_item.lcid);

      res.srbs_setup_mod_list.emplace(srbs_setup_mod_item.srb_id, srbs_setup_mod_item);
    }
  }

  // Add SRBs modified list
  if (asn1_response->srbs_modified_list_present) {
    for (auto asn1_srbs_modified_list_item : asn1_response->srbs_modified_list.value) {
      auto& asn1_srbs_modified_item = asn1_srbs_modified_list_item.value().srbs_modified_item();

      cu_cp_srbs_setup_modified_item srbs_modified_item;
      srbs_modified_item.srb_id = int_to_srb_id(asn1_srbs_modified_item.srb_id);
      srbs_modified_item.lcid   = uint_to_lcid(asn1_srbs_modified_item.lcid);

      res.srbs_modified_list.emplace(srbs_modified_item.srb_id, srbs_modified_item);
    }
  }

  // Add full configuration
  if (asn1_response->full_cfg_present) {
    res.full_cfg = asn1_response->full_cfg.value.to_string();
  }
}

/// \brief Convert the UE Context Modification Failure from ASN.1 to common type.
/// \param[out] res The common type struct to store the result.
/// \param[in] asn1_fail The ASN.1 type UE Context Modification Failure.
inline void fill_f1ap_ue_context_modification_response_message(cu_cp_ue_context_modification_response&  res,
                                                               const asn1::f1ap::ue_context_mod_fail_s& asn1_fail)
{
  res.success = false;
  res.cause   = f1ap_cause_to_cause(asn1_fail->cause.value);
  if (asn1_fail->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }
}

/// \brief Convert the \c cu_cp_paging_message type to ASN.1.
/// \param[out] asn1_paging The ASN.1 type struct to store the result.
/// \param[in] paging The common type Paging message.
inline void fill_asn1_paging_message(asn1::f1ap::paging_s& asn1_paging, const cu_cp_paging_message& paging)
{
  // Add ue id idx value
  // UE Identity Index value is defined as: UE_ID 5G-S-TMSI mod 1024  (see TS 38.304 section 7.1)
  asn1_paging->ue_id_idx_value.value.set_idx_len10().from_number(paging.ue_paging_id.five_g_tmsi % 1024);

  // Add paging id
  asn1_paging->paging_id.value.set_cn_ue_paging_id().set_five_g_s_tmsi().from_number(paging.ue_paging_id.five_g_tmsi);

  // Add paging drx
  if (paging.paging_drx.has_value()) {
    asn1_paging->paging_drx_present = true;
    asn1::number_to_enum(asn1_paging->paging_drx.value, paging.paging_drx.value());
  }

  // Add paging prio
  if (paging.paging_prio.has_value()) {
    asn1_paging->paging_prio_present = true;
    asn1::number_to_enum(asn1_paging->paging_prio.value, paging.paging_prio.value());
  }

  // Add paging cell list
  for (const auto& cell_item : paging.assist_data_for_paging.value()
                                   .assist_data_for_recommended_cells.value()
                                   .recommended_cells_for_paging.recommended_cell_list) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::paging_cell_item_ies_o> asn1_paging_cell_item_container;
    auto& asn1_paging_cell_item = asn1_paging_cell_item_container->paging_cell_item();

    asn1_paging_cell_item.nr_cgi.nr_cell_id.from_number(cell_item.ngran_cgi.nci);
    asn1_paging_cell_item.nr_cgi.plmn_id.from_string(cell_item.ngran_cgi.plmn_hex);

    asn1_paging->paging_cell_list.value.push_back(asn1_paging_cell_item_container);
  }

  // Add paging origin
  if (paging.paging_origin.has_value()) {
    asn1_paging->paging_origin_present = true;
    asn1::string_to_enum(asn1_paging->paging_origin.value, paging.paging_origin.value());
  }
}

} // namespace srs_cu_cp
} // namespace srsran
