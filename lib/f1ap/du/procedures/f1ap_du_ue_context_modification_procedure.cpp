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

#include "f1ap_du_ue_context_modification_procedure.h"
#include "../../asn1_helpers.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"

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
    auto* srb = ue.bearers.find_srb(srb_id_t::srb1);
    if (srb != nullptr) {
      srb->handle_pdu(req->rrc_container.copy());
    } else {
      du_response.result = false;
    }
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
    // >> Pass UE capabilities.
    du_request.ue_cap_rat_list = msg->cu_to_du_rrc_info.ue_cap_rat_container_list.copy();
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

  // > DU-to-CU RRC Container.
  if (not du_response.du_to_cu_rrc_container.empty()) {
    resp->du_to_cu_rrc_info_present        = true;
    resp->du_to_cu_rrc_info.cell_group_cfg = du_response.du_to_cu_rrc_container.copy();
  }

  // > Full Config IE.
  if (du_response.full_config_present) {
    resp->full_cfg_present = true;
    resp->full_cfg.value   = asn1::f1ap::full_cfg_opts::full;
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
