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

#include "f1ap_du_ue_context_modification_procedure.h"
#include "../../asn1_helpers.h"
#include "proc_logger.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

f1ap_du_ue_context_modification_procedure::f1ap_du_ue_context_modification_procedure(
    const asn1::f1ap::ue_context_mod_request_s& msg,
    f1ap_du_ue&                                 ue_) :
  req(msg), ue(ue_), logger(srslog::fetch_basic_logger("DU-F1"))
{
}

void f1ap_du_ue_context_modification_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Modify UE configuration in DU.
  create_du_request(req);
  CORO_AWAIT_VALUE(du_response, ue.du_handler.request_ue_context_update(du_request));

  // Set that the UE has a pending RRC configuration.
  ue.context.rrc_state = f1ap_ue_context::ue_rrc_state::config_pending;

  // "If the UE CONTEXT MODIFICATION REQUEST message contains the RRC-Container IE, the gNB-DU shall send the
  // corresponding RRC message to the UE."
  if (du_response.result and req->rrc_container_present) {
    CORO_AWAIT_VALUE(bool result, handle_rrc_container());
    if (not result and req->rrc_delivery_status_request_present) {
      // Note: Even if lower layers do not notify RRC container delivery, the procedure moves forward. We can rely on
      // RLC RLF in case something goes wrong.
      logger.warning(
          "{}: Ignoring RRC Delivery status request. Cause: Lower layers have not confirmed RRC container delivery",
          f1ap_log_prefix{ue.context, name()});
    }
  }

  if (req->tx_action_ind_present) {
    // "If the UE CONTEXT MODIFICATION REQUEST message contains the Transmission Action Indicator IE, the gNB-DU
    // shall stop or restart (if already stopped) data transmission for the UE, according to the value of this IE.
    // It is up to gNB-DU implementation when to stop or restart the UE scheduling.
    CORO_AWAIT(handle_tx_action_indicator());
  }

  if (du_response.result) {
    send_ue_context_modification_response();
  } else {
    send_ue_context_modification_failure();
  }

  CORO_RETURN();
}

