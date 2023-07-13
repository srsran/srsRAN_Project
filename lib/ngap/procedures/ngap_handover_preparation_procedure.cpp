/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_handover_preparation_procedure.h"
#include "../ngap_asn1_converters.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_handover_preparation_procedure::ngap_handover_preparation_procedure(ngap_context_t&           context_,
                                                                         ngap_message_notifier&    amf_notif_,
                                                                         ngap_transaction_manager& ev_mng_,
                                                                         timer_factory             timers,
                                                                         srslog::basic_logger&     logger_) :
  context(context_),
  amf_notifier(amf_notif_),
  ev_mng(ev_mng_),
  logger(logger_),
  ng_setup_wait_timer(timers.create_timer())
{
}

void ngap_handover_preparation_procedure::operator()(coro_context<async_task<ngap_handover_preparation_result>>& ctx)
{
  CORO_BEGIN(ctx);

  // Forward procedure result to DU manager.
  CORO_RETURN(ngap_handover_preparation_result{});
}

