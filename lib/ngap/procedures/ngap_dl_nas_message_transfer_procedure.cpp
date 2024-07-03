/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_dl_nas_message_transfer_procedure.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_dl_nas_message_transfer_procedure::ngap_dl_nas_message_transfer_procedure(
    const ngap_dl_nas_transport_message& msg_,
    ngap_rrc_ue_pdu_notifier&            rrc_ue_pdu_notifier_,
    ngap_ue_logger&                      logger_) :
  msg(msg_), rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_), logger(logger_)
{
}

void ngap_dl_nas_message_transfer_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  send_pdu_to_rrc_ue();

  logger.log_debug("\"{}\" finalized", name());

  CORO_RETURN();
}

void ngap_dl_nas_message_transfer_procedure::send_pdu_to_rrc_ue()
{
  rrc_ue_pdu_notifier.on_new_pdu(std::move(msg.nas_pdu));
}
