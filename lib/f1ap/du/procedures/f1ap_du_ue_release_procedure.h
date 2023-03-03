/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {
namespace srs_du {

class f1ap_du_ue_release_procedure
{
public:
  f1ap_du_ue_release_procedure(const asn1::f1ap::ue_context_release_cmd_s& msg_, f1ap_du_ue_manager& ues) :
    msg(msg_), ue_db(ues), ue(*ues.find(int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id->value)))
  {
  }

  void operator()(coro_context<async_task<void>>& ctx)
  {
    CORO_BEGIN(ctx);

    if (msg->rrc_container_present) {
      // If the UE CONTEXT RELEASE COMMAND message contains the RRC-Container IE, the gNB-DU shall send the RRC
      // container to the UE via the SRB indicated by the SRB ID IE.
      f1c_bearer* srb = nullptr;
      if (msg->srb_id_present) {
        srb_id_t srb_id = int_to_srb_id(msg->srb_id->value);
        srb             = ue.bearers.find_srb(srb_id);
      }

      if (srb != nullptr) {
        byte_buffer pdu{msg->rrc_container.value};

        // Forward F1AP PDU to lower layers to be transmitted over-the-air.
        srb->handle_pdu(std::move(pdu));
      } else {
        logger.warning("SRB-ID not defined");
        // Handle Error.
      }
    }

    // Remove UE from DU manager.
    CORO_AWAIT(ue.du_handler.request_ue_removal(f1ap_ue_delete_request{ue.context.ue_index}));

    // Send UE CONTEXT RELEASE COMPLETE to CU.
    send_ue_context_release_complete();

    // Delete UE context from F1AP.
    ue_db.remove_ue(ue.context.ue_index);

    CORO_RETURN();
  }

private:
  void send_ue_context_release_complete()
  {
    using namespace asn1::f1ap;

    f1ap_message f1ap_msg;
    f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
    ue_context_release_complete_s& resp = f1ap_msg.pdu.successful_outcome().value.ue_context_release_complete();

    resp->gnb_du_ue_f1ap_id->value = msg->gnb_du_ue_f1ap_id->value;
    resp->gnb_cu_ue_f1ap_id->value = msg->gnb_cu_ue_f1ap_id->value;

    ue.f1ap_msg_notifier.on_new_message(f1ap_msg);
  }

  const asn1::f1ap::ue_context_release_cmd_s msg;
  f1ap_du_ue_manager&                        ue_db;
  f1ap_du_ue&                                ue;
  srslog::basic_logger&                      logger = srslog::fetch_basic_logger("DU-F1");
};

} // namespace srs_du
} // namespace srsran
