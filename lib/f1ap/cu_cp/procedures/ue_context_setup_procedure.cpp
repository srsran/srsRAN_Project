/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ue_context_setup_procedure.h"
#include "../f1ap_asn1_converters.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::f1ap;

/// \brief Convert the UE Context Setup Request from common type to ASN.1.
/// \param[out] asn1_request The ASN.1 struct to store the result.
/// \param[in] request The common type UE Context Setup Request.
static void fill_asn1_ue_context_setup_request(asn1::f1ap::ue_context_setup_request_s& asn1_request,
                                               const f1ap_ue_context_setup_request&    request,
                                               const f1ap_ue_ids&                      ue_ids);

/// \brief Convert the UE Context Setup Failure from ASN.1 to common type.
/// \param[in] response The common type UE Context Setup Response.
/// \param[in] ue_index UE index.
/// \param[in] asn1_failure The ASN.1 struct to convert.
static void fill_f1ap_ue_context_setup_response(f1ap_ue_context_setup_response&            response,
                                                ue_index_t                                 ue_index,
                                                const asn1::f1ap::ue_context_setup_fail_s& asn1_failure);

/// \brief Convert the UE Context Setup Response from ASN.1 to common type.
/// \param[out] response  The common type struct to store the result.
/// \param[in] ue_index UE index.
/// \param[in] asn1_response The ASN.1 UE Context Setup Response.
static void fill_f1ap_ue_context_setup_response(f1ap_ue_context_setup_response&            response,
                                                ue_index_t                                 ue_index,
                                                const asn1::f1ap::ue_context_setup_resp_s& asn1_response);

// ---- UE Context Setup Procedure ----

ue_context_setup_procedure::ue_context_setup_procedure(const f1ap_ue_context_setup_request& request_,
                                                       f1ap_ue_context_list&                ue_ctxt_list_,
                                                       f1ap_du_processor_notifier&          du_processor_notifier_,
                                                       f1ap_message_notifier&               f1ap_notif_,
                                                       srslog::basic_logger&                logger_,
                                                       optional<rrc_ue_transfer_context>    rrc_context_) :
  request(request_),
  ue_ctxt_list(ue_ctxt_list_),
  du_processor_notifier(du_processor_notifier_),
  f1ap_notifier(f1ap_notif_),
  logger(logger_),
  rrc_context(rrc_context_)
{
  srsran_assert(request.ue_index != ue_index_t::invalid, "UE index of F1AP UeContextSetupRequest must not be invalid");
}

void ue_context_setup_procedure::operator()(coro_context<async_task<f1ap_ue_context_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} proc=\"{}\": started...", request.ue_index, name());

  // Create F1AP UE context if it doesn't exist.
  if (not find_or_create_f1ap_ue_context()) {
    CORO_EARLY_RETURN(handle_procedure_result());
  }

  // Subscribe to respective publisher to receive UE CONTEXT SETUP RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt->ev_mng.context_setup_outcome);

  // Send command to DU.
  send_ue_context_setup_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle result of the transaction.
  CORO_RETURN(handle_procedure_result());
}

bool ue_context_setup_procedure::find_or_create_f1ap_ue_context()
{
  // Check if F1AP UE context exists first.
  ue_ctxt = ue_ctxt_list.find(request.ue_index);
  if (ue_ctxt != nullptr) {
    logger.debug("{}: UE context found", f1ap_ue_log_prefix{ue_ctxt->ue_ids, name()});
    return true;
  }

  // F1AP UE context does not yet exist.
  // Allocate gNB-CU-UE-F1AP-ID.
  gnb_cu_ue_f1ap_id_t tmp_cu_ue_f1ap_id = ue_ctxt_list.next_gnb_cu_ue_f1ap_id();
  if (tmp_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::invalid) {
    logger.warning("ue={} proc=\"{}\": No CU UE F1AP ID available", request.ue_index, name());
    return false;
  }

  // Create F1AP UE context.
  ue_ctxt = &ue_ctxt_list.add_ue(request.ue_index, tmp_cu_ue_f1ap_id);
  logger.info("{}: UE successfully created.", f1ap_ue_log_prefix{ue_ctxt->ue_ids, name()});

  return true;
}

bool ue_context_setup_procedure::create_ue_rrc_context(const f1ap_ue_context_setup_response& ue_ctxt_setup_resp)
{
  if (not ue_ctxt_setup_resp.success or ue_ctxt_setup_resp.ue_index == ue_index_t::invalid) {
    logger.warning("Couldn't create UE in target cell");
    return false;
  }

  if (ue_ctxt_setup_resp.c_rnti.has_value()) {
    // An C-RNTI has been allocated by the DU. In such case, we need to create a new UE RRC context in the CU-CP.

    ue_rrc_context_creation_request req;
    req.ue_index               = ue_ctxt_setup_resp.ue_index;
    req.c_rnti                 = ue_ctxt_setup_resp.c_rnti.value();
    req.cgi                    = request.sp_cell_id;
    req.du_to_cu_rrc_container = ue_ctxt_setup_resp.du_to_cu_rrc_info.cell_group_cfg.copy();
    req.prev_context           = std::move(rrc_context);

    ue_rrc_context_creation_response resp = du_processor_notifier.on_ue_rrc_context_creation_request(req);
    if (resp.f1ap_rrc_notifier == nullptr) {
      logger.warning("Couldn't create UE RRC context in target cell");
      return false;
    }

    // Add RRC notifier to F1AP UE context.
    ue_ctxt_list.add_rrc_notifier(req.ue_index, resp.f1ap_rrc_notifier);

    logger.debug("ue={} Added RRC UE notifier", req.ue_index);
  }

  return true;
}

