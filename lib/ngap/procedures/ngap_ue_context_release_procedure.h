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

class ngap_ue_context_release_procedure
{
public:
  ngap_ue_context_release_procedure(const cu_cp_ngap_ue_context_release_command& command_,
                                    ngap_ue_context&                             ue_ctxt_,
                                    ngap_du_processor_control_notifier&          du_processor_ctrl_notifier_,
                                    ngap_message_notifier&                       amf_notifier_,
                                    srslog::basic_logger&                        logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Context Release Procedure"; }

private:
  // results senders
  void send_ue_context_release_complete(ue_index_t ue_index, amf_ue_id_t amf_ue_id, ran_ue_id_t ran_ue_id);

  cu_cp_ngap_ue_context_release_command command;
  const ngap_ue_context                 ue_ctxt;
  cu_cp_ue_context_release_complete     ue_context_release_complete;
  ngap_du_processor_control_notifier&   du_processor_ctrl_notifier;
  ngap_message_notifier&                amf_notifier;
  srslog::basic_logger&                 logger;
};

} // namespace srs_cu_cp
} // namespace srsran
