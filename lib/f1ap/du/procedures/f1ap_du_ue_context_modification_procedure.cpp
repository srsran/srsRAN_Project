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

#include "f1ap_du_ue_context_modification_procedure.h"
#include "f1ap_du_ue_context_common.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

f1ap_du_ue_context_modification_procedure::f1ap_du_ue_context_modification_procedure(
    const asn1::f1ap::ue_context_mod_request_s& msg,
    f1ap_du_ue&                                 ue_) :
  ue(ue_), logger(srslog::fetch_basic_logger("F1AP-DU"))
{
  create_du_request(msg);
}

void f1ap_du_ue_context_modification_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Setup new UE configuration in DU.
  CORO_AWAIT_VALUE(du_response, ue.du_handler.request_ue_context_update(du_request));

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
    du_request.srbs_to_setup.push_back(make_srb_id(srb.value().srbs_to_be_setup_mod_item()));
  }

  // >> Pass DRBs to setup/modify.
  for (const auto& drb : msg->drbs_to_be_setup_mod_list) {
    du_request.drbs_to_setup.push_back(make_drb_to_setup(drb.value().drbs_to_be_setup_mod_item()));
  }

  // >> Pass DRBs to remove
  for (const auto& drb : msg->drbs_to_be_released_list) {
    du_request.drbs_to_rem.push_back(make_drb_id(drb.value().drbs_to_be_released_item()));
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

  // > DRBs-SetupMod-List.
  resp->drbs_setup_mod_list_present = not du_response.drbs_setup.empty();
  resp->drbs_setup_mod_list.resize(du_response.drbs_setup.size());
  for (unsigned i = 0; i != du_response.drbs_setup.size(); ++i) {
    resp->drbs_setup_mod_list[i].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
    const f1ap_drb_setup&  drb_setup = du_response.drbs_setup[i];
    drbs_setup_mod_item_s& asn1_drb  = resp->drbs_setup_mod_list[i]->drbs_setup_mod_item();
    asn1_drb.drb_id                  = drb_id_to_uint(du_request.drbs_to_setup[i].drb_id);
    asn1_drb.lcid_present            = drb_setup.lcid.has_value();
    if (asn1_drb.lcid_present) {
      asn1_drb.lcid = drb_setup.lcid.value();
    }
    asn1_drb.dl_up_tnl_info_to_be_setup_list.resize(drb_setup.dluptnl_info_list.size());
    for (unsigned j = 0; j != drb_setup.dluptnl_info_list.size(); ++j) {
      up_transport_layer_info_to_asn1(asn1_drb.dl_up_tnl_info_to_be_setup_list[j].dl_up_tnl_info,
                                      drb_setup.dluptnl_info_list[j]);
    }
  }
  resp->drbs_modified_list_present               = false;
  resp->srbs_failed_to_be_setup_mod_list_present = false;
  // > DRBs-FailedToBeSetupMod-List.
  resp->drbs_failed_to_be_setup_mod_list_present = not du_response.drbs_failed_to_setup.empty();
  resp->drbs_failed_to_be_setup_mod_list.resize(du_response.drbs_failed_to_setup.size());
  for (unsigned i = 0; i != du_response.drbs_failed_to_setup.size(); ++i) {
    resp->drbs_failed_to_be_setup_mod_list[i].load_info_obj(ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_MOD_ITEM);
    drbs_failed_to_be_setup_mod_item_s& asn1_drb =
        resp->drbs_failed_to_be_setup_mod_list[i]->drbs_failed_to_be_setup_mod_item();
    asn1_drb.drb_id                      = drb_id_to_uint(du_response.drbs_failed_to_setup[i]);
    asn1_drb.cause.set_transport().value = cause_transport_opts::transport_res_unavailable;
  }
  resp->scell_failedto_setup_mod_list_present   = false;
  resp->drbs_failed_to_be_modified_list_present = false;
  resp->inactivity_monitoring_resp_present      = false;
  resp->crit_diagnostics_present                = false;
  resp->c_rnti_present                          = false;
  resp->associated_scell_list_present           = false;

  // > SRBs-SetupMod-List.
  resp->srbs_setup_mod_list_present = not du_request.srbs_to_setup.empty();
  resp->srbs_setup_mod_list.resize(du_request.srbs_to_setup.size());
  for (unsigned i = 0; i != du_request.srbs_to_setup.size(); ++i) {
    resp->srbs_setup_mod_list[i].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_MOD_ITEM);
    srbs_setup_mod_item_s& srb = resp->srbs_setup_mod_list[i].value().srbs_setup_mod_item();
    srb.srb_id                 = srb_id_to_uint(du_request.srbs_to_setup[i]);
    srb.lcid                   = srb_id_to_lcid(du_request.srbs_to_setup[i]);
  }
  resp->srbs_modified_list_present = false;
  resp->full_cfg_present           = false;

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
