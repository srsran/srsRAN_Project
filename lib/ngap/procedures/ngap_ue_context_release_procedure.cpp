/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_ue_context_release_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_ue_context_release_procedure::ngap_ue_context_release_procedure(
    const cu_cp_ngap_ue_context_release_command& command_,
    ngap_du_processor_control_notifier&          du_processor_ctrl_notif_,
    ngap_message_notifier&                       amf_notif_,
    ngap_ue_manager&                             ue_manager_,
    srslog::basic_logger&                        logger_) :
  command(command_),
  du_processor_ctrl_notifier(du_processor_ctrl_notif_),
  amf_notifier(amf_notif_),
  ue_manager(ue_manager_),
  logger(logger_)
{
}

void ngap_ue_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized", command.ue_index, name());

  // Notify DU processor about UE Context Release Command
  CORO_AWAIT_VALUE(ue_context_release_complete, du_processor_ctrl_notifier.on_new_ue_context_release_command(command));

  send_ue_context_release_complete();

  logger.debug("ue={}: \"{}\" finalized", ue_context_release_complete.ue_index, name());
  CORO_RETURN();
}

void ngap_ue_context_release_procedure::send_ue_context_release_complete()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);

  auto& asn1_ue_context_release_complete = ngap_msg.pdu.successful_outcome().value.ue_context_release_complete();
  asn1_ue_context_release_complete->amf_ue_ngap_id =
      amf_ue_id_to_uint(ue_manager.find_ngap_ue(ue_context_release_complete.ue_index)->get_amf_ue_id());
  asn1_ue_context_release_complete->ran_ue_ngap_id =
      ran_ue_id_to_uint(ue_manager.find_ngap_ue(ue_context_release_complete.ue_index)->get_ran_ue_id());

  fill_asn1_ue_context_release_complete(asn1_ue_context_release_complete, ue_context_release_complete);

  // Remove NGAP UE
  ue_manager.remove_ngap_ue(ue_context_release_complete.ue_index);

  logger.info("ue={}: Sending UeContextReleaseComplete", ue_context_release_complete.ue_index);

  amf_notifier.on_new_message(ngap_msg);
}
