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

#include "../common/asn1_helpers.h"
#include "f1ap_asn1_converters.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/rnti.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Convert the F1 Setup Request from ASN.1 to common type.
/// \param[out] request The common type struct to store the result.
/// \param[in] asn1_request The ASN.1 type F1 Setup Request.
inline void fill_f1_setup_request(f1ap_f1_setup_request& request, const asn1::f1ap::f1_setup_request_s& asn1_request)
{
  // GNB DU ID
  request.gnb_du_id = asn1_request->gnb_du_id;

  // GNB DU name
  if (asn1_request->gnb_du_name_present) {
    request.gnb_du_name = asn1_request->gnb_du_name.to_string();
  }

  // GNB DU served cells list
  if (asn1_request->gnb_du_served_cells_list_present) {
    for (const auto& asn1_served_cell_item : asn1_request->gnb_du_served_cells_list) {
      auto& asn1_served_cell = asn1_served_cell_item.value().gnb_du_served_cells_item();

      cu_cp_du_served_cells_item served_cell;

      // served cell info
      // NR CGI
      served_cell.served_cell_info.nr_cgi = cgi_from_asn1(asn1_served_cell.served_cell_info.nr_cgi);

      // NR PCI
      served_cell.served_cell_info.nr_pci = asn1_served_cell.served_cell_info.nr_pci;

      // 5GS TAC
      if (asn1_served_cell.served_cell_info.five_gs_tac_present) {
        served_cell.served_cell_info.five_gs_tac = asn1_served_cell.served_cell_info.five_gs_tac.to_number();
      }

      // cfg EPS TAC
      if (asn1_served_cell.served_cell_info.cfg_eps_tac_present) {
        served_cell.served_cell_info.cfg_eps_tac = asn1_served_cell.served_cell_info.cfg_eps_tac.to_number();
      }

      // served PLMNs
      for (const auto& asn1_plmn : asn1_served_cell.served_cell_info.served_plmns) {
        served_cell.served_cell_info.served_plmns.push_back(asn1_plmn.plmn_id.to_string());
      }

      // NR mode info
      served_cell.served_cell_info.nr_mode_info =
          f1ap_asn1_to_nr_mode_info(asn1_served_cell.served_cell_info.nr_mode_info);

      // meas timing cfg
      served_cell.served_cell_info.meas_timing_cfg = asn1_served_cell.served_cell_info.meas_timing_cfg.copy();

      // GNB DU sys info
      if (asn1_served_cell.gnb_du_sys_info_present) {
        cu_cp_gnb_du_sys_info gnb_du_sys_info;

        // MIB msg
        gnb_du_sys_info.mib_msg = asn1_served_cell.gnb_du_sys_info.mib_msg.copy();

        // SIB1 msg
        gnb_du_sys_info.sib1_msg = asn1_served_cell.gnb_du_sys_info.sib1_msg.copy();

        served_cell.gnb_du_sys_info = gnb_du_sys_info;
      } else {
        srslog::fetch_basic_logger("CU-CP-F1").error("gNB DU system information must be present for NG-RAN.");
      }

      request.gnb_du_served_cells_list.push_back(served_cell);
    }
  }

  // GNB DU RRC version
  request.gnb_du_rrc_version = asn1_request->gnb_du_rrc_version.latest_rrc_version.to_number();

  // TODO: Add optional fields
}

/// \brief Convert the F1 Setup Response from common type to ASN.1.
/// \param[out] asn1_response The F1 Setup Response ASN.1 struct to store the result.
/// \param[in] msg The common type F1 Setup Response.
inline void fill_asn1_f1_setup_response(asn1::f1ap::f1_setup_resp_s&  asn1_response,
                                        const f1ap_f1_setup_response& response)
{
  // fill CU common info
  if (response.gnb_cu_name.has_value()) {
    asn1_response->gnb_cu_name_present = true;
    asn1_response->gnb_cu_name.from_string(response.gnb_cu_name.value());
  }
  asn1_response->gnb_cu_rrc_version.latest_rrc_version.from_number(response.gnb_cu_rrc_version);

  // activate all DU cells
  if (response.cells_to_be_activ_list.size() > 0) {
    asn1_response->cells_to_be_activ_list_present = true;
    for (const auto& du_cell : response.cells_to_be_activ_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o> resp_cell;
      resp_cell->cells_to_be_activ_list_item().nr_cgi.plmn_id.from_number(plmn_string_to_bcd(du_cell.nr_cgi.plmn));
      resp_cell->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.from_number(du_cell.nr_cgi.nci);

      if (du_cell.nr_pci.has_value()) {
        resp_cell->cells_to_be_activ_list_item().nr_pci_present = true;
        resp_cell->cells_to_be_activ_list_item().nr_pci         = du_cell.nr_pci.value();
      }

      asn1_response->cells_to_be_activ_list.push_back(resp_cell);
    }
  }
}

/// \brief Convert the F1 Setup Failure from common type to ASN.1.
/// \param[out] asn1_failure The F1 Setup Failure ASN.1 struct to store the result.
/// \param[in] msg The common type F1 Setup Failure.
inline void fill_asn1_f1_setup_failure(asn1::f1ap::f1_setup_fail_s& asn1_failure, const f1ap_f1_setup_response& failure)
{
  if (failure.cause.has_value()) {
    asn1_failure->cause = cause_to_f1ap_asn1(failure.cause.value());
  } else {
    asn1_failure->cause.radio_network();
  }

  // TODO: Add optional values
}

