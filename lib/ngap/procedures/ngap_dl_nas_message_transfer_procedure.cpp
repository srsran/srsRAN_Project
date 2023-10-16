/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_dl_nas_message_transfer_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_dl_nas_message_transfer_procedure::ngap_dl_nas_message_transfer_procedure(
    byte_buffer               nas_pdu_,
    ngap_ue_context&          ue_ctxt_,
    ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier_,
    srslog::basic_logger&     logger_) :
  nas_pdu(nas_pdu_),
  ue_index(ue_ctxt_.ue_index),
  ran_ue_id(ue_ctxt_.ran_ue_id),
  amf_ue_id(ue_ctxt_.amf_ue_id),
  rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_),
  logger(logger_)
{
}

void ngap_dl_nas_message_transfer_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} ran_ue_id={} amf_ue_id={}: \"{}\" initialized", ue_index, ran_ue_id, amf_ue_id, name());

  send_pdu_to_rrc_ue();

  logger.debug("ue={} ran_ue_id={} amf_ue_id={}: \"{}\" finalized", ue_index, ran_ue_id, amf_ue_id, name());

  CORO_RETURN();
}

void ngap_dl_nas_message_transfer_procedure::send_pdu_to_rrc_ue()
{
  logger.debug(nas_pdu.begin(), nas_pdu.end(), "DlNasTransport PDU ({} B)", nas_pdu.length());
  rrc_ue_pdu_notifier.on_new_pdu(std::move(nas_pdu));
}
