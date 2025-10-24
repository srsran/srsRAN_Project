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

#include "f1ap_du_ue_context_setup_procedure.h"
#include "../../asn1_helpers.h"
#include "../ue_context/f1ap_du_ue_manager.h"
#include "proc_logger.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

/// Time waiting for RRC container delivery.
static constexpr std::chrono::milliseconds rrc_container_delivery_timeout{120};

static bool requires_ue_ran_config_update(const ue_context_setup_request_s& msg)
{
  return msg->srbs_to_be_setup_list_present or msg->drbs_to_be_setup_list_present or
         msg->scell_to_be_setup_list_present;
}

static expected<default_success_t, asn1::f1ap::cause_c>
validate_drb_to_be_setup_item(const asn1::f1ap::drbs_to_be_setup_item_s& drb_to_be_setup_item)
{
  asn1::f1ap::cause_c cause;
  cause.set_protocol().value = asn1::f1ap::cause_protocol_opts::semantic_error;

  // Check DL PDCP SN length information present.
  // DL PDCP SN information is mandatory in the IE extension field.
  if (not drb_to_be_setup_item.ie_exts_present) {
    return make_unexpected(cause);
  }
  return default_success_t{};
}

f1ap_du_ue_context_setup_procedure::f1ap_du_ue_context_setup_procedure(
    const asn1::f1ap::ue_context_setup_request_s& msg_,
    f1ap_du_ue_manager&                           ue_mng_,
    f1ap_du_configurator&                         du_mng_,
    du_ue_index_t                                 ue_index_,
    const f1ap_du_context&                        ctxt_) :
  msg(msg_),
  ue_mng(ue_mng_),
  du_mng(du_mng_),
  ue_index(ue_index_),
  logger(srslog::fetch_basic_logger("DU-F1")),
  du_ctxt(ctxt_)
{
}

void f1ap_du_ue_context_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Find the cell index from the NR-CGI.
  sp_cell_index =
      get_cell_index_from_nr_cgi({plmn_identity::from_bytes(msg->sp_cell_id.plmn_id.to_bytes()).value(),
                                  nr_cell_identity::create(msg->sp_cell_id.nr_cell_id.to_number()).value()});
  if (not sp_cell_index.has_value()) {
    // Failed to create UE context in the DU.
    logger.warning("{}: Failed to to find spCell with PLMN '{}' and NCI '{}' in DU.",
                   f1ap_log_prefix{int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id), name()},
                   plmn_identity::from_bytes(msg->sp_cell_id.plmn_id.to_bytes()).value(),
                   nr_cell_identity::create(msg->sp_cell_id.nr_cell_id.to_number()).value());
    send_ue_context_setup_failure();
    CORO_EARLY_RETURN();
  }

  if (msg->gnb_du_ue_f1ap_id_present) {
    const gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id);
    const gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id);

    logger.debug("{}: Starting procedure...", f1ap_log_prefix{gnb_du_ue_f1ap_id, gnb_cu_ue_f1ap_id, name()});

    ue = ue_mng.find(gnb_du_ue_f1ap_id);
    if (ue == nullptr) {
      logger.warning("{}: Discarding UeContextSetupRequest. Cause: Unrecognized GNB-DU-UE-F1AP-ID.",
                     f1ap_log_prefix{gnb_du_ue_f1ap_id, gnb_cu_ue_f1ap_id, name()});
      send_ue_context_setup_failure();
      CORO_EARLY_RETURN();
    }

    if (requires_ue_ran_config_update(msg)) {
      // Set that the UE has a pending configuration.
      ue->context.rrc_state = f1ap_ue_context::ue_rrc_state::config_pending;
    }

  } else {
    // [TS38.473, 8.3.1.2] If no UE-associated logical F1-connection exists, the UE-associated logical F1-connection
    // shall be established as part of the procedure.
    // Request the creation of a new UE context in the DU.
    CORO_AWAIT_VALUE(du_ue_create_response,
                     du_mng.request_ue_creation(f1ap_ue_context_creation_request{ue_index, sp_cell_index.value()}));
    if (not du_ue_create_response->result) {
      // Failed to create UE context in the DU.
      logger.warning("{}: Failed to allocate new UE context in DU.",
                     f1ap_log_prefix{int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id), name()});
      send_ue_context_setup_failure();
      CORO_EARLY_RETURN();
    }

    // Fetch created UE context.
    ue = ue_mng.find(ue_index);
    srsran_assert(ue != nullptr, "UE context was not correctly created");

    // Save allocated C-RNTI for created UE.
    ue->context.rnti = du_ue_create_response->crnti;

    // Set that the UE has a pending configuration (provided by the ReconfigWithSync).
    ue->context.rrc_state = f1ap_ue_context::ue_rrc_state::config_pending;
  }

  if (ue->context.gnb_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::invalid) {
    // If this is the first message with a GNB-CU-UE-F1AP-ID, update the UE context.
    ue->context.gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id);
  }

  // Setup new UE configuration in DU.
  CORO_AWAIT_VALUE(du_ue_cfg_response, request_du_ue_config());
  if (not du_ue_cfg_response.result) {
    // Clear up resources allocated during the procedure.
    if (du_ue_create_response.has_value() and du_ue_cfg_response.result) {
      CORO_AWAIT(du_mng.request_ue_removal(f1ap_ue_delete_request{ue_index}));
    }
    send_ue_context_setup_failure();
    CORO_EARLY_RETURN();
  }

  // > If the UE CONTEXT SETUP REQUEST message contains the RRC-Container IE, the gNB-DU shall send the corresponding
  // RRC message to the UE via SRB1.
  if (msg->rrc_container_present and not msg->rrc_container.empty()) {
    CORO_AWAIT_VALUE(bool ret, handle_rrc_container());
    if (ret) {
      logger.debug("{}: RRC container sent successfully.", f1ap_log_prefix{ue->context, name()});
    } else {
      const f1ap_du_cell_context& cell_ctx = du_ctxt.served_cells[sp_cell_index.value()];
      std::chrono::milliseconds   timeout  = rrc_container_delivery_timeout + cell_ctx.ntn_link_rtt;
      logger.error("{}: Failed to send RRC container after timeout of {}msec",
                   f1ap_log_prefix{ue->context, name()},
                   timeout.count());
      send_ue_context_setup_failure();
      CORO_EARLY_RETURN();
    }
  }

  // Respond back to CU-CP with success.
  send_ue_context_setup_response();

  CORO_RETURN();
}