/// \brief Convert the UE Context Setup Request from common type to ASN.1.
/// \param[out] asn1_request The ASN.1 struct to store the result.
/// \param[in] request The common type UE Context Setup Request.
inline void fill_asn1_ue_context_setup_request(asn1::f1ap::ue_context_setup_request_s& asn1_request,
                                               const f1ap_ue_context_setup_request&    request)
{
  // sp cell id
  asn1_request->sp_cell_id = nr_cgi_to_f1ap_asn1(request.sp_cell_id);

  // serv cell idx
  asn1_request->serv_cell_idx = request.serv_cell_idx;

  // sp cell ul cfg
  if (request.sp_cell_ul_cfg.has_value()) {
    asn1_request->sp_cell_ul_cfg = cell_ul_cfg_to_asn1(request.sp_cell_ul_cfg.value());
  }

  // cu to du rrc info
  cu_to_du_rrc_info_to_asn1(asn1_request->cu_to_du_rrc_info, request.cu_to_du_rrc_info);

  // candidate sp cell list
  if (!request.candidate_sp_cell_list.empty()) {
    asn1_request->candidate_sp_cell_list_present = true;
    for (const auto& candidate_cell_item : request.candidate_sp_cell_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::candidate_sp_cell_item_ies_o> asn1_candidate_cell_item_container;
      asn1_candidate_cell_item_container.load_info_obj(ASN1_F1AP_ID_CANDIDATE_SP_CELL_ITEM);

      auto& asn1_candidate_cell_item = asn1_candidate_cell_item_container.value().candidate_sp_cell_item();
      asn1_candidate_cell_item.candidate_sp_cell_id = nr_cgi_to_f1ap_asn1(candidate_cell_item.candidate_sp_cell_id);

      asn1_request->candidate_sp_cell_list.push_back(asn1_candidate_cell_item_container);
    }
  }

  // drx cycle
  if (request.drx_cycle.has_value()) {
    asn1_request->drx_cycle_present = true;
    // long drx cycle len
    asn1::number_to_enum(asn1_request->drx_cycle.long_drx_cycle_len, request.drx_cycle.value().long_drx_cycle_len);

    // short drx cycle len
    if (request.drx_cycle.value().short_drx_cycle_len.has_value()) {
      asn1_request->drx_cycle.short_drx_cycle_len_present = true;
      asn1::number_to_enum(asn1_request->drx_cycle.short_drx_cycle_len,
                           request.drx_cycle.value().short_drx_cycle_len.value());
    }

    // short drx cycle timer
    if (request.drx_cycle.value().short_drx_cycle_timer.has_value()) {
      asn1_request->drx_cycle.short_drx_cycle_timer_present = true;
      asn1_request->drx_cycle.short_drx_cycle_timer         = request.drx_cycle.value().short_drx_cycle_timer.value();
    }
  }

  // res coordination transfer container
  if (!request.res_coordination_transfer_container.empty()) {
    asn1_request->res_coordination_transfer_container_present = true;
    asn1_request->res_coordination_transfer_container         = request.res_coordination_transfer_container.copy();
  }

  // scell to be setup list
  if (!request.scell_to_be_setup_list.empty()) {
    asn1_request->scell_to_be_setup_list_present = true;

    for (const auto& scell_to_be_setup_item : request.scell_to_be_setup_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_item_ies_o>
          asn1_scell_to_be_setup_item_container;
      asn1_scell_to_be_setup_item_container.load_info_obj(ASN1_F1AP_ID_SCELL_TO_BE_SETUP_ITEM);

      auto& asn1_scell_to_be_setup_item = asn1_scell_to_be_setup_item_container.value().scell_to_be_setup_item();

      f1ap_scell_to_be_setup_mod_item_to_asn1(asn1_scell_to_be_setup_item, scell_to_be_setup_item);
      asn1_request->scell_to_be_setup_list.push_back(asn1_scell_to_be_setup_item_container);
    }
  }

  // srbs to be setup list
  if (!request.srbs_to_be_setup_list.empty()) {
    asn1_request->srbs_to_be_setup_list_present = true;

    for (const auto& srbs_to_be_setup_item : request.srbs_to_be_setup_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_item_ies_o>
          asn1_srbs_to_be_setup_item_container;
      asn1_srbs_to_be_setup_item_container.set_item(ASN1_F1AP_ID_SRBS_TO_BE_SETUP_ITEM);
      auto& asn1_srbs_to_be_setup_item = asn1_srbs_to_be_setup_item_container.value().srbs_to_be_setup_item();

      f1ap_srbs_to_be_setup_mod_item_to_asn1(asn1_srbs_to_be_setup_item, srbs_to_be_setup_item);

      asn1_request->srbs_to_be_setup_list.push_back(asn1_srbs_to_be_setup_item_container);
    }
  }

  // drbs to be setup list
  if (!request.drbs_to_be_setup_list.empty()) {
    asn1_request->drbs_to_be_setup_list_present = true;

    for (const auto& drb_to_be_setup_item : request.drbs_to_be_setup_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_item_ies_o> asn1_drb_to_be_setup_item_container;
      asn1_drb_to_be_setup_item_container.load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_SETUP_ITEM);

      f1ap_drbs_to_be_setup_mod_item_to_asn1(asn1_drb_to_be_setup_item_container.value().drbs_to_be_setup_item(),
                                             drb_to_be_setup_item);

      asn1_request->drbs_to_be_setup_list.push_back(asn1_drb_to_be_setup_item_container);
    }
  }

  // inactivity monitoring request
  if (request.inactivity_monitoring_request.has_value()) {
    asn1_request->inactivity_monitoring_request_present = true;
    asn1::bool_to_enum(asn1_request->inactivity_monitoring_request, request.inactivity_monitoring_request.value());
  }

  // rat freq prio info
  if (request.rat_freq_prio_info.has_value()) {
    asn1_request->rat_freq_prio_info_present = true;
    asn1_request->rat_freq_prio_info         = f1ap_rat_freq_prio_info_to_asn1(request.rat_freq_prio_info.value());
  }

  // rrc container
  if (!request.rrc_container.empty()) {
    asn1_request->rrc_container_present = true;
    asn1_request->rrc_container         = request.rrc_container.copy();
  }

  // masked imeisv
  if (request.masked_imeisv.has_value()) {
    asn1_request->masked_imeisv_present = true;
    asn1_request->masked_imeisv.from_number(request.masked_imeisv.value());
  }

  // serving plmn
  if (request.serving_plmn.has_value()) {
    asn1_request->serving_plmn_present = true;
    asn1_request->serving_plmn.from_string(request.serving_plmn.value());
  }

  // gnb du ue ambr ul
  if (request.gnb_du_ue_ambr_ul.has_value()) {
    asn1_request->gnb_du_ue_ambr_ul_present = true;
    asn1_request->gnb_du_ue_ambr_ul         = request.gnb_du_ue_ambr_ul.value();
  }

  // rrc delivery status request
  if (request.rrc_delivery_status_request.has_value()) {
    asn1_request->rrc_delivery_status_request_present = true;
    asn1::bool_to_enum(asn1_request->rrc_delivery_status_request, request.rrc_delivery_status_request.value());
  }

  // res coordination transfer info
  if (request.res_coordination_transfer_info.has_value()) {
    asn1_request->res_coordination_transfer_info_present                                  = true;
    asn1_request->res_coordination_transfer_info.res_coordination_eutra_cell_info_present = false;
    asn1_request->res_coordination_transfer_info.m_enb_cell_id.from_number(
        request.res_coordination_transfer_info.value().m_enb_cell_id);
  }

  // serving cell mo
  if (request.serving_cell_mo.has_value()) {
    asn1_request->serving_cell_mo_present = true;
    asn1_request->serving_cell_mo         = request.serving_cell_mo.value();
  }

  // new gnb cu ue f1ap id
  if (request.new_gnb_cu_ue_f1ap_id.has_value()) {
    asn1_request->new_gnb_cu_ue_f1ap_id_present = true;
    asn1_request->new_gnb_cu_ue_f1ap_id         = gnb_cu_ue_f1ap_id_to_uint(request.new_gnb_cu_ue_f1ap_id.value());
  }

  // ran ue id
  if (request.ran_ue_id.has_value()) {
    asn1_request->ran_ue_id_present = true;
    asn1_request->ran_ue_id.from_number(ran_ue_id_to_uint(request.ran_ue_id.value()));
  }
}

