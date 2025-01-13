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

#include "ue_context_modification_procedure.h"
#include "../f1ap_asn1_converters.h"
#include "asn1_helpers.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ran/cause/ngap_cause.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::f1ap;

/// \brief Convert the UE Context Modification Request from common type to ASN.1.
/// \param[out] asn1_request The ASN.1 struct to store the result.
/// \param[in] request The common type UE Context Modification Request.
static void fill_asn1_ue_context_modification_request(asn1::f1ap::ue_context_mod_request_s&       asn1_request,
                                                      const f1ap_ue_context_modification_request& request);

/// \brief Convert the UE Context Modification Response from ASN.1 to common type.
/// \param[out] res The common type struct to store the result.
/// \param[in] asn1_response The ASN.1 type UE Context Modification Response.
static void fill_f1ap_ue_context_modification_response(f1ap_ue_context_modification_response&   res,
                                                       const asn1::f1ap::ue_context_mod_resp_s& asn1_response);

/// \brief Convert the UE Context Modification Failure from ASN.1 to common type.
/// \param[out] res The common type struct to store the result.
/// \param[in] asn1_fail The ASN.1 type UE Context Modification Failure.
static void fill_f1ap_ue_context_modification_response(f1ap_ue_context_modification_response&   res,
                                                       const asn1::f1ap::ue_context_mod_fail_s& asn1_fail);

ue_context_modification_procedure::ue_context_modification_procedure(
    const f1ap_configuration&                   f1ap_cfg_,
    const f1ap_ue_context_modification_request& request_,
    f1ap_ue_context&                            ue_ctxt_,
    f1ap_message_notifier&                      f1ap_notif_) :
  f1ap_cfg(f1ap_cfg_),
  request(request_),
  ue_ctxt(ue_ctxt_),
  f1ap_notifier(f1ap_notif_),
  logger(srslog::fetch_basic_logger("CU-CP-F1"))
{
}

void ue_context_modification_procedure::operator()(coro_context<async_task<f1ap_ue_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("{}: Procedure started...", f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()});

  // Subscribe to respective publisher to receive UE CONTEXT MODIFICATION RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt.ev_mng.context_modification_outcome, f1ap_cfg.proc_timeout);

  // Send command to DU.
  send_ue_context_modification_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_modification_result());
}

void ue_context_modification_procedure::send_ue_context_modification_request()
{
  // Pack message into PDU
  f1ap_message f1ap_ue_ctxt_mod_request_msg;
  f1ap_ue_ctxt_mod_request_msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_request_s& ctx_mod = f1ap_ue_ctxt_mod_request_msg.pdu.init_msg().value.ue_context_mod_request();

  fill_asn1_ue_context_modification_request(ctx_mod, request);

  srsran_sanity_check(ue_ctxt.ue_ids.du_ue_f1ap_id && ue_ctxt.ue_ids.du_ue_f1ap_id != gnb_du_ue_f1ap_id_t::invalid,
                      "Invalid gNB-DU-UE-F1AP-Id");
  ctx_mod->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(*ue_ctxt.ue_ids.du_ue_f1ap_id);
  ctx_mod->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.ue_ids.cu_ue_f1ap_id);

  // send UE context modification request message
  f1ap_notifier.on_new_message(f1ap_ue_ctxt_mod_request_msg);
}

f1ap_ue_context_modification_response ue_context_modification_procedure::create_ue_context_modification_result()
{
  f1ap_ue_context_modification_response res{};

  auto logger_prefix = f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()};

  if (transaction_sink.successful()) {
    const asn1::f1ap::ue_context_mod_resp_s& resp = transaction_sink.response();

    logger.info("{}: Procedure finished successfully", logger_prefix);

    fill_f1ap_ue_context_modification_response(res, resp);

  } else if (transaction_sink.failed()) {
    const asn1::f1ap::ue_context_mod_fail_s& fail = transaction_sink.failure();

    logger.warning("{}: Procedure failed. Cause: {}", logger_prefix, get_cause_str(fail->cause));

    fill_f1ap_ue_context_modification_response(res, fail);
  } else {
    logger.warning("{}: Procedure failed. Cause: Timeout reached for UEContextModificationResponse reception",
                   logger_prefix);

    res.success = false;
    res.cause   = cause_misc_t::unspecified;
  }

  return res;
}

static void fill_asn1_ue_context_modification_request(asn1::f1ap::ue_context_mod_request_s&       asn1_request,
                                                      const f1ap_ue_context_modification_request& request)
{
  // sp cell id
  if (request.sp_cell_id.has_value()) {
    asn1_request->sp_cell_id_present = true;
    asn1_request->sp_cell_id.nr_cell_id.from_number(request.sp_cell_id.value().nci.value());
    asn1_request->sp_cell_id.plmn_id = request.sp_cell_id.value().plmn_id.to_bytes();
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
      asn1_scell_to_be_remd_item.scell_id.nr_cell_id.from_number(scell_to_be_remd_item.scell_id.nci.value());
      asn1_scell_to_be_remd_item.scell_id.plmn_id = scell_to_be_remd_item.scell_id.plmn_id.to_bytes();

      asn1_request->scell_to_be_remd_list.push_back(asn1_scell_to_be_remd_item_container);
    }
  }

  // srbs to be setup mod list
  if (!request.srbs_to_be_setup_mod_list.empty()) {
    asn1_request->srbs_to_be_setup_mod_list_present = true;
    asn1_request->srbs_to_be_setup_mod_list         = make_srb_to_setupmod_list(request.srbs_to_be_setup_mod_list);
  }

  // drbs to be setup mod list
  if (!request.drbs_to_be_setup_mod_list.empty()) {
    asn1_request->drbs_to_be_setup_mod_list_present = true;
    asn1_request->drbs_to_be_setup_mod_list         = make_drb_to_setupmod_list(request.drbs_to_be_setup_mod_list);
  }

  // drbs to be modified list
  if (!request.drbs_to_be_modified_list.empty()) {
    asn1_request->drbs_to_be_modified_list_present = true;
    asn1_request->drbs_to_be_modified_list         = make_drb_to_modify_list(request.drbs_to_be_modified_list);
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
        request.res_coordination_transfer_info.value().m_enb_cell_id.value());
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

