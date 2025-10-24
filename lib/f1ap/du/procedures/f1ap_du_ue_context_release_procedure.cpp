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

#include "f1ap_du_ue_context_release_procedure.h"
#include "../f1ap_du_context.h"
#include "proc_logger.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srs_du;

/// Wait period for RRC container in UE CONTEXT RELEASE REQUEST to be delivered in the lower layers.
/// Note: This timeout should account for the delay for the UE to receive the RRC container, which is non-deterministic,
/// and the timeout of 60msec specified in TS 38.331, 5.3.8.3 for the UE to ACK the RRC container.
static constexpr std::chrono::milliseconds rrc_container_delivery_timeout{120};

f1ap_du_ue_context_release_procedure::f1ap_du_ue_context_release_procedure(
    const asn1::f1ap::ue_context_release_cmd_s& msg_,
    f1ap_du_ue_manager&                         ues,
    const srs_du::f1ap_du_context&              ctxt_) :
  msg(msg_),
  ue(*ues.find(int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id))),
  cu_msg_notifier(ue.f1ap_msg_notifier),
  du_ctxt(ctxt_)
{
}

void f1ap_du_ue_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("{}: Started.", f1ap_log_prefix{ue.context, name()});

  // Mark UE context for release, so that any UE Context Release Request coming from lower layers due to RLF is ignored.
  ue.context.marked_for_release = true;

  // Stop activity in the DRBs before sending the UE's RRC release and performing the UE context removal.
  CORO_AWAIT(ue.du_handler.request_ue_drb_deactivation(ue.context.ue_index));

  // If the UE CONTEXT RELEASE COMMAND message contains the RRC-Container IE, the gNB-DU shall send the RRC
  // container to the UE via the SRB indicated by the SRB ID IE.
  if (msg->rrc_container_present) {
    CORO_AWAIT_VALUE(bool ret, handle_rrc_container());
    if (ret) {
      logger.debug("{}: RRC container delivered successfully.", f1ap_log_prefix{ue.context, name()});
    } else {
      const f1ap_du_cell_context& cell_ctx = du_ctxt.served_cells[ue.context.sp_cell_index];
      std::chrono::milliseconds   timeout  = rrc_container_delivery_timeout + cell_ctx.ntn_link_rtt;
      logger.info(
          "{}: RRC container not ACKed within a time window of {}msec. Proceeding with the UE context release...",
          f1ap_log_prefix{ue.context, name()},
          timeout.count());
    }
  }

  // Remove UE from DU manager.
  logger.debug("{}: Initiate UE release in lower layers.", f1ap_log_prefix{ue.context, name()});
  CORO_AWAIT(ue.du_handler.request_ue_removal(f1ap_ue_delete_request{ue.context.ue_index}));

  // Note: UE F1AP context deleted at this point.

  // Send UE CONTEXT RELEASE COMPLETE to CU.
  send_ue_context_release_complete();

  CORO_RETURN();
}

async_task<bool> f1ap_du_ue_context_release_procedure::handle_rrc_container()
{
  f1c_bearer* srb = nullptr;
  if (msg->srb_id_present) {
    srb_id_t srb_id = int_to_srb_id(msg->srb_id);
    srb             = ue.bearers.find_srb(srb_id);
    if (srb == nullptr) {
      logger.error("{}: Discarding RRC container. Cause: {} not found", f1ap_log_prefix{ue.context, name()}, srb_id);
      // Handle Error.
      return launch_no_op_task(false);
    }

  } else {
    logger.error("{}: Discarding RRC container. Cause: SRB-ID not defined", f1ap_log_prefix{ue.context, name()});
    // Handle Error.
    return launch_no_op_task(false);
  }

  const f1ap_du_cell_context& cell_ctx = du_ctxt.served_cells[ue.context.sp_cell_index];
  std::chrono::milliseconds   timeout  = rrc_container_delivery_timeout + cell_ctx.ntn_link_rtt;

  // Forward F1AP PDU to lower layers, and await for PDU to be transmitted over-the-air.
  return srb->handle_pdu_and_await_delivery(
      msg->rrc_container.copy(), msg->rrc_delivery_status_request_present, timeout);
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
