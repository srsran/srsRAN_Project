/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "bearer_context_release_procedure.h"
#include "common/e1ap_asn1_converters.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"

using namespace srsran;
using namespace srsran::srs_cu_up;

bearer_context_release_procedure::bearer_context_release_procedure(const e1ap_ue_context& ue_ctxt_,
                                                                   const asn1::e1ap::bearer_context_release_cmd_s& cmd_,
                                                                   e1ap_ue_context_list&        ue_ctxt_list_,
                                                                   e1ap_message_notifier&       pdu_notifier_,
                                                                   e1ap_cu_up_manager_notifier& cu_up_notifier_,
                                                                   srslog::basic_logger&        logger_) :
  ue_ctxt(ue_ctxt_),
  cmd(cmd_),
  ue_ctxt_list(ue_ctxt_list_),
  pdu_notifier(pdu_notifier_),
  cu_up_notifier(cu_up_notifier_),
  logger(logger_)
{
}

void bearer_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  bearer_context_release_cmd.ue_index = ue_ctxt.ue_ids.ue_index;
  bearer_context_release_cmd.cause    = asn1_to_cause(cmd->cause);

  // Forward message to CU-UP
  CORO_AWAIT(cu_up_notifier.on_bearer_context_release_command_received(bearer_context_release_cmd));

  // Remove UE context
  ue_ctxt_list.remove_ue(ue_ctxt.ue_ids.ue_index);

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
