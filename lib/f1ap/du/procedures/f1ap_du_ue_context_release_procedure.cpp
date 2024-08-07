/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_ue_context_release_procedure.h"
#include "proc_logger.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srs_du;

f1ap_du_ue_context_release_procedure::f1ap_du_ue_context_release_procedure(
    const asn1::f1ap::ue_context_release_cmd_s& msg_,
    f1ap_du_ue_manager&                         ues) :
  msg(msg_),
  ue(*ues.find(int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id))),
  cu_msg_notifier(ue.f1ap_msg_notifier),
  release_wait_timer(ue.du_handler.get_timer_factory().create_timer())
{
}

void f1ap_du_ue_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  // Wait period before the UE context is deleted from the DU. This value should be large enough to ensure any
  // pending RRC message (e.g. RRC Release) is sent to the UE.
  static const std::chrono::milliseconds ue_release_timeout{120};

  CORO_BEGIN(ctx);

  logger.debug("{}: Started.", f1ap_log_prefix{ue.context, name()});

  // Mark UE context for release, so that any UE Context Release Request coming from lower layers due to RLF is ignored.
  ue.context.marked_for_release = true;

  // Stop activity in the DRBs before sending the UE's RRC release and performing the UE context removal.
  CORO_AWAIT(ue.du_handler.request_ue_drb_deactivation(ue.context.ue_index));

  // If the UE CONTEXT RELEASE COMMAND message contains the RRC-Container IE, the gNB-DU shall send the RRC
  // container to the UE via the SRB indicated by the SRB ID IE.
  if (msg->rrc_container_present) {
    CORO_AWAIT(handle_rrc_container());
  }

  // Wait for pending RRC messages to be flushed.
  logger.debug("{}: Waiting for {} milliseconds to flush pending RRC messages.",
               f1ap_log_prefix{ue.context, name()},
               ue_release_timeout.count());
  CORO_AWAIT(async_wait_for(release_wait_timer, ue_release_timeout));

  // Remove UE from DU manager.
  logger.debug("{}: Initiate UE release in lower layers.", f1ap_log_prefix{ue.context, name()});
  CORO_AWAIT(ue.du_handler.request_ue_removal(f1ap_ue_delete_request{ue.context.ue_index}));

  // Note: UE F1AP context deleted at this point.

  // Send UE CONTEXT RELEASE COMPLETE to CU.
  send_ue_context_release_complete();

  CORO_RETURN();
}

async_task<void> f1ap_du_ue_context_release_procedure::handle_rrc_container()
{
  f1c_bearer* srb = nullptr;
  if (msg->srb_id_present) {
    srb_id_t srb_id = int_to_srb_id(msg->srb_id);
    srb             = ue.bearers.find_srb(srb_id);
    if (srb == nullptr) {
      logger.error("{}: Discarding RRC container. Cause: {} not found", f1ap_log_prefix{ue.context, name()}, srb_id);
      // Handle Error.
      return launch_no_op_task();
    }

  } else {
    logger.error("{}: Discarding RRC container. Cause: SRB-ID not defined", f1ap_log_prefix{ue.context, name()});
    // Handle Error.
    return launch_no_op_task();
  }

  // Forward F1AP PDU to lower layers, and await for PDU to be transmitted over-the-air.
  return srb->handle_pdu_and_await_transmission(msg->rrc_container.copy());
}

void f1ap_du_ue_context_release_procedure::send_ue_context_release_complete()
{
  using namespace asn1::f1ap;

  // Note: Do not use ue context here as it may have been deleted.

  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  ue_context_release_complete_s& resp = f1ap_msg.pdu.successful_outcome().value.ue_context_release_complete();

  resp->gnb_du_ue_f1ap_id = msg->gnb_du_ue_f1ap_id;
  resp->gnb_cu_ue_f1ap_id = msg->gnb_cu_ue_f1ap_id;

  logger.debug("{}: Finished successfully.", f1ap_log_prefix{ue.context, name()});
  cu_msg_notifier.on_new_message(f1ap_msg);
}
