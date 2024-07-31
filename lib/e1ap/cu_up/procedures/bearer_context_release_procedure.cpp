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

#include "bearer_context_release_procedure.h"
#include "common/e1ap_asn1_converters.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"

using namespace srsran;
using namespace srsran::srs_cu_up;

bearer_context_release_procedure::bearer_context_release_procedure(ue_index_t ue_index_,
                                                                   const asn1::e1ap::bearer_context_release_cmd_s& cmd_,
                                                                   e1ap_message_notifier&       pdu_notifier_,
                                                                   e1ap_cu_up_manager_notifier& cu_up_notifier_,
                                                                   srslog::basic_logger&        logger_) :
  ue_index(ue_index_), cmd(cmd_), pdu_notifier(pdu_notifier_), cu_up_notifier(cu_up_notifier_), logger(logger_)
{
}

void bearer_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  bearer_context_release_cmd.ue_index = ue_index;
  bearer_context_release_cmd.cause    = asn1_to_cause(cmd->cause);

  // Forward message to CU-UP
  CORO_AWAIT(cu_up_notifier.on_bearer_context_release_command_received(bearer_context_release_cmd));

  e1ap_msg.pdu.set_successful_outcome();
  e1ap_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);
  e1ap_msg.pdu.successful_outcome().value.bearer_context_release_complete()->gnb_cu_cp_ue_e1ap_id =
      cmd->gnb_cu_cp_ue_e1ap_id;
  e1ap_msg.pdu.successful_outcome().value.bearer_context_release_complete()->gnb_cu_up_ue_e1ap_id =
      cmd->gnb_cu_up_ue_e1ap_id;

  // send response
  logger.debug("ue={} cu_up_ue_e1ap_id={} cu_cp_ue_e1ap_id={}: Sending BearerContextReleaseComplete",
               bearer_context_release_cmd.ue_index,
               cmd->gnb_cu_up_ue_e1ap_id,
               cmd->gnb_cu_cp_ue_e1ap_id);
  pdu_notifier.on_new_message(e1ap_msg);
  CORO_RETURN();
}