async_task<bool> f1ap_du_ue_context_setup_procedure::handle_rrc_container()
{
  f1c_bearer* srb1 = ue->bearers.find_srb(srb_id_t::srb1);
  if (srb1 == nullptr) {
    logger.error("{}: Failed to find SRB1 bearer to send RRC container.", f1ap_log_prefix{ue->context, name()});
    return launch_no_op_task(false);
  }
  const f1ap_du_cell_context& cell_ctx = du_ctxt.served_cells[sp_cell_index.value()];
  std::chrono::milliseconds   timeout  = rrc_container_delivery_timeout + cell_ctx.ntn_link_rtt;
  return srb1->handle_pdu_and_await_transmission(
      msg->rrc_container.copy(), msg->rrc_delivery_status_request_present, timeout);
}

expected<du_cell_index_t>
f1ap_du_ue_context_setup_procedure::get_cell_index_from_nr_cgi(nr_cell_global_id_t nr_cgi) const
{
  // Find the spCell index in the F1AP DU context.
  const auto* cell = du_ctxt.find_cell(nr_cgi);
  if (cell != nullptr) {
    return cell->cell_index;
  }

  return make_unexpected(default_error_t());
}

async_task<f1ap_ue_context_update_response> f1ap_du_ue_context_setup_procedure::request_du_ue_config()
{
  // Construct DU request.
  f1ap_ue_context_update_request du_request = {};
  du_request.ue_index                       = ue->context.ue_index;

  auto plmn = plmn_identity::from_bytes(msg->sp_cell_id.plmn_id.to_bytes());
  auto nci  = nr_cell_identity::create(msg->sp_cell_id.nr_cell_id.to_number());
  du_request.spcell_id.emplace(plmn.value(), nci.value());

  // > Set whether full configuration is required.
  // [TS 38.473, section 8.3.1.1] If the received CU to DU RRC Information IE does not include source cell group
  // configuration, the gNB-DU shall generate the cell group configuration using full configuration. Otherwise,
  // delta configuration is allowed.
  du_request.full_config_required =
      not msg->cu_to_du_rrc_info.ie_exts_present or not msg->cu_to_du_rrc_info.ie_exts.cell_group_cfg_present;

  // > Pass SRBs to setup.
  for (const auto& srb : msg->srbs_to_be_setup_list) {
    du_request.srbs_to_setup.push_back(int_to_srb_id(srb.value().srbs_to_be_setup_item().srb_id));
  }

  // > Pass DRBs to setup.
  for (const auto& drb : msg->drbs_to_be_setup_list) {
    expected<default_success_t, asn1::f1ap::cause_c> drb_valid =
        validate_drb_to_be_setup_item(drb->drbs_to_be_setup_item());
    if (drb_valid.has_value()) {
      du_request.drbs_to_setup.push_back(make_drb_to_setup(drb.value().drbs_to_be_setup_item()));
    } else {
      f1ap_drb_failed_to_setupmod failed_drb = {uint_to_drb_id(drb->drbs_to_be_setup_item().drb_id),
                                                asn1_to_cause(drb_valid.error())};
      failed_drbs.push_back(failed_drb);
    }
  }

  // > measConfig IE.
  // [TS 38.473, 8.3.1.2] If the MeasConfig IE is included in the CU to DU RRC Information IE in the UE CONTEXT SETUP
  // REQUEST message, the gNB-DU shall deduce that changes to the measurements configuration need to be applied.
  du_request.meas_cfg = msg->cu_to_du_rrc_info.meas_cfg.copy();

  if (msg->cu_to_du_rrc_info.ie_exts_present) {
    // > Add HO preparation information in case of Handover.
    if (msg->cu_to_du_rrc_info.ie_exts.ho_prep_info_present) {
      du_request.ho_prep_info = msg->cu_to_du_rrc_info.ie_exts.ho_prep_info.copy();
    }

    // > Add source cell group config for delta configuration.
    if (msg->cu_to_du_rrc_info.ie_exts.cell_group_cfg_present) {
      du_request.source_cell_group_cfg = msg->cu_to_du_rrc_info.ie_exts.cell_group_cfg.copy();
    }
  }

  // > Add UE capabilities information.
  if (not msg->cu_to_du_rrc_info.ue_cap_rat_container_list.empty()) {
    du_request.ue_cap_rat_list = msg->cu_to_du_rrc_info.ue_cap_rat_container_list.copy();
  }

  // > servingCellMO IE.
  // [TS 38.473, 8.3.1.2] If the servingCellMO IE is included in the UE CONTEXT SETUP REQUEST message, the gNB-DU shall
  // configure the servingCellMO for the indicated SpCell accordingly.
  if (msg->serving_cell_mo_present) {
    du_request.serving_cell_mo.emplace(msg->serving_cell_mo);
  }

  // > servingCellMO List IE.
  // [TS 38.473, 8.3.1.2] If the servingCellMO List IE is included in the UE CONTEXT SETUP REQUEST message, the gNB-DU
  // shall, if supported, select servingCellMO after determining the list of BWPs for the UE and include the list of
  // servingCellMOs that have been encoded in CellGroupConfig IE as ServingCellMO-encoded-in-CGC List IE in the UE
  // CONTEXT SETUP RESPONSE message.
  if (msg->serving_cell_mo_list_present) {
    auto& list = du_request.serving_cell_mo_list.emplace(msg->serving_cell_mo_list.size());
    for (const auto& item : msg->serving_cell_mo_list) {
      list.emplace_back(f1ap_serving_cell_mo_list_item{item->serving_cell_mo_list_item().serving_cell_mo,
                                                       item->serving_cell_mo_list_item().ssb_freq});
    }
  }

  return ue->du_handler.request_ue_context_update(du_request);
}