/// \brief Convert the UE Context Setup Response from ASN.1 to common type.
/// \param[out] response  The common type struct to store the result.
/// \param[in] asn1_response The ASN.1 UE Context Setup Response.

inline void fill_f1ap_ue_context_setup_response(f1ap_ue_context_setup_response&            response,
                                                ue_index_t                                 ue_index,
                                                const asn1::f1ap::ue_context_setup_resp_s& asn1_response)
{
  response.success  = true;
  response.ue_index = ue_index;

  // du to cu rrc info
  response.du_to_cu_rrc_info.cell_group_cfg      = asn1_response->du_to_cu_rrc_info.cell_group_cfg.copy();
  response.du_to_cu_rrc_info.meas_gap_cfg        = asn1_response->du_to_cu_rrc_info.meas_gap_cfg.copy();
  response.du_to_cu_rrc_info.requested_p_max_fr1 = asn1_response->du_to_cu_rrc_info.requested_p_max_fr1.copy();

  // c rnti
  if (asn1_response->c_rnti_present) {
    response.c_rnti = to_rnti(asn1_response->c_rnti);
  }

  // res coordination transfer container
  if (asn1_response->res_coordination_transfer_container_present) {
    response.res_coordination_transfer_container = asn1_response->res_coordination_transfer_container.copy();
  }

  // full cfg
  if (asn1_response->full_cfg_present) {
    response.full_cfg = asn1::enum_to_bool(asn1_response->full_cfg);
  }

  // drbs setup list
  if (asn1_response->drbs_setup_list_present) {
    for (auto asn1_drbs_setup_list_item : asn1_response->drbs_setup_list) {
      auto& asn1_drb_mod_item = asn1_drbs_setup_list_item.value().drbs_setup_item();

      f1ap_drbs_setup_mod_item drb_setup_item = asn1_to_f1ap_drbs_setup_mod_item(asn1_drb_mod_item);

      response.drbs_setup_list.emplace(drb_setup_item.drb_id, drb_setup_item);
    }
  }

  // srbs failed to be setup list
  if (asn1_response->srbs_failed_to_be_setup_list_present) {
    for (auto asn1_srbs_failed_setup_list_item : asn1_response->srbs_failed_to_be_setup_list) {
      auto& asn1_srb_failed_item = asn1_srbs_failed_setup_list_item.value().srbs_failed_to_be_setup_item();

      f1ap_srbs_failed_to_be_setup_mod_item srb_failed_item =
          asn1_to_f1ap_srbs_failed_to_be_setup_mod_item(asn1_srb_failed_item);

      response.srbs_failed_to_be_setup_list.emplace(srb_failed_item.srb_id, srb_failed_item);
    }
  }

  // drbs failed to be setup list
  if (asn1_response->drbs_failed_to_be_setup_list_present) {
    for (auto asn1_drbs_failed_setup_list_item : asn1_response->drbs_failed_to_be_setup_list) {
      auto& asn1_drb_failed_item = asn1_drbs_failed_setup_list_item.value().drbs_failed_to_be_setup_item();

      f1ap_drbs_failed_to_be_setup_mod_item drb_failed_item =
          asn1_to_f1ap_drbs_failed_to_be_setup_mod_item(asn1_drb_failed_item);

      response.drbs_failed_to_be_setup_list.emplace(drb_failed_item.drb_id, drb_failed_item);
    }
  }

  // scell failed to setup list
  if (asn1_response->scell_failedto_setup_list_present) {
    for (const auto& asn1_scell_failed_to_setup_item : asn1_response->scell_failedto_setup_list) {
      f1ap_scell_failed_to_setup_mod_item scell_failed_to_setup_item;

      // scell id
      scell_failed_to_setup_item.scell_id =
          f1ap_asn1_to_nr_cgi(asn1_scell_failed_to_setup_item->scell_failedto_setup_item().scell_id);

      // cause
      if (asn1_scell_failed_to_setup_item->scell_failedto_setup_item().cause_present) {
        scell_failed_to_setup_item.cause =
            f1ap_asn1_to_cause(asn1_scell_failed_to_setup_item->scell_failedto_setup_item().cause);
      }

      response.scell_failed_to_setup_list.push_back(scell_failed_to_setup_item);
    }
  }

  // Add inactivity monitoring response
  if (asn1_response->inactivity_monitoring_resp_present) {
    bool inactivity_monitoring_resp     = asn1::enum_to_bool(asn1_response->inactivity_monitoring_resp);
    response.inactivity_monitoring_resp = inactivity_monitoring_resp;
  }

  // srbs setup list
  if (asn1_response->srbs_setup_list_present) {
    for (auto asn1_srbs_setup_list_item : asn1_response->srbs_setup_list) {
      auto& asn1_srbs_setup_item = asn1_srbs_setup_list_item.value().srbs_setup_item();

      f1ap_srbs_setup_mod_item srbs_setup_item = asn1_to_f1ap_srbs_setup_mod_item(asn1_srbs_setup_item);

      response.srbs_setup_list.emplace(srbs_setup_item.srb_id, srbs_setup_item);
    }
  }
}

