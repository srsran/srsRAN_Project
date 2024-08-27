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

#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_handover_resource_allocation_procedure
{
public:
  ngap_handover_resource_allocation_procedure(const ngap_handover_request& request_,
                                              const amf_ue_id_t            amf_ue_id_,
                                              ngap_ue_context_list&        ue_ctxt_list_,
                                              ngap_cu_cp_notifier&         cu_cp_notifier_,
                                              ngap_message_notifier&       amf_notif_,
                                              timer_manager&               timers_,
                                              task_executor&               task_exec_,
                                              srslog::basic_logger&        logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "NG Handover Target GNB Routine"; }

private:
  bool create_ngap_ue(ue_index_t ue_index);

  // results senders
  void send_handover_request_ack(ue_index_t ue_index, ran_ue_id_t ran_ue_id);
  void send_handover_failure();

  const ngap_handover_request& request;
  const amf_ue_id_t            amf_ue_id;
  ngap_ue_context_list&        ue_ctxt_list;
  ngap_cu_cp_notifier&         cu_cp_notifier;
  ngap_message_notifier&       amf_notifier;
  timer_manager&               timers;
  task_executor&               task_exec;
  srslog::basic_logger&        logger;

  // (sub-)routine requests

  // (sub-)routine results
  ngap_handover_resource_allocation_response response;
};

} // namespace srs_cu_cp
} // namespace srsran
