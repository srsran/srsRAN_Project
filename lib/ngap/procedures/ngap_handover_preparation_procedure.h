/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ngap_asn1_utils.h"
#include "../ngap_context.h"
#include "ngap_transaction_manager.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/async_timer.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_handover_preparation_procedure
{
public:
  ngap_handover_preparation_procedure(ngap_context_t&           context_,
                                      ngap_message_notifier&    amf_notif_,
                                      ngap_transaction_manager& ev_mng_,
                                      timer_factory             timers,
                                      srslog::basic_logger&     logger_);

  void operator()(coro_context<async_task<ngap_handover_preparation_response>>& ctx);

private:
  ngap_context_t&           context;
  ngap_message_notifier&    amf_notifier;
  ngap_transaction_manager& ev_mng;
  srslog::basic_logger&     logger;

  unique_timer tng_reloc_prep_timer;

  protocol_transaction_outcome_observer<asn1::ngap::ho_cmd_s, asn1::ngap::ho_prep_fail_s> transaction_sink;

  void send_handover_required();
};

} // namespace srs_cu_cp
} // namespace srsran
