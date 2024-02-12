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

#include "ngap_ue_context_release_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_ue_context_release_procedure::ngap_ue_context_release_procedure(
    const cu_cp_ue_context_release_command& command_,
    const ngap_ue_ids&                      ue_ids_,
    ngap_du_processor_control_notifier&     du_processor_ctrl_notifier_,
    ngap_message_notifier&                  amf_notifier_,
    ngap_ue_logger&                         logger_) :
  command(command_),
  ue_ids(ue_ids_),
  du_processor_ctrl_notifier(du_processor_ctrl_notifier_),
  amf_notifier(amf_notifier_),
  logger(logger_)
{
}

void ngap_ue_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  // Notify DU processor about UE Context Release Command
  CORO_AWAIT_VALUE(ue_context_release_complete, du_processor_ctrl_notifier.on_new_ue_context_release_command(command));

  // Verify response from DU processor.
  if (ue_context_release_complete.ue_index != command.ue_index) {
    logger.log_debug("\"{}\" aborted. UE does not exist anymore", name());
    CORO_EARLY_RETURN();
  }

  // Note: From this point the UE is removed and only the stored context can be accessed.

  send_ue_context_release_complete();

  logger.log_debug("\"{}\" finalized", name());
  CORO_RETURN();
}

void ngap_ue_context_release_procedure::send_ue_context_release_complete()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);

  auto& asn1_ue_context_release_complete = ngap_msg.pdu.successful_outcome().value.ue_context_release_complete();
  asn1_ue_context_release_complete->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ids.amf_ue_id);
  asn1_ue_context_release_complete->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ids.ran_ue_id);

  fill_asn1_ue_context_release_complete(asn1_ue_context_release_complete, ue_context_release_complete);

  logger.log_info("Sending UeContextReleaseComplete");

  amf_notifier.on_new_message(ngap_msg);
}
