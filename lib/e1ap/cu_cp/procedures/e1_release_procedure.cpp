/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "e1_release_procedure.h"
#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "cu_cp/ue_context/e1ap_cu_cp_ue_context.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srs_cu_cp;

e1_release_procedure::e1_release_procedure(const asn1::e1ap::e1_release_request_s& request_,
                                           cu_up_index_t                           cu_up_index_,
                                           e1ap_message_notifier&                  pdu_notifier_,
                                           e1ap_cu_cp_notifier&                    cu_cp_notifier_,
                                           e1ap_ue_context_list&                   ue_list_,
                                           timer_factory                           timers_,
                                           srslog::basic_logger&                   logger_) :
  request(request_),
  cu_up_index(cu_up_index_),
  pdu_notifier(pdu_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  ue_list(ue_list_),
  logger(logger_),
  ue_release_wait_timer(timers_.create_timer())
{
}

void e1_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  // If there are still active UEs, release them.
  if (ue_list.size() > 0) {
    for (ue = ue_list.begin(); ue != ue_list.end(); ++ue) {
      release_request.ue_index = ue->second.ue_ids.ue_index;
      release_request.cause    = ngap_cause_radio_network_t::release_due_to_ngran_generated_reason;
      CORO_AWAIT(cu_cp_notifier.on_ue_release_required(release_request));
    }
  }

  // Wait until all UEs are released.
  while (ue_list.size() > 0) {
    logger.debug("Waiting for all UEs to be released ({} remaining)...", ue_list.size());
    CORO_AWAIT(async_wait_for(ue_release_wait_timer, std::chrono::milliseconds{10}));
  }

  // Notify CU-CP about the E1 Release Request.
  cu_cp_notifier.on_e1_release_request_received(cu_up_index);

  // Send E1 Release Response
  send_e1_release_response();

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN();
}

void e1_release_procedure::send_e1_release_response()
{
  e1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_E1AP_ID_E1_RELEASE);
  asn1::e1ap::e1_release_resp_s& rel_resp = resp.pdu.successful_outcome().value.e1_release_resp();
  rel_resp->transaction_id                = request->transaction_id;
  pdu_notifier.on_new_message(resp);
}