/// \brief Convert the UE Context Setup Response from ASN.1 to common type.
/// \param[out] response  The common type struct to store the result.
/// \param[in] asn1_failure The ASN.1 UE Context Setup Failure.

inline void fill_f1ap_ue_context_setup_response(f1ap_ue_context_setup_response&            response,
                                                const asn1::f1ap::ue_context_setup_fail_s& asn1_failure)
{
  response.success = false;

  // cause
  response.cause = f1ap_asn1_to_cause(asn1_failure->cause);

  // potential sp cell list
  if (asn1_failure->potential_sp_cell_list_present) {
    f1ap_potential_sp_cell_item potential_sp_cell_item;

    for (const auto& asn1_potential_sp_cell_item : asn1_failure->potential_sp_cell_list) {
      potential_sp_cell_item.potential_sp_cell_id =
          f1ap_asn1_to_nr_cgi(asn1_potential_sp_cell_item->potential_sp_cell_item().potential_sp_cell_id);

      response.potential_sp_cell_list.push_back(potential_sp_cell_item);
    }
  }

  // crit diagnostics
  if (asn1_failure->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }
}

/// \brief Convert the UE Context Modification Request from common type to ASN.1.
/// \param[out] asn1_request The ASN.1 struct to store the result.
/// \param[in] request The common type UE Context Modification Request.
inline void fill_asn1_ue_context_modification_request(asn1::f1ap::ue_context_mod_request_s&       asn1_request,
                                                      const f1ap_ue_context_modification_request& request)
{
  // sp cell id
  if (request.sp_cell_id.has_value()) {
    asn1_request->sp_cell_id_present = true;
    asn1_request->sp_cell_id.nr_cell_id.from_number(request.sp_cell_id.value().nci);
    asn1_request->sp_cell_id.plmn_id.from_string(request.sp_cell_id.value().plmn_hex);
  }

  // serv cell idx
  if (request.serv_cell_idx.has_value()) {
    asn1_request->serv_cell_idx_present = true;
    asn1_request->serv_cell_idx         = request.serv_cell_idx.value();
  }

  // sp cell ul cfg
  if (request.sp_cell_ul_cfg.has_value()) {
    asn1_request->sp_cell_ul_cfg_present = true;
    asn1_request->sp_cell_ul_cfg         = cell_ul_cfg_to_asn1(request.sp_cell_ul_cfg.value());
  }

  // drx cycle
  if (request.drx_cycle.has_value()) {
    asn1_request->drx_cycle_present = true;
    asn1::number_to_enum(asn1_request->drx_cycle.long_drx_cycle_len, request.drx_cycle.value().long_drx_cycle_len);

    if (request.drx_cycle.value().short_drx_cycle_len.has_value()) {
      asn1_request->drx_cycle.short_drx_cycle_len_present = true;
      asn1::number_to_enum(asn1_request->drx_cycle.short_drx_cycle_len,
                           request.drx_cycle.value().short_drx_cycle_len.value());
    }

    if (request.drx_cycle.value().short_drx_cycle_timer.has_value()) {
      asn1_request->drx_cycle.short_drx_cycle_timer_present = true;
      asn1_request->drx_cycle.short_drx_cycle_timer         = request.drx_cycle.value().short_drx_cycle_timer.value();
    }
  }

  // cu to du rrc info
  if (request.cu_to_du_rrc_info.has_value()) {
    asn1_request->cu_to_du_rrc_info_present     = true;
    asn1_request->cu_to_du_rrc_info.cg_cfg_info = request.cu_to_du_rrc_info.value().cg_cfg_info.copy();
    asn1_request->cu_to_du_rrc_info.ue_cap_rat_container_list =
        request.cu_to_du_rrc_info.value().ue_cap_rat_container_list.copy();
    asn1_request->cu_to_du_rrc_info.meas_cfg = request.cu_to_du_rrc_info.value().meas_cfg.copy();
  }

  // tx action ind
  if (request.tx_action_ind.has_value()) {
    asn1_request->tx_action_ind_present = true;
    asn1_request->tx_action_ind         = f1ap_tx_action_ind_to_asn1(request.tx_action_ind.value());
  }

  // res coordination transfer container
  if (!request.res_coordination_transfer_container.empty()) {
    asn1_request->res_coordination_transfer_container_present = true;
    asn1_request->res_coordination_transfer_container         = request.res_coordination_transfer_container.copy();
  }

  // rrc recfg complete ind
  if (request.rrc_recfg_complete_ind.has_value()) {
    asn1_request->rrc_recfg_complete_ind_present = true;
    asn1_request->rrc_recfg_complete_ind = f1ap_rrc_recfg_complete_ind_to_asn1(request.rrc_recfg_complete_ind.value());
  }

  // rrc container
  if (!request.rrc_container.empty()) {
    asn1_request->rrc_container_present = true;
    asn1_request->rrc_container         = request.rrc_container.copy();
  }

  // scell to be setup mod list
  if (!request.scell_to_be_setup_mod_list.empty()) {
    asn1_request->scell_to_be_setup_mod_list_present = true;
    for (const auto& scell_to_be_setup_mod_item : request.scell_to_be_setup_mod_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_mod_item_ies_o>
          asn1_scell_to_be_setup_mod_item_container;
      asn1_scell_to_be_setup_mod_item_container.load_info_obj(ASN1_F1AP_ID_SCELL_TO_BE_SETUP_MOD_ITEM);

      auto& asn1_scell_to_be_setup_mod_item =
          asn1_scell_to_be_setup_mod_item_container.value().scell_to_be_setup_mod_item();

      f1ap_scell_to_be_setup_mod_item_to_asn1(asn1_scell_to_be_setup_mod_item, scell_to_be_setup_mod_item);

      asn1_request->scell_to_be_setup_mod_list.push_back(asn1_scell_to_be_setup_mod_item_container);
    }
  }

  // scell to be remd list
  if (!request.scell_to_be_remd_list.empty()) {
    asn1_request->scell_to_be_remd_list_present = true;
    for (const auto& scell_to_be_remd_item : request.scell_to_be_remd_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_remd_item_ies_o>
          asn1_scell_to_be_remd_item_container;
      asn1_scell_to_be_remd_item_container.load_info_obj(ASN1_F1AP_ID_SCELL_TO_BE_REMD_ITEM);

      auto& asn1_scell_to_be_remd_item = asn1_scell_to_be_remd_item_container.value().scell_to_be_remd_item();
      asn1_scell_to_be_remd_item.scell_id.nr_cell_id.from_number(scell_to_be_remd_item.scell_id.nci);
      asn1_scell_to_be_remd_item.scell_id.plmn_id.from_string(scell_to_be_remd_item.scell_id.plmn_hex);

      asn1_request->scell_to_be_remd_list.push_back(asn1_scell_to_be_remd_item_container);
    }
  }

  // srbs to be setup mod list
  if (!request.srbs_to_be_setup_mod_list.empty()) {
    asn1_request->srbs_to_be_setup_mod_list_present = true;
    for (const auto& srbs_to_be_setup_mod_item : request.srbs_to_be_setup_mod_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_mod_item_ies_o>
          asn1_srbs_to_be_setup_mod_item_container;
      asn1_srbs_to_be_setup_mod_item_container.load_info_obj(ASN1_F1AP_ID_SRBS_TO_BE_SETUP_MOD_ITEM);

      auto& asn1_srbs_to_be_setup_mod_item =
          asn1_srbs_to_be_setup_mod_item_container.value().srbs_to_be_setup_mod_item();

      f1ap_srbs_to_be_setup_mod_item_to_asn1(asn1_srbs_to_be_setup_mod_item, srbs_to_be_setup_mod_item);
      asn1_request->srbs_to_be_setup_mod_list.push_back(asn1_srbs_to_be_setup_mod_item_container);
    }
  }

  // drbs to be setup mod list
  if (!request.drbs_to_be_setup_mod_list.empty()) {
    asn1_request->drbs_to_be_setup_mod_list_present = true;
    for (const auto& drb_to_be_setup_mod_item : request.drbs_to_be_setup_mod_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o>
          asn1_drb_to_be_setup_mod_item_container;
      asn1_drb_to_be_setup_mod_item_container.load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_SETUP_MOD_ITEM);

      f1ap_drbs_to_be_setup_mod_item_to_asn1(
          asn1_drb_to_be_setup_mod_item_container.value().drbs_to_be_setup_mod_item(), drb_to_be_setup_mod_item);

      asn1_request->drbs_to_be_setup_mod_list.push_back(asn1_drb_to_be_setup_mod_item_container);
    }
  }

  // drbs to be modified list
  if (!request.drbs_to_be_modified_list.empty()) {
    asn1_request->drbs_to_be_modified_list_present = true;

    for (const auto& drb_to_be_modified_item : request.drbs_to_be_modified_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_modified_item_ies_o>
          asn1_drb_to_be_modified_item_container;
      asn1_drb_to_be_modified_item_container.load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_MODIFIED_ITEM);

      auto& asn1_drb_to_be_modified_item  = asn1_drb_to_be_modified_item_container.value().drbs_to_be_modified_item();
      asn1_drb_to_be_modified_item.drb_id = drb_id_to_uint(drb_to_be_modified_item.drb_id);

      // qos info
      if (drb_to_be_modified_item.qos_info.has_value()) {
        asn1_drb_to_be_modified_item.qos_info_present = true;
        asn1_drb_to_be_modified_item.qos_info         = f1ap_qos_info_to_asn1(drb_to_be_modified_item.qos_info.value());
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

        asn1_drb_to_be_modified_item.ul_cfg.ul_ue_cfg =
            f1ap_ul_ue_cfg_to_asn1(drb_to_be_modified_item.ul_cfg.value().ul_ue_cfg);
      }

      asn1_request->drbs_to_be_modified_list.push_back(asn1_drb_to_be_modified_item_container);
    }
  }

  // srbs to be released list
  if (!request.srbs_to_be_released_list.empty()) {
    asn1_request->srbs_to_be_released_list_present = true;
    for (const auto& srb_id : request.srbs_to_be_released_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_released_item_ies_o>
          asn1_srb_to_be_released_item_container;
      asn1_srb_to_be_released_item_container.load_info_obj(ASN1_F1AP_ID_SRBS_TO_BE_RELEASED_ITEM);

      auto& asn1_srb_to_be_released_item  = asn1_srb_to_be_released_item_container.value().srbs_to_be_released_item();
      asn1_srb_to_be_released_item.srb_id = srb_id_to_uint(srb_id);

      asn1_request->srbs_to_be_released_list.push_back(asn1_srb_to_be_released_item_container);
    }
  }

  // drbs to be released list
  if (!request.drbs_to_be_released_list.empty()) {
    asn1_request->drbs_to_be_released_list_present = true;
    for (const auto& drb_id : request.drbs_to_be_released_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_released_item_ies_o>
          asn1_drb_to_be_released_item_container;
      asn1_drb_to_be_released_item_container.load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_RELEASED_ITEM);

      auto& asn1_drb_to_be_released_item  = asn1_drb_to_be_released_item_container.value().drbs_to_be_released_item();
      asn1_drb_to_be_released_item.drb_id = drb_id_to_uint(drb_id);

      asn1_request->drbs_to_be_released_list.push_back(asn1_drb_to_be_released_item_container);
    }
  }

  // inactivity monitoring request
  if (request.inactivity_monitoring_request.has_value()) {
    asn1_request->inactivity_monitoring_request_present = true;
    asn1::bool_to_enum(asn1_request->inactivity_monitoring_request, request.inactivity_monitoring_request.value());
  }

  // rat freq prio info
  if (request.rat_freq_prio_info.has_value()) {
    asn1_request->rat_freq_prio_info_present = true;
    asn1_request->rat_freq_prio_info         = f1ap_rat_freq_prio_info_to_asn1(request.rat_freq_prio_info.value());
  }

  // drx cfg ind
  if (request.drx_cfg_ind.has_value()) {
    asn1_request->drx_cfg_ind_present = true;
    asn1::bool_to_enum(asn1_request->drx_cfg_ind, request.drx_cfg_ind.value());
  }

  // rlc fail ind
  if (request.rlc_fail_ind.has_value()) {
    asn1_request->rlc_fail_ind_present        = true;
    asn1_request->rlc_fail_ind.assocated_lcid = request.rlc_fail_ind.value().assocated_lcid;
  }

  // ul tx direct current list info
  if (!request.ul_tx_direct_current_list_info.empty()) {
    asn1_request->ul_tx_direct_current_list_info_present = true;
    asn1_request->ul_tx_direct_current_list_info         = request.ul_tx_direct_current_list_info.copy();
  }

  // gnb du cfg query
  if (request.gnb_du_cfg_query.has_value()) {
    asn1_request->gnb_du_cfg_query_present = true;
    asn1::bool_to_enum(asn1_request->gnb_du_cfg_query, request.gnb_du_cfg_query.value());
  }

  // gnb du ue ambr ul
  if (request.gnb_du_ue_ambr_ul.has_value()) {
    asn1_request->gnb_du_ue_ambr_ul_present = true;
    asn1_request->gnb_du_ue_ambr_ul         = request.gnb_du_ue_ambr_ul.value();
  }

  // execute dupl
  if (request.execute_dupl.has_value()) {
    asn1_request->execute_dupl_present = true;
    asn1::bool_to_enum(asn1_request->execute_dupl, request.execute_dupl.value());
  }

  // rrc delivery status request
  if (request.rrc_delivery_status_request.has_value()) {
    asn1_request->rrc_delivery_status_request_present = true;
    asn1::bool_to_enum(asn1_request->rrc_delivery_status_request, request.rrc_delivery_status_request.value());
  }

  // res coordination transfer info
  if (request.res_coordination_transfer_info.has_value()) {
    asn1_request->res_coordination_transfer_info_present                                  = true;
    asn1_request->res_coordination_transfer_info.res_coordination_eutra_cell_info_present = false;
    asn1_request->res_coordination_transfer_info.m_enb_cell_id.from_number(
        request.res_coordination_transfer_info.value().m_enb_cell_id);
  }

  // serving cell mo
  if (request.serving_cell_mo.has_value()) {
    asn1_request->serving_cell_mo_present = true;
    asn1_request->serving_cell_mo         = request.serving_cell_mo.value();
  }

  // need for gap
  if (request.need_for_gap.has_value()) {
    asn1_request->needfor_gap_present = true;
    asn1::bool_to_enum(asn1_request->needfor_gap, request.need_for_gap.value());
  }

  // full cfg
  if (request.full_cfg.has_value()) {
    asn1_request->full_cfg_present = true;
    asn1::bool_to_enum(asn1_request->full_cfg, request.full_cfg.value());
  }
}

