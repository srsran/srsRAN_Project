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

#include "ng_setup_procedure.h"
#include "../ngap_asn1_helpers.h"
#include "srsran/ngap/ngap_setup.h"
#include "srsran/support/async/async_timer.h"
#include <variant>

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

static constexpr std::chrono::milliseconds ng_setup_response_timeout{5000};

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
    transaction_sink.subscribe_to(ev_mng.ng_setup_outcome, ng_setup_response_timeout);

    // Forward message to AMF.
    if (!amf_notifier.on_new_message(request)) {
      logger.warning("AMF notifier is not set. Cannot send NGSetupRequest");
      CORO_EARLY_RETURN(ngap_ng_setup_failure{ngap_cause_misc_t::unspecified});
    }

    // Await AMF response.
    CORO_AWAIT(transaction_sink);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.debug("Reinitiating NG setup in {}s (retry={}/{}). Received NGSetupFailure with Time to Wait IE",
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

  if (transaction_sink.timeout_expired()) {
    // Timeout case.
    logger.warning("\"{}\" timed out after {}ms", name(), ng_setup_response_timeout.count());
    fmt::print("\"{}\" timed out after {}ms", name(), ng_setup_response_timeout.count());
    return false;
  }

  if (!transaction_sink.failed()) {
    // No response received.
    logger.warning("\"{}\" failed. No response received", name());
    fmt::print("\"{}\" failed. No response received\n", name());
    return false;
  }

  const asn1::ngap::ng_setup_fail_s& ng_fail = transaction_sink.failure();

  // No point in retrying when the failure is due to misconfiguration.
  if (is_failure_misconfiguration(ng_fail->cause)) {
    logger.warning("\"{}\": Stopping procedure. Cause: misconfiguration between gNB and AMF", name());
    logger.warning("\"{}\" failed. AMF NGAP cause: \"{}\"", name(), asn1_utils::get_cause_str(ng_fail->cause));
    fmt::print("\"{}\" failed. AMF NGAP cause: \"{}\"\n", name(), asn1_utils::get_cause_str(ng_fail->cause));
    return false;
  }

  if (not ng_fail->time_to_wait_present) {
    // AMF didn't command a waiting time.
    logger.warning("\"{}\": Stopping procedure. Cause: AMF did not set any retry waiting time", name());
    logger.warning("\"{}\" failed. AMF NGAP cause: \"{}\"", name(), asn1_utils::get_cause_str(ng_fail->cause));
    fmt::print("\"{}\" failed. AMF NGAP cause: \"{}\"\n", name(), asn1_utils::get_cause_str(ng_fail->cause));
    return false;
  }
  if (ng_setup_retry_no++ >= max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.warning("\"{}\": Stopping procedure. Cause: Reached maximum number of NG Setup connection retries ({})",
                   name(),
                   max_setup_retries);
    logger.warning("\"{}\" failed. AMF NGAP cause: \"{}\"", name(), asn1_utils::get_cause_str(ng_fail->cause));
    fmt::print("\"{}\" failed. AMF NGAP cause: \"{}\"\n", name(), asn1_utils::get_cause_str(ng_fail->cause));
    return false;
  }

  time_to_wait = std::chrono::seconds{ng_fail->time_to_wait.to_number()};
  return true;
}

ngap_ng_setup_result ng_setup_procedure::create_ng_setup_result()
{
  ngap_ng_setup_result res{};

  if (transaction_sink.successful()) {
    logger.debug("\"{}\" finished successfully", name());

    fill_ngap_ng_setup_result(res, transaction_sink.response());

    auto& ng_setup_response = std::get<ngap_ng_setup_response>(res);
    for (const auto& guami_item : ng_setup_response.served_guami_list) {
      context.served_guami_list.push_back(guami_item.guami);
    }
    context.amf_name = ng_setup_response.amf_name;
  } else if (transaction_sink.failed()) {
    fill_ngap_ng_setup_result(res, transaction_sink.failure());
  } else {
    res = ngap_ng_setup_failure{ngap_cause_misc_t::unspecified};
  }

  return res;
}

bool ng_setup_procedure::is_failure_misconfiguration(const cause_c& cause)
{
  switch (cause.type()) {
    case cause_c::types_opts::radio_network:
      return false;
    case cause_c::types_opts::transport:
      return false;
    case cause_c::types_opts::nas:
      return false;
    case cause_c::types_opts::protocol:
      return false;
    case cause_c::types_opts::misc:
      return cause.misc() == asn1::ngap::cause_misc_opts::unknown_plmn_or_sn_pn;
    default:
      break;
  }
  return false;
}
