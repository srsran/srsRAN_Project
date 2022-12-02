/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_ue_context_modification_procedure.h"

using namespace srsgnb;
using namespace srs_du;
using namespace asn1::f1ap;

f1ap_du_ue_context_modification_procedure::f1ap_du_ue_context_modification_procedure(
    const asn1::f1ap::ue_context_mod_request_s& msg,
    f1ap_du_ue&                                 ue_) :
  ue(ue_)
{
  // Construct DU request.
  du_request.ue_index = ue.context.ue_index;
  for (const auto& srb : msg->srbs_to_be_setup_mod_list.value) {
    du_request.srbs_to_addmod.push_back((srb_id_t)srb.value().srbs_to_be_setup_mod_item().srbid);
  }
  for (const auto& drb : msg->drbs_to_be_setup_mod_list.value) {
    drb_to_addmod drb_to_add;
    drb_to_add.drbid = (drb_id_t)drb.value().drbs_to_be_setup_mod_item().drbid;
    drb_to_add.lcid  = lcid_t::INVALID_LCID;
    du_request.drbs_to_addmod.push_back(drb_to_add);
  }
}

void f1ap_du_ue_context_modification_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Create GTPU bearers.
  // TODO.

  // Setup new UE configuration in DU.
  CORO_AWAIT_VALUE(du_response, ue.du_handler.request_ue_config_update(du_request));

  if (du_response.result) {
    send_ue_context_modification_response();
  } else {
    send_ue_context_modification_failure();
  }

  CORO_RETURN();
}

void f1ap_du_ue_context_modification_procedure::send_ue_context_modification_response()
{
  f1c_message f1c_msg;

  f1c_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_resp_s& resp = f1c_msg.pdu.successful_outcome().value.ue_context_mod_resp();

  resp->gnb_du_ue_f1_ap_id->value                   = gnb_du_ue_f1ap_id_to_uint(ue.context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1_ap_id->value                   = gnb_cu_ue_f1ap_id_to_uint(ue.context.gnb_cu_ue_f1ap_id);
  resp->res_coordination_transfer_container_present = false;
  resp->duto_currc_info_present                     = false;
  resp->drbs_setup_mod_list_present                 = not du_request.drbs_to_addmod.empty();
  if (resp->drbs_setup_mod_list_present) {
    resp->drbs_setup_mod_list.value.resize(du_request.drbs_to_addmod.size());
    for (unsigned i = 0; i != du_request.drbs_to_addmod.size(); ++i) {
      resp->drbs_setup_mod_list.value[i].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
      drbs_setup_mod_item_s& drb = resp->drbs_setup_mod_list.value[i]->drbs_setup_mod_item();
      drb.drbid                  = drb_id_to_uint(du_request.drbs_to_addmod[i].drbid);
      drb.lcid_present           = false;
      drb.dluptnl_info_to_be_setup_list.resize(1);
      drb.dluptnl_info_to_be_setup_list[0].dluptnl_info.set_gtp_tunnel();
      //        gtpu.transport_layer_address = ; // TODO
      //        gtpu.gtp_teid = ; // TODO
    }
  }
  resp->drbs_modified_list_present               = false;
  resp->srbs_failed_to_be_setup_mod_list_present = false;
  resp->drbs_failed_to_be_setup_mod_list_present = false;
  resp->scell_failedto_setup_mod_list_present    = false;
  resp->drbs_failed_to_be_modified_list_present  = false;
  resp->inactivity_monitoring_resp_present       = false;
  resp->crit_diagnostics_present                 = false;
  resp->c_rnti_present                           = false;
  resp->associated_scell_list_present            = false;
  resp->srbs_setup_mod_list_present              = not du_request.srbs_to_addmod.empty();
  if (resp->srbs_setup_mod_list_present) {
    resp->srbs_setup_mod_list.value.resize(du_request.srbs_to_addmod.size());
    for (unsigned i = 0; i != du_request.srbs_to_addmod.size(); ++i) {
      resp->srbs_setup_mod_list.value[i].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_MOD_ITEM);
      srbs_setup_mod_item_s& srb = resp->srbs_setup_mod_list.value[i].value().srbs_setup_mod_item();
      srb.srbid                  = srb_id_to_uint(du_request.srbs_to_addmod[i]);
      srb.lcid                   = srb_id_to_lcid(du_request.srbs_to_addmod[i]);
    }
  }
  resp->srbs_modified_list_present = false;
  resp->full_cfg_present           = false;

  ue.f1c_msg_notifier.on_new_message(f1c_msg);
}

void f1ap_du_ue_context_modification_procedure::send_ue_context_modification_failure()
{
  f1c_message f1c_msg;
  f1c_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_fail_s& resp = f1c_msg.pdu.unsuccessful_outcome().value.ue_context_mod_fail();

  resp->gnb_du_ue_f1_ap_id->value        = gnb_du_ue_f1ap_id_to_uint(ue.context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1_ap_id->value        = gnb_cu_ue_f1ap_id_to_uint(ue.context.gnb_cu_ue_f1ap_id);
  resp->cause->set_radio_network().value = asn1::f1ap::cause_radio_network_opts::unspecified;

  ue.f1c_msg_notifier.on_new_message(f1c_msg);
}
