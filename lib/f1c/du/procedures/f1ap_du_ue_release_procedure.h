/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {
namespace srs_du {

class f1ap_du_ue_release_procedure
{
public:
  f1ap_du_ue_release_procedure(const asn1::f1ap::ue_context_release_cmd_s& msg_, f1ap_du_ue& u) : msg(msg_), ue(u) {}

  void operator()(coro_context<async_task<void>>& ctx)
  {
    CORO_BEGIN(ctx);

    if (msg->rrc_container_present) {
      // If the UE CONTEXT RELEASE COMMAND message contains the RRC-Container IE, the gNB-DU shall send the RRC
      // container to the UE via the SRB indicated by the SRB ID IE.
      f1_bearer* srb = nullptr;
      if (msg->srbid_present) {
        srb_id_t srb_id = int_to_srb_id(msg->srbid->value);
        srb             = ue.find_srb(srb_id);
      }

      if (srb != nullptr) {
        byte_buffer sdu{msg->rrc_container.value};
        srb->handle_sdu(std::move(sdu));

        // TODO: Await SDU to be transmitted over-the-air.

      } else {
        logger.warning("SRB-ID not defined");
        // Handle Error.
      }
    }

    CORO_AWAIT(ue.du_handler.request_ue_removal(f1ap_ue_delete_request{ue.context.ue_index}));

    send_ue_context_release_complete();

    CORO_RETURN();
  }

private:
  void send_ue_context_release_complete()
  {
    using namespace asn1::f1ap;

    f1c_message f1c_msg;
    f1c_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
    ue_context_release_complete_s& resp = f1c_msg.pdu.successful_outcome().value.ue_context_release_complete();

    resp->gnb_du_ue_f1_ap_id->value = msg->gnb_du_ue_f1_ap_id->value;
    resp->gnb_cu_ue_f1_ap_id->value = msg->gnb_cu_ue_f1_ap_id->value;

    ue.f1c_msg_notifier.on_new_message(f1c_msg);
  }

  const asn1::f1ap::ue_context_release_cmd_s msg;
  srslog::basic_logger&                      logger = srslog::fetch_basic_logger("F1AP-DU");
  f1ap_du_ue&                                ue;
};

} // namespace srs_du
} // namespace srsgnb
