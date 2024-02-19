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

#include "ng_setup_procedure.h"
#include "../ngap_asn1_helpers.h"
#include "srsran/adt/variant.h"
#include "srsran/ngap/ngap_setup.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ng_setup_procedure::ng_setup_procedure(ngap_context_t&           context_,
                                       const ngap_message&       request_,
                                       const unsigned            max_setup_retries_,
                                       ngap_message_notifier&    amf_notif_,
                                       ngap_transaction_manager& ev_mng_,
                                       timer_factory             timers,
                                       srslog::basic_logger&     logger_) :
  context(context_),
  request(request_),
  max_setup_retries(max_setup_retries_),
  amf_notifier(amf_notif_),
  ev_mng(ev_mng_),
  logger(logger_),
  ng_setup_wait_timer(timers.create_timer())
{
}

void ng_setup_procedure::operator()(coro_context<async_task<ngap_ng_setup_result>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  while (true) {
    // Subscribe to respective publisher to receive NG SETUP RESPONSE/FAILURE message.
    transaction_sink.subscribe_to(ev_mng.ng_setup_outcome);

    // Send request to AMF.
    amf_notifier.on_new_message(request);

    // Await AMF response.
    CORO_AWAIT(transaction_sink);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.info("Reinitiating NG setup in {}s (retry={}/{}). Received NGSetupFailure with Time to Wait IE",
                time_to_wait.count(),
                ng_setup_retry_no,
                max_setup_retries);
    CORO_AWAIT(
        async_wait_for(ng_setup_wait_timer, std::chrono::duration_cast<std::chrono::milliseconds>(time_to_wait)));
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_ng_setup_result());
}

bool ng_setup_procedure::retry_required()
{
  if (transaction_sink.successful()) {
    // Success case.
    return false;
  }

  const asn1::ngap::ng_setup_fail_s& ng_fail = transaction_sink.failure();
  logger.warning("\"{}\" failed. AMF NGAP cause: \"{}\"", name(), get_cause_str(ng_fail->cause));

  if (not ng_fail->time_to_wait_present) {
    // AMF didn't command a waiting time.
    logger.warning("\"{}\": Stopping procedure. Cause: AMF did not set any retry waiting time", name());
    return false;
  }
  if (ng_setup_retry_no++ >= max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.warning("\"{}\": Stopping procedure. Cause: Reached maximum number of NG Setup connection retries ({})",
                   max_setup_retries);
    return false;
  }

  time_to_wait = std::chrono::seconds{ng_fail->time_to_wait.to_number()};
  return true;
}

ngap_ng_setup_result ng_setup_procedure::create_ng_setup_result()
{
  ngap_ng_setup_result res{};

  if (transaction_sink.successful()) {
    logger.info("\"{}\" finished successfully", name());

    fill_ngap_ng_setup_result(res, transaction_sink.response());

    for (const auto& guami_item : variant_get<ngap_ng_setup_response>(res).served_guami_list) {
      context.served_guami_list.push_back(guami_item.guami);
    }

  } else {
    fill_ngap_ng_setup_result(res, transaction_sink.failure());
  }

  return res;
}
