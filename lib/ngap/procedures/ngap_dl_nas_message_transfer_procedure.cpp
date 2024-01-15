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

#include "ngap_dl_nas_message_transfer_procedure.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_dl_nas_message_transfer_procedure::ngap_dl_nas_message_transfer_procedure(
    byte_buffer               nas_pdu_,
    ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier_,
    ngap_ue_logger&           logger_) :
  nas_pdu(nas_pdu_), rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_), logger(logger_)
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
  logger.log_debug(nas_pdu.begin(), nas_pdu.end(), "DlNasTransport PDU ({} B)", nas_pdu.length());
  rrc_ue_pdu_notifier.on_new_pdu(std::move(nas_pdu));
}