/// \brief Convert the UE Context Modification Response from ASN.1 to common type.
/// \param[out] res The common type struct to store the result.
/// \param[in] asn1_response The ASN.1 type UE Context Modification Response.
inline void fill_f1ap_ue_context_modification_response(f1ap_ue_context_modification_response&   res,
                                                       const asn1::f1ap::ue_context_mod_resp_s& asn1_response)
{
  res.success = true;

  // DUtoCURRCInformation
  if (asn1_response->du_to_cu_rrc_info_present) {
    res.du_to_cu_rrc_info.cell_group_cfg      = asn1_response->du_to_cu_rrc_info.cell_group_cfg.copy();
    res.du_to_cu_rrc_info.meas_gap_cfg        = asn1_response->du_to_cu_rrc_info.meas_gap_cfg.copy();
    res.du_to_cu_rrc_info.requested_p_max_fr1 = asn1_response->du_to_cu_rrc_info.requested_p_max_fr1.copy();
  }

  // Add DRBs setup mod list
  if (asn1_response->drbs_setup_mod_list_present) {
    for (auto asn1_drb_setup_mod_list_item : asn1_response->drbs_setup_mod_list) {
      auto& asn1_drb_mod_item = asn1_drb_setup_mod_list_item.value().drbs_setup_mod_item();

      f1ap_drbs_setup_mod_item drb_setup_mod_item = asn1_to_f1ap_drbs_setup_mod_item(asn1_drb_mod_item);

      res.drbs_setup_mod_list.emplace(drb_setup_mod_item.drb_id, drb_setup_mod_item);
    }
  }

  // Add DRBs modified list
  if (asn1_response->drbs_modified_list_present) {
    for (auto asn1_drbs_modified_list_item : asn1_response->drbs_modified_list) {
      auto& asn1_drb_mod_item = asn1_drbs_modified_list_item.value().drbs_modified_item();

      f1ap_drbs_setup_mod_item drb_setup_mod_item = asn1_to_f1ap_drbs_setup_mod_item(asn1_drb_mod_item);

      res.drbs_modified_list.emplace(drb_setup_mod_item.drb_id, drb_setup_mod_item);
    }
  }

  // Add SRBs failed to be setup mod list
  if (asn1_response->srbs_failed_to_be_setup_mod_list_present) {
    for (auto asn1_srbs_failed_setup_mod_list_item : asn1_response->srbs_failed_to_be_setup_mod_list) {
      auto& asn1_srb_failed_item = asn1_srbs_failed_setup_mod_list_item.value().srbs_failed_to_be_setup_mod_item();

      f1ap_srbs_failed_to_be_setup_mod_item srb_failed_item =
          asn1_to_f1ap_srbs_failed_to_be_setup_mod_item(asn1_srb_failed_item);

      res.srbs_failed_to_be_setup_mod_list.emplace(srb_failed_item.srb_id, srb_failed_item);
    }
  }

  // Add DRBs failed to be setup mod list
  if (asn1_response->drbs_failed_to_be_setup_mod_list_present) {
    for (auto asn1_drbs_failed_setup_mod_list_item : asn1_response->drbs_failed_to_be_setup_mod_list) {
      auto& asn1_drb_failed_item = asn1_drbs_failed_setup_mod_list_item.value().drbs_failed_to_be_setup_mod_item();

      f1ap_drbs_failed_to_be_setup_mod_item drb_failed_item =
          asn1_to_f1ap_drbs_failed_to_be_setup_mod_item(asn1_drb_failed_item);

      res.drbs_failed_to_be_setup_mod_list.emplace(drb_failed_item.drb_id, drb_failed_item);
    }
  }

  // Add SCell failed to be setup mod list
  if (asn1_response->scell_failedto_setup_mod_list_present) {
    for (auto asn1_scell_failed_setup_mod_list_item : asn1_response->scell_failedto_setup_mod_list) {
      auto& asn1_scell_failed_item = asn1_scell_failed_setup_mod_list_item.value().scell_failedto_setup_mod_item();

      f1ap_scell_failed_to_setup_mod_item scell_failed_item;
      scell_failed_item.scell_id = cgi_from_asn1(asn1_scell_failed_item.scell_id);
      if (asn1_scell_failed_item.cause_present) {
        scell_failed_item.cause = f1ap_asn1_to_cause(asn1_scell_failed_item.cause);
      }
      res.scell_failed_to_setup_mod_list.push_back(scell_failed_item);
    }
  }

  // Add DRBs failed to be modified list
  if (asn1_response->drbs_failed_to_be_modified_list_present) {
    for (auto asn1_drbs_failed_modified_list_item : asn1_response->drbs_failed_to_be_modified_list) {
      auto& asn1_drb_failed_item = asn1_drbs_failed_modified_list_item.value().drbs_failed_to_be_modified_item();

      f1ap_drbs_failed_to_be_setup_mod_item drb_failed_item =
          asn1_to_f1ap_drbs_failed_to_be_setup_mod_item(asn1_drb_failed_item);

      res.drbs_failed_to_be_modified_list.emplace(drb_failed_item.drb_id, drb_failed_item);
    }
  }

  // Add inactivity monitoring response
  if (asn1_response->inactivity_monitoring_resp_present) {
    res.inactivity_monitoring_resp = asn1::enum_to_bool(asn1_response->inactivity_monitoring_resp);
  }

  // Add C-RNTI
  if (asn1_response->c_rnti_present) {
    res.c_rnti = to_rnti(asn1_response->c_rnti);
  }

  // Add associated SCell list
  if (asn1_response->associated_scell_list_present) {
    for (auto asn1_associated_scell_list_item : asn1_response->associated_scell_list) {
      auto& asn1_associated_scell_item = asn1_associated_scell_list_item.value().associated_scell_item();

      f1ap_associated_scell_item associated_scell_item;
      associated_scell_item.scell_id = f1ap_asn1_to_nr_cell_identity(asn1_associated_scell_item.scell_id);

      res.associated_scell_list.push_back(associated_scell_item);
    }
  }

  // Add SRBs setup mod list
  if (asn1_response->srbs_setup_mod_list_present) {
    for (auto asn1_srbs_setup_mod_list_item : asn1_response->srbs_setup_mod_list) {
      auto& asn1_srbs_setup_mod_item = asn1_srbs_setup_mod_list_item.value().srbs_setup_mod_item();

      f1ap_srbs_setup_mod_item srbs_setup_mod_item = asn1_to_f1ap_srbs_setup_mod_item(asn1_srbs_setup_mod_item);

      res.srbs_setup_mod_list.emplace(srbs_setup_mod_item.srb_id, srbs_setup_mod_item);
    }
  }

  // Add SRBs modified list
  if (asn1_response->srbs_modified_list_present) {
    for (auto asn1_srbs_modified_list_item : asn1_response->srbs_modified_list) {
      auto& asn1_srbs_modified_item = asn1_srbs_modified_list_item.value().srbs_modified_item();

      f1ap_srbs_setup_mod_item srbs_modified_item = asn1_to_f1ap_srbs_setup_mod_item(asn1_srbs_modified_item);

      res.srbs_modified_list.emplace(srbs_modified_item.srb_id, srbs_modified_item);
    }
  }

  // Add full configuration
  if (asn1_response->full_cfg_present) {
    res.full_cfg = asn1_response->full_cfg.to_string();
  }
}

