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
#include "f1ap_du_ue_context_common.h"

using namespace srsgnb;
using namespace srs_du;
using namespace asn1::f1ap;

f1ap_du_ue_context_setup_procedure::f1ap_du_ue_context_setup_procedure(
    const asn1::f1ap::ue_context_setup_request_s& msg,
    f1ap_du_ue&                                   ue_) :
  ue(ue_)
{
  create_du_request(msg);
  if (msg->rrc_container_present) {
    rrc_container = msg->rrc_container.value.copy();
  }
}

void f1ap_du_ue_context_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Setup new UE configuration in DU.
  CORO_AWAIT_VALUE(du_response, ue.du_handler.request_ue_context_update(du_request));

  send_rrc_container();

  if (du_response.result) {
    send_ue_context_setup_response();
  } else {
    send_ue_context_setup_failure();
  }

  CORO_RETURN();
}

void f1ap_du_ue_context_setup_procedure::create_du_request(const ue_context_setup_request_s& msg)
{
  // > Construct DU request.
  du_request.ue_index = ue.context.ue_index;

  // >> Pass SRBs to setup.
  for (const auto& srb : msg->srbs_to_be_setup_list.value) {
    du_request.srbs_to_setup.push_back(make_srb_id(srb.value().srbs_to_be_setup_item()));
  }

  // >> Pass DRBs to setup.
  for (const auto& drb : msg->drbs_to_be_setup_list.value) {
    du_request.drbs_to_setup.push_back(make_drb_to_setup(drb.value().drbs_to_be_setup_item()));
  }
}

void f1ap_du_ue_context_setup_procedure::send_rrc_container()
{
  // > If the UE CONTEXT SETUP REQUEST message contains the RRC-Container IE, the gNB-DU shall send the corresponding
  // RRC message to the UE via SRB1.
  if (rrc_container.empty()) {
    return;
  }
  ue.bearers.find_srb(srb_id_t::srb1)->handle_pdu(std::move(rrc_container));
}

void f1ap_du_ue_context_setup_procedure::send_ue_context_setup_response()
{
  f1c_message f1c_msg;
  f1c_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_resp_s& resp = f1c_msg.pdu.successful_outcome().value.ue_context_setup_resp();

  resp->gnb_du_ue_f1ap_id->value = gnb_du_ue_f1ap_id_to_uint(ue.context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1ap_id->value = gnb_cu_ue_f1ap_id_to_uint(ue.context.gnb_cu_ue_f1ap_id);

  // > DU-to-CU RRC Container.
  resp->du_to_cu_rrc_info.value.cell_group_cfg.append(du_response.du_to_cu_rrc_container);

  // > If the C-RNTI IE is included in the UE CONTEXT SETUP RESPONSE, the gNB-CU shall consider that the C-RNTI has
  // been allocated by the gNB-DU for this UE context.
  resp->c_rnti_present = true;
  resp->c_rnti->value  = ue.context.rnti;

  // > SRBs setup list.
  resp->srbs_setup_list_present = not du_request.srbs_to_setup.empty();
  if (resp->srbs_setup_list_present) {
    resp->srbs_setup_list->resize(du_request.srbs_to_setup.size());
    for (unsigned i = 0; i != du_request.srbs_to_setup.size(); ++i) {
      resp->srbs_setup_list.value[i].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
      srbs_setup_item_s& srb_item = resp->srbs_setup_list.value[i].value().srbs_setup_item();
      srb_item.srb_id             = srb_id_to_uint(du_request.srbs_to_setup[i]);
      srb_item.lcid               = srb_id_to_lcid(du_request.srbs_to_setup[i]);
    }
  }

  // > DRBs setup List.
  resp->drbs_setup_list_present = not du_request.drbs_to_setup.empty();
  if (resp->drbs_setup_list_present) {
    // TODO: Consider that not all DRBs may have been setup.
    resp->drbs_setup_list->resize(du_request.drbs_to_setup.size());
    for (unsigned i = 0; i != du_request.drbs_to_setup.size(); ++i) {
      resp->drbs_setup_list.value[i].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_ITEM);
      drbs_setup_item_s& drb_item = resp->drbs_setup_list.value[i].value().drbs_setup_item();
      drb_item.drb_id             = drb_id_to_uint(du_request.drbs_to_setup[i].drb_id);
      drb_item.dl_up_tnl_info_to_be_setup_list.resize(1);
      drb_item.dl_up_tnl_info_to_be_setup_list[0].dl_up_tnl_info.set_gtp_tunnel();
      // TODO: GTPU params.
    }
  }

  // Send Response to CU-CP.
  ue.f1c_msg_notifier.on_new_message(f1c_msg);
}

void f1ap_du_ue_context_setup_procedure::send_ue_context_setup_failure()
{
  f1c_message f1c_msg;
  f1c_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_fail_s& resp = f1c_msg.pdu.unsuccessful_outcome().value.ue_context_setup_fail();

  resp->gnb_du_ue_f1ap_id->value         = gnb_du_ue_f1ap_id_to_uint(ue.context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1ap_id->value         = gnb_cu_ue_f1ap_id_to_uint(ue.context.gnb_cu_ue_f1ap_id);
  resp->cause->set_radio_network().value = asn1::f1ap::cause_radio_network_opts::unspecified;

  ue.f1c_msg_notifier.on_new_message(f1c_msg);
}