void f1ap_du_ue_context_modification_procedure::create_du_request(const asn1::f1ap::ue_context_mod_request_s& msg)
{
  // Construct DU request.
  du_request.ue_index = ue.context.ue_index;

  if (msg->sp_cell_id_present) {
    // > [TS 38.473, 8.3.4.2] "If the SpCell ID IE is included in the UE CONTEXT MODIFICATION REQUEST message, the
    // gNB-DU shall replace any previously received value and regard it as a reconfiguration with sync as defined in
    // TS 38.331"
    auto plmn = plmn_identity::from_bytes(msg->sp_cell_id.plmn_id.to_bytes());
    auto nci  = nr_cell_identity::create(msg->sp_cell_id.nr_cell_id.to_number());
    du_request.spcell_id.emplace(plmn.value(), nci.value());
  }

  // > Set whether full configuration is required.
  // [TS 38.473, section 8.3.4.2] If the Full Configuration IE is contained in the UE CONTEXT MODIFICATION REQUEST
  // message, the gNB-DU shall generate a CellGroupConfig IE using full configuration and include it in the UE CONTEXT
  // MODIFICATION RESPONSE.
  du_request.full_config_required = msg->full_cfg_present;

  // >> Pass SRBs to setup/modify.
  for (const auto& srb : msg->srbs_to_be_setup_mod_list) {
    du_request.srbs_to_setup.push_back(int_to_srb_id(srb.value().srbs_to_be_setup_mod_item().srb_id));
  }

  // >> Pass DRBs to setup.
  for (const auto& drb : msg->drbs_to_be_setup_mod_list) {
    du_request.drbs_to_setup.push_back(make_drb_to_setup(drb.value().drbs_to_be_setup_mod_item()));
  }

  // >> Pass DRBs to modify.
  // Note: This field is used during RRC Reestablishment.
  for (const auto& drb : msg->drbs_to_be_modified_list) {
    du_request.drbs_to_mod.push_back(make_drb_to_modify(drb.value().drbs_to_be_modified_item()));
  }

  // >> Pass DRBs to remove
  for (const auto& drb : msg->drbs_to_be_released_list) {
    du_request.drbs_to_rem.push_back(get_drb_id(drb.value().drbs_to_be_released_item()));
  }

  if (msg->cu_to_du_rrc_info_present) {
    // >> measConfig IE.
    // [TS 38.473, 8.3.4.2] If the MeasConfig IE is included in the CU to DU RRC Information IE in the UE CONTEXT
    // MODIFICATION REQUEST message, the gNB-DU shall deduce that changes to the measurements’ configuration need to be
    // applied.
    du_request.meas_cfg = msg->cu_to_du_rrc_info.meas_cfg.copy();

    // >> Pass UE capabilities.
    du_request.ue_cap_rat_list = msg->cu_to_du_rrc_info.ue_cap_rat_container_list.copy();
  }

  if (msg->rrc_recfg_complete_ind_present) {
    // >>RRC Reconfiguration Complete Indicator IE.
    // [TS 38.473, 9.3.1.30] This IE indicates the result of the reconfiguration performed towards the UE.
    du_request.rrc_recfg_complete_ind =
        msg->rrc_recfg_complete_ind == asn1::f1ap::rrc_recfg_complete_ind_opts::options::true_value;
  } else {
    du_request.rrc_recfg_complete_ind = false;
  }

  if (msg->serving_cell_mo_present) {
    // >> servingCellMO IE.
    // [TS 38.473, 8.3.4.2]  If the servingCellMO IE is included in the UE CONTEXT MODIFICATION REQUEST message, the
    // gNB-DU shall configure servingCellMO for the indicated SpCell accordingly.
    du_request.serving_cell_mo.emplace(msg->serving_cell_mo);
  }

  if (msg->serving_cell_mo_list_present) {
    // > servingCellMO List IE.
    // [TS 38.473, 8.3.4.2] If the servingCellMO List IE is included in the UE CONTEXT MODIFICATION REQUEST message,
    // the gNB-DU shall, if supported, configure servingCellMO after determining the list of BWPs for the UE and
    // include the list of servingCellMOs that have been encoded in CellGroupConfig IE as ServingCellMO-encoded-in-CGC
    // List IE in the UE CONTEXT MODIFICATION RESPONSE message.
    auto& list = du_request.serving_cell_mo_list.emplace(msg->serving_cell_mo_list.size());
    for (const auto& item : msg->serving_cell_mo_list) {
      list.emplace_back(f1ap_serving_cell_mo_list_item{item->serving_cell_mo_list_item().serving_cell_mo,
                                                       item->serving_cell_mo_list_item().ssb_freq});
    }
  }
}