void f1ap_du_ue_context_setup_procedure::send_ue_context_setup_response()
{
  // Prepare UE CONTEXT SETUP RESPONSE message.
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_resp_s& resp = f1ap_msg.pdu.successful_outcome().value.ue_context_setup_resp();

  resp->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ue->context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ue->context.gnb_cu_ue_f1ap_id);

  // Prepare DU to CU RRC Container.
  // > cellGroupConfig
  resp->du_to_cu_rrc_info.cell_group_cfg = du_ue_cfg_response.cell_group_cfg.copy();
  // > measGapConfig
  resp->du_to_cu_rrc_info.meas_gap_cfg = du_ue_cfg_response.meas_gap_cfg.copy();

  // > Check if DU-to-CU RRC Container is a full cellGroupConfig or a delta.
  if (du_ue_cfg_response.full_config_present) {
    resp->full_cfg_present = true;
    resp->full_cfg.value   = asn1::f1ap::full_cfg_opts::full;
  }

  // > If the C-RNTI IE is included in the UE CONTEXT SETUP RESPONSE, the gNB-CU shall consider that the C-RNTI has
  // been allocated by the gNB-DU for this UE context.
  if (du_ue_create_response.has_value()) {
    resp->c_rnti_present = true;
    resp->c_rnti         = to_value(du_ue_create_response->crnti);
  }

  // > SRBs setup list.
  resp->srbs_setup_list_present = msg->srbs_to_be_setup_list_present;
  if (resp->srbs_setup_list_present) {
    resp->srbs_setup_list.resize(msg->srbs_to_be_setup_list.size());
    for (unsigned i = 0; i != resp->srbs_setup_list.size(); ++i) {
      resp->srbs_setup_list[i].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
      const auto&        srb_req_item = msg->srbs_to_be_setup_list[i]->srbs_to_be_setup_item();
      srbs_setup_item_s& srb_item     = resp->srbs_setup_list[i].value().srbs_setup_item();
      srb_item.srb_id                 = srb_req_item.srb_id;
      srb_item.lcid                   = srb_id_to_lcid(int_to_srb_id(srb_req_item.srb_id));
    }
  }

  // > DRBs setup List.
  resp->drbs_setup_list         = make_drbs_setup_list(du_ue_cfg_response.drbs_setup);
  resp->drbs_setup_list_present = resp->drbs_setup_list.size() > 0;
  // > DRBs-FailedToBeSetup-List.
  failed_drbs.insert(
      failed_drbs.end(), du_ue_cfg_response.failed_drbs_setups.begin(), du_ue_cfg_response.failed_drbs_setups.end());
  resp->drbs_failed_to_be_setup_list         = make_drbs_failed_to_be_setup_list(failed_drbs);
  resp->drbs_failed_to_be_setup_list_present = resp->drbs_failed_to_be_setup_list.size() > 0;

  if (msg->serving_cell_mo_list_present) {
    // > ServingCellMO-encoded-in-CGC List.
    resp->serving_cell_mo_encoded_in_cgc_list_present = true;
    resp->serving_cell_mo_encoded_in_cgc_list =
        make_serving_cell_mo_encoded_in_cgc_list(du_ue_cfg_response.serving_cell_mo_encoded_in_cgc_list);
  }

  // Send Response to CU-CP.
  ue->f1ap_msg_notifier.on_new_message(f1ap_msg);

  logger.debug("{}: Procedure finished successfully.", f1ap_log_prefix{ue->context, name()});
}

void f1ap_du_ue_context_setup_procedure::send_ue_context_setup_failure()
{
  // Prepare UE CONTEXT SETUP FAILURE message.
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_fail_s& resp = f1ap_msg.pdu.unsuccessful_outcome().value.ue_context_setup_fail();

  resp->gnb_cu_ue_f1ap_id = msg->gnb_cu_ue_f1ap_id;
  if (ue != nullptr) {
    resp->gnb_du_ue_f1ap_id_present = true;
    resp->gnb_du_ue_f1ap_id         = gnb_du_ue_f1ap_id_to_uint(ue->context.gnb_du_ue_f1ap_id);
  }

  resp->cause.set_radio_network().value = asn1::f1ap::cause_radio_network_opts::no_radio_res_available;

  // Send UE CONTEXT SETUP FAILURE to CU-CP.
  if (ue != nullptr) {
    ue->f1ap_msg_notifier.on_new_message(f1ap_msg);
  }

  logger.debug("{}: Procedure finished with failure.",
               ue == nullptr ? f1ap_log_prefix{int_to_gnb_cu_ue_f1ap_id(resp->gnb_cu_ue_f1ap_id), name()}
                             : f1ap_log_prefix{ue->context, name()});
}
