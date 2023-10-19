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

#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_dl_nas_message_transfer_procedure
{
public:
  ngap_dl_nas_message_transfer_procedure(byte_buffer               nas_pdu_,
                                         ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier_,
                                         ngap_ue_logger&           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "DL NAS Message Transfer Procedure"; }

private:
  // results senders
  void send_pdu_to_rrc_ue();

  byte_buffer               nas_pdu;
  const ngap_ue_ids         ue_ids;
  ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier;
  ngap_ue_logger&           logger;
};

} // namespace srs_cu_cp
} // namespace srsran