/// \brief Convert the UE Context Modification Failure from ASN.1 to common type.
/// \param[out] res The common type struct to store the result.
/// \param[in] asn1_fail The ASN.1 type UE Context Modification Failure.
inline void fill_f1ap_ue_context_modification_response(f1ap_ue_context_modification_response&   res,
                                                       const asn1::f1ap::ue_context_mod_fail_s& asn1_fail)
{
  res.success = false;
  res.cause   = f1ap_asn1_to_cause(asn1_fail->cause);
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
  uint64_t five_g_s_tmsi = five_g_s_tmsi_struct_to_number(paging.ue_paging_id);

  // UE Identity Index value is defined as: UE_ID 5G-S-TMSI mod 1024  (see TS 38.304 section 7.1)
  asn1_paging->ue_id_idx_value.set_idx_len10().from_number(five_g_s_tmsi % 1024);

  // Add paging id
  asn1_paging->paging_id.set_cn_ue_paging_id().set_five_g_s_tmsi().from_number(five_g_s_tmsi);

  // Add paging drx
  if (paging.paging_drx.has_value()) {
    asn1_paging->paging_drx_present = true;
    asn1::number_to_enum(asn1_paging->paging_drx, paging.paging_drx.value());
  }

  // Add paging prio
  if (paging.paging_prio.has_value()) {
    asn1_paging->paging_prio_present = true;
    asn1::number_to_enum(asn1_paging->paging_prio, paging.paging_prio.value());
  }

  // Add paging cell list
  for (const auto& cell_item : paging.assist_data_for_paging.value()
                                   .assist_data_for_recommended_cells.value()
                                   .recommended_cells_for_paging.recommended_cell_list) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::paging_cell_item_ies_o> asn1_paging_cell_item_container;
    auto& asn1_paging_cell_item = asn1_paging_cell_item_container->paging_cell_item();

    asn1_paging_cell_item.nr_cgi.nr_cell_id.from_number(cell_item.ngran_cgi.nci);
    asn1_paging_cell_item.nr_cgi.plmn_id.from_string(cell_item.ngran_cgi.plmn_hex);

    asn1_paging->paging_cell_list.push_back(asn1_paging_cell_item_container);
  }

  // Add paging origin
  if (paging.paging_origin.has_value()) {
    asn1_paging->paging_origin_present = true;
    asn1::bool_to_enum(asn1_paging->paging_origin, paging.paging_origin.value());
  }
}

} // namespace srs_cu_cp
} // namespace srsran
