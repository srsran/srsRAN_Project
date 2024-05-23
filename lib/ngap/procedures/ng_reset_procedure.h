/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ngap_transaction_manager.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ng_reset_procedure
{
public:
  ng_reset_procedure(const ngap_message&       msg_,
                     ngap_message_notifier&    amf_notif_,
                     ngap_transaction_manager& ev_mng_,
                     srslog::basic_logger&     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "NG Reset Procedure"; }

private:
  const ngap_message        msg;
  ngap_message_notifier&    amf_notifier;
  ngap_transaction_manager& ev_mng;
  srslog::basic_logger&     logger;

  protocol_transaction_outcome_observer<asn1::ngap::ng_reset_ack_s> transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsran
