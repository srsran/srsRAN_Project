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

#include "ngap_dl_nas_message_transfer_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_dl_nas_message_transfer_procedure::ngap_dl_nas_message_transfer_procedure(
    byte_buffer               nas_pdu_,
    const ngap_ue_ids&        ue_ids_,
    ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier_,
    srslog::basic_logger&     logger_) :
  nas_pdu(nas_pdu_), ue_ids(ue_ids_), rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_), logger(logger_)
{
}

void ngap_dl_nas_message_transfer_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} ran_ue_id={} amf_ue_id={}: \"{}\" initialized",
               ue_ids.ue_index,
               ue_ids.ran_ue_id,
               ue_ids.amf_ue_id,
               name());

  send_pdu_to_rrc_ue();

  logger.debug(
      "ue={} ran_ue_id={} amf_ue_id={}: \"{}\" finalized", ue_ids.ue_index, ue_ids.ran_ue_id, ue_ids.amf_ue_id, name());

  CORO_RETURN();
}

void ngap_dl_nas_message_transfer_procedure::send_pdu_to_rrc_ue()
{
  logger.debug(nas_pdu.begin(), nas_pdu.end(), "DlNasTransport PDU ({} B)", nas_pdu.length());
  rrc_ue_pdu_notifier.on_new_pdu(std::move(nas_pdu));
}