static void fill_f1ap_ue_context_modification_response(f1ap_ue_context_modification_response&   res,
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
      res.drbs_setup_list.push_back(make_drb_setupmod(asn1_drb_mod_item));
    }
  }

  // Add DRBs modified list
  if (asn1_response->drbs_modified_list_present) {
    for (auto asn1_drbs_modified_list_item : asn1_response->drbs_modified_list) {
      auto& asn1_drb_mod_item = asn1_drbs_modified_list_item.value().drbs_modified_item();
      res.drbs_modified_list.push_back(make_drb_setupmod(asn1_drb_mod_item));
    }
  }

  // Add SRBs failed to be setup mod list
  if (asn1_response->srbs_failed_to_be_setup_mod_list_present) {
    for (auto asn1_srbs_failed_setup_mod_list_item : asn1_response->srbs_failed_to_be_setup_mod_list) {
      auto& asn1_srb_failed_item = asn1_srbs_failed_setup_mod_list_item.value().srbs_failed_to_be_setup_mod_item();
      res.srbs_failed_to_be_setup_list.push_back(make_srb_failed_to_setupmod(asn1_srb_failed_item));
    }
  }

  // Add DRBs failed to be setup mod list
  if (asn1_response->drbs_failed_to_be_setup_mod_list_present) {
    for (auto asn1_drbs_failed_setup_mod_list_item : asn1_response->drbs_failed_to_be_setup_mod_list) {
      auto& asn1_drb_failed_item = asn1_drbs_failed_setup_mod_list_item.value().drbs_failed_to_be_setup_mod_item();
      res.drbs_failed_to_be_setup_list.push_back(make_drb_failed_to_setupmod(asn1_drb_failed_item));
    }
  }

  // Add SCell failed to be setup mod list
  if (asn1_response->scell_failedto_setup_mod_list_present) {
    for (auto asn1_scell_failed_setup_mod_list_item : asn1_response->scell_failedto_setup_mod_list) {
      auto& asn1_scell_failed_item = asn1_scell_failed_setup_mod_list_item.value().scell_failedto_setup_mod_item();

      f1ap_scell_failed_to_setup_mod_item scell_failed_item;
      scell_failed_item.scell_id = cgi_from_asn1(asn1_scell_failed_item.scell_id).value();
      if (asn1_scell_failed_item.cause_present) {
        scell_failed_item.cause = asn1_to_cause(asn1_scell_failed_item.cause);
      }
      res.scell_failed_to_be_setup_list.push_back(scell_failed_item);
    }
  }

  // Add DRBs failed to be modified list
  if (asn1_response->drbs_failed_to_be_modified_list_present) {
    for (auto asn1_drbs_failed_modified_list_item : asn1_response->drbs_failed_to_be_modified_list) {
      auto& asn1_drb_failed_item = asn1_drbs_failed_modified_list_item.value().drbs_failed_to_be_modified_item();
      res.drbs_failed_to_be_modified_list.push_back(make_drb_failed_to_setupmod(asn1_drb_failed_item));
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
      associated_scell_item.scell_id = nr_cell_id_from_asn1(asn1_associated_scell_item.scell_id).value();

      res.associated_scell_list.push_back(associated_scell_item);
    }
  }

  // Add SRBs setup mod list
  if (asn1_response->srbs_setup_mod_list_present) {
    for (auto asn1_srbs_setup_mod_list_item : asn1_response->srbs_setup_mod_list) {
      auto& asn1_srbs_setup_mod_item = asn1_srbs_setup_mod_list_item.value().srbs_setup_mod_item();
      res.srbs_setup_mod_list.push_back(asn1_to_f1ap_srbs_setup_mod_item(asn1_srbs_setup_mod_item));
    }
  }

  // Add SRBs modified list
  if (asn1_response->srbs_modified_list_present) {
    for (auto asn1_srbs_modified_list_item : asn1_response->srbs_modified_list) {
      auto& asn1_srbs_modified_item = asn1_srbs_modified_list_item.value().srbs_modified_item();
      res.srbs_modified_list.push_back(asn1_to_f1ap_srbs_setup_mod_item(asn1_srbs_modified_item));
    }
  }

  // Add full configuration
  if (asn1_response->full_cfg_present) {
    res.full_cfg = asn1_response->full_cfg.to_string();
  }
}

static void fill_f1ap_ue_context_modification_response(f1ap_ue_context_modification_response&   res,
                                                       const asn1::f1ap::ue_context_mod_fail_s& asn1_fail)
{
  res.success = false;
  res.cause   = asn1_to_cause(asn1_fail->cause);
  if (asn1_fail->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }
}
