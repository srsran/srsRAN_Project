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

#include "f1ap_du_ue_context_release_procedure.h"
#include "srsran/f1ap/common/f1ap_message.h"
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

  logger.debug("ue={}, proc=\"UE Context Release\": Started.", ue.context.ue_index);

  // Mark UE context for release, so that any UE Context Release Request coming from lower layers due to RLF is ignored.
  ue.context.marked_for_release = true;

  if (msg->rrc_container_present) {
    // If the UE CONTEXT RELEASE COMMAND message contains the RRC-Container IE, the gNB-DU shall send the RRC
    // container to the UE via the SRB indicated by the SRB ID IE.
    f1c_bearer* srb = nullptr;
    if (msg->srb_id_present) {
      srb_id_t srb_id = int_to_srb_id(msg->srb_id);
      srb             = ue.bearers.find_srb(srb_id);
    }

    if (srb != nullptr) {
      byte_buffer pdu{msg->rrc_container};

      // Forward F1AP PDU to lower layers to be transmitted over-the-air.
      srb->handle_pdu(std::move(pdu));
    } else {
      logger.warning("SRB-ID not defined");
      // Handle Error.
    }
  }

  // Wait for pending RRC messages to be flushed.
  logger.debug("ue={}, proc=\"UE Context Release\": Waiting for {} milliseconds to flush pending RRC messages.",
               ue.context.ue_index,
               ue_release_timeout.count());
  CORO_AWAIT(async_wait_for(release_wait_timer, ue_release_timeout));

  // Remove UE from DU manager.
  logger.debug("ue={}, proc=\"UE Context Release\": Initiate UE release in lower layers.", ue.context.ue_index);
  CORO_AWAIT(ue.du_handler.request_ue_removal(f1ap_ue_delete_request{ue.context.ue_index}));

  // Note: UE F1AP context deleted at this point.

  // Send UE CONTEXT RELEASE COMPLETE to CU.
  send_ue_context_release_complete();

  CORO_RETURN();
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

  logger.debug("ue={}, proc=\"UE Context Release\": Finished successfully.", ue.context.ue_index);
  cu_msg_notifier.on_new_message(f1ap_msg);
}
