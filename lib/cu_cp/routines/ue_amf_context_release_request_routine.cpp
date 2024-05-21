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

#include "ue_amf_context_release_request_routine.h"
#include "ue_context_release_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

ue_amf_context_release_request_routine::ue_amf_context_release_request_routine(
    const cu_cp_ue_context_release_request& request_,
    ngap_control_message_handler&           ng_release_handler_,
    cu_cp_ue_context_release_handler&       cu_cp_ue_ctxt_release_handler_,
    srslog::basic_logger&                   logger_) :
  request(request_),
  ng_release_handler(ng_release_handler_),
  cu_cp_ue_ctxt_release_handler(cu_cp_ue_ctxt_release_handler_),
  logger(logger_)
{
}

void ue_amf_context_release_request_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Notify NGAP to request a release from the AMF
  CORO_AWAIT_VALUE(amf_ue_release_requested, ng_release_handler.handle_ue_context_release_request(request));

  if (not amf_ue_release_requested) {
    logger.debug("ue={}: UE does not have an NG UE context. Skipping NG UE Context Release Request", request.ue_index);
    // If NGAP release request was not sent to AMF, release UE from DU processor, RRC and F1AP
    CORO_AWAIT(cu_cp_ue_ctxt_release_handler.handle_ue_context_release_command({request.ue_index, request.cause}));
  }

  CORO_RETURN();
}