void f1ap_du_ue_context_modification_procedure::send_ue_context_modification_response()
{
  f1ap_message f1ap_msg;

  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_resp_s& resp = f1ap_msg.pdu.successful_outcome().value.ue_context_mod_resp();

  resp->gnb_du_ue_f1ap_id                           = gnb_du_ue_f1ap_id_to_uint(ue.context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1ap_id                           = gnb_cu_ue_f1ap_id_to_uint(ue.context.gnb_cu_ue_f1ap_id);
  resp->res_coordination_transfer_container_present = false;

  // DRBs-SetupMod-List
  resp->drbs_setup_mod_list         = make_drbs_setup_mod_list(du_response.drbs_setup);
  resp->drbs_setup_mod_list_present = resp->drbs_setup_mod_list.size() > 0;
  // DRBs-FailedToBeSetupMod-List
  resp->drbs_failed_to_be_setup_mod_list = make_drbs_failed_to_be_setup_mod_list(du_response.failed_drbs_setups);
  resp->drbs_failed_to_be_setup_mod_list_present = resp->drbs_failed_to_be_setup_mod_list.size() > 0;
  // DRBs-Modified-List
  resp->drbs_modified_list         = make_drbs_modified_list(du_response.drbs_mod);
  resp->drbs_modified_list_present = resp->drbs_modified_list.size() > 0;
  // DRBs-FailedToBeModified-List
  resp->drbs_failed_to_be_modified_list         = make_drbs_failed_to_be_modified_list(du_response.failed_drb_mods);
  resp->drbs_failed_to_be_modified_list_present = resp->drbs_failed_to_be_modified_list.size() > 0;

  resp->scell_failedto_setup_mod_list_present = false;
  resp->inactivity_monitoring_resp_present    = false;
  resp->crit_diagnostics_present              = false;
  resp->c_rnti_present                        = false;
  resp->associated_scell_list_present         = false;

  // > SRBs-SetupMod-List.
  resp->srbs_setup_mod_list         = make_srb_setupmod_list(du_request.srbs_to_setup);
  resp->srbs_setup_mod_list_present = not du_request.srbs_to_setup.empty();

  resp->srbs_failed_to_be_setup_mod_list_present = false;
  resp->srbs_modified_list_present               = false;
  resp->full_cfg_present                         = false;

  // > cellGroupConfig IE.
  if (not du_response.cell_group_cfg.empty()) {
    resp->du_to_cu_rrc_info_present        = true;
    resp->du_to_cu_rrc_info.cell_group_cfg = du_response.cell_group_cfg.copy();
  }

  // > measGapConfig IE.
  if (not du_response.meas_gap_cfg.empty()) {
    resp->du_to_cu_rrc_info_present      = true;
    resp->du_to_cu_rrc_info.meas_gap_cfg = du_response.meas_gap_cfg.copy();
  }

  // > Full Config IE.
  if (du_response.full_config_present) {
    resp->full_cfg_present = true;
    resp->full_cfg.value   = asn1::f1ap::full_cfg_opts::full;
  }

  if (req->serving_cell_mo_list_present) {
    // > ServingCellMO-encoded-in-CGC List.
    resp->serving_cell_mo_encoded_in_cgc_list_present = true;
    resp->serving_cell_mo_encoded_in_cgc_list =
        make_serving_cell_mo_encoded_in_cgc_list(du_response.serving_cell_mo_encoded_in_cgc_list);
  }

  ue.f1ap_msg_notifier.on_new_message(f1ap_msg);
}

void f1ap_du_ue_context_modification_procedure::send_ue_context_modification_failure()
{
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_fail_s& resp = f1ap_msg.pdu.unsuccessful_outcome().value.ue_context_mod_fail();

  resp->gnb_du_ue_f1ap_id               = gnb_du_ue_f1ap_id_to_uint(ue.context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1ap_id               = gnb_cu_ue_f1ap_id_to_uint(ue.context.gnb_cu_ue_f1ap_id);
  resp->cause.set_radio_network().value = asn1::f1ap::cause_radio_network_opts::unspecified;

  ue.f1ap_msg_notifier.on_new_message(f1ap_msg);
}

async_task<bool> f1ap_du_ue_context_modification_procedure::handle_rrc_container()
{
  // Time waiting for RRC container delivery.
  static constexpr std::chrono::milliseconds rrc_container_delivery_timeout{120};

  f1c_bearer* srb1 = ue.bearers.find_srb(srb_id_t::srb1);
  if (srb1 == nullptr) {
    logger.error("{}: Failed to find SRB1 bearer to send RRC container.", f1ap_log_prefix{ue.context, name()});
    return launch_no_op_task(false);
  }

  // If RRC delivery status is requested, we wait for the PDU delivery and report the status afterwards.
  return srb1->handle_pdu_and_await_transmission(
      req->rrc_container.copy(), req->rrc_delivery_status_request_present, rrc_container_delivery_timeout);
}

async_task<void> f1ap_du_ue_context_modification_procedure::handle_tx_action_indicator()
{
  if (req->tx_action_ind.value == asn1::f1ap::tx_action_ind_opts::stop) {
    return ue.du_handler.request_ue_drb_deactivation(ue.context.ue_index);
  }
  logger.error("{}: Ignoring Transmission Action Indicator IE with \"restart\" value. Cause: Feature not supported",
               f1ap_log_prefix{ue.context, name()});
  return launch_no_op_task();
}