void ue_context_setup_procedure::send_ue_context_setup_request()
{
  // Pack message into PDU
  f1ap_message f1ap_ue_ctxt_setup_request_msg;
  f1ap_ue_ctxt_setup_request_msg.pdu.set_init_msg();
  f1ap_ue_ctxt_setup_request_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& req = f1ap_ue_ctxt_setup_request_msg.pdu.init_msg().value.ue_context_setup_request();

  // Convert common type to asn1
  fill_asn1_ue_context_setup_request(req, request, ue_ctxt->ue_ids);

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1ap_ue_ctxt_setup_request_msg.pdu.to_json(js);
    logger.debug("Containerized UeContextSetupRequest: {}", js.to_string());
  }

  // send UE context setup request message
  f1ap_notifier.on_new_message(f1ap_ue_ctxt_setup_request_msg);
}

f1ap_ue_context_setup_response ue_context_setup_procedure::handle_procedure_result()
{
  f1ap_ue_context_setup_response resp;
  resp.ue_index = request.ue_index;
  resp.success  = false;

  if (ue_ctxt == nullptr) {
    logger.warning("ue={} proc=\"{}\" failed", request.ue_index, name());
    return resp;
  }

  if (transaction_sink.successful()) {
    const auto& asn1_resp = transaction_sink.response();

    // Update gNB DU F1AP ID in F1AP UE context.
    ue_ctxt->ue_ids.du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(asn1_resp->gnb_du_ue_f1ap_id);
    logger.debug("{}: Updated UE gNB-DU-UE-ID", f1ap_ue_log_prefix{ue_ctxt->ue_ids, name()});

    // Fill response to the UE context setup procedure.
    fill_f1ap_ue_context_setup_response(resp, request.ue_index, transaction_sink.response());

    // Create UE RRC context in CU-CP, if required.
    resp.success = create_ue_rrc_context(resp);

    return resp;
  }

  // Procedure failed.

  if (transaction_sink.failed()) {
    logger.warning("{}: Procedure failed. Cause: {}",
                   f1ap_ue_log_prefix{ue_ctxt->ue_ids, name()},
                   get_cause_str(transaction_sink.failure()->cause));

    // Fill response to the UE context setup procedure.
    fill_f1ap_ue_context_setup_response(resp, request.ue_index, transaction_sink.failure());

  } else {
    logger.warning("{}: Procedure failed. Cause: Timeout reached while waiting for DU response.",
                   f1ap_ue_log_prefix{ue_ctxt->ue_ids, name()});
  }

  // Delete created F1AP UE context.
  ue_ctxt_list.remove_ue(request.ue_index);

  return resp;
}

static void fill_asn1_ue_context_setup_request(asn1::f1ap::ue_context_setup_request_s& asn1_request,
                                               const f1ap_ue_context_setup_request&    request,
                                               const f1ap_ue_ids&                      ue_ids)
{
  asn1_request->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ue_ids.cu_ue_f1ap_id);

  asn1_request->gnb_du_ue_f1ap_id_present = ue_ids.du_ue_f1ap_id != gnb_du_ue_f1ap_id_t::invalid;
  if (asn1_request->gnb_du_ue_f1ap_id_present) {
    asn1_request->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ue_ids.du_ue_f1ap_id);
  }

  asn1_request->sp_cell_id    = nr_cgi_to_f1ap_asn1(request.sp_cell_id);
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

static void fill_f1ap_ue_context_setup_response(f1ap_ue_context_setup_response&            response,
                                                ue_index_t                                 ue_index,
                                                const asn1::f1ap::ue_context_setup_fail_s& asn1_failure)
{
  response.success  = false;
  response.ue_index = ue_index;

  // cause
  response.cause = asn1_to_cause(asn1_failure->cause);

  // potential sp cell list
  if (asn1_failure->potential_sp_cell_list_present) {
    f1ap_potential_sp_cell_item potential_sp_cell_item;

    for (const auto& asn1_potential_sp_cell_item : asn1_failure->potential_sp_cell_list) {
      potential_sp_cell_item.potential_sp_cell_id =
          f1ap_asn1_to_nr_cgi(asn1_potential_sp_cell_item->potential_sp_cell_item().potential_sp_cell_id);

      response.potential_sp_cell_list.push_back(potential_sp_cell_item);
    }
  }
}

static void fill_f1ap_ue_context_setup_response(f1ap_ue_context_setup_response&            response,
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
            asn1_to_cause(asn1_scell_failed_to_setup_item->scell_failedto_setup_item().cause);
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
