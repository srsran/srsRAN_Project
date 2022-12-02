/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_ue_context_setup_procedure.h"

using namespace srsgnb;
using namespace srs_du;
using namespace asn1::f1ap;

f1ap_du_ue_context_setup_procedure::f1ap_du_ue_context_setup_procedure(
    const asn1::f1ap::ue_context_setup_request_s& msg,
    f1ap_du_ue&                                   ue_) :
  ue(ue_)
{
  using namespace asn1::f1ap;

  // Construct DU request.
  du_request.ue_index = ue.context.ue_index;
  for (const auto& srb : msg->srbs_to_be_setup_list.value) {
    du_request.srbs_to_addmod.push_back((srb_id_t)srb.value().srbs_to_be_setup_item().srbid);
  }
  for (const auto& drb : msg->drbs_to_be_setup_list.value) {
    const drbs_to_be_setup_item_s& drb_item = drb.value().drbs_to_be_setup_item();

    drb_to_addmod drb_to_add;
    drb_to_add.drbid = (drb_id_t)drb_item.drbid;
    drb_to_add.lcid  = lcid_t::INVALID_LCID;
    du_request.drbs_to_addmod.push_back(drb_to_add);
  }
}

void f1ap_du_ue_context_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Create GTPU bearers if there are DRBs to setup.
  // TODO.

  // Setup new UE configuration in DU.
  CORO_AWAIT_VALUE(du_response, ue.du_handler.request_ue_config_update(du_request));

  if (du_response.result) {
    send_ue_context_setup_response();
  } else {
    send_ue_context_setup_failure();
  }

  CORO_RETURN();
}

void f1ap_du_ue_context_setup_procedure::send_ue_context_setup_response()
{
  f1c_message f1c_msg;
  f1c_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_resp_s& resp = f1c_msg.pdu.successful_outcome().value.ue_context_setup_resp();

  resp->gnb_du_ue_f1_ap_id->value = gnb_du_ue_f1ap_id_to_uint(ue.context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1_ap_id->value = gnb_cu_ue_f1ap_id_to_uint(ue.context.gnb_cu_ue_f1ap_id);
  resp->duto_currc_info.value.cell_group_cfg.resize(du_response.du_to_cu_rrc_container.length());
  std::copy(du_response.du_to_cu_rrc_container.begin(),
            du_response.du_to_cu_rrc_container.end(),
            resp->duto_currc_info.value.cell_group_cfg.begin());
  resp->drbs_setup_list_present = not du_request.drbs_to_addmod.empty();
  if (resp->drbs_setup_list_present) {
    resp->drbs_setup_list->resize(du_request.drbs_to_addmod.size());
    for (unsigned i = 0; i != du_request.drbs_to_addmod.size(); ++i) {
      resp->drbs_setup_list.value[i].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_ITEM);
      drbs_setup_item_s& drb_item = resp->drbs_setup_list.value[i].value().drbs_setup_item();
      drb_item.drbid              = drb_id_to_uint(du_request.drbs_to_addmod[i].drbid);
      drb_item.dluptnl_info_to_be_setup_list.resize(1);
      drb_item.dluptnl_info_to_be_setup_list[0].dluptnl_info.set_gtp_tunnel();
      // TODO: GTPU params.
    }
  }
  resp->srbs_setup_list_present = not du_request.srbs_to_addmod.empty();
  if (resp->srbs_setup_list_present) {
    resp->srbs_setup_list->resize(du_request.srbs_to_addmod.size());
    for (unsigned i = 0; i != du_request.srbs_to_addmod.size(); ++i) {
      resp->srbs_setup_list.value[i].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
      srbs_setup_item_s& srb_item = resp->srbs_setup_list.value[i].value().srbs_setup_item();
      srb_item.srbid              = srb_id_to_uint(du_request.srbs_to_addmod[i]);
      srb_item.lcid               = srb_id_to_lcid(du_request.srbs_to_addmod[i]);
    }
  }

  ue.f1c_msg_notifier.on_new_message(f1c_msg);
}

void f1ap_du_ue_context_setup_procedure::send_ue_context_setup_failure()
{
  f1c_message f1c_msg;
  f1c_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_fail_s& resp = f1c_msg.pdu.unsuccessful_outcome().value.ue_context_setup_fail();

  resp->gnb_du_ue_f1_ap_id->value        = gnb_du_ue_f1ap_id_to_uint(ue.context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1_ap_id->value        = gnb_cu_ue_f1ap_id_to_uint(ue.context.gnb_cu_ue_f1ap_id);
  resp->cause->set_radio_network().value = asn1::f1ap::cause_radio_network_opts::unspecified;

  ue.f1c_msg_notifier.on_new_message(f1c_msg);
}
