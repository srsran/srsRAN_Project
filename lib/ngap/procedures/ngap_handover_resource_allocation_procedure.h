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

#include "srsran/cu_cp/ue_manager.h" // for ngap_ue
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_handover_resource_allocation_procedure
{
public:
  ngap_handover_resource_allocation_procedure(const ngap_handover_request&       request_,
                                              const amf_ue_id_t                  amf_ue_id_,
                                              ngap_cu_cp_du_repository_notifier& du_repository_notif_,
                                              ngap_message_notifier&             amf_notif_,
                                              ngap_ue_manager&                   ue_manager_,
                                              srslog::basic_logger&              logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "NG Handover Target GNB Routine"; }

private:
  // results senders
  void send_handover_request_ack();
  void send_handover_failure();

  const ngap_handover_request&       request;
  const amf_ue_id_t                  amf_ue_id;
  ngap_cu_cp_du_repository_notifier& du_repository_notifier;
  ngap_message_notifier&             amf_notifier;
  ngap_ue_manager&                   ue_manager;
  srslog::basic_logger&              logger;

  ngap_ue* ue = nullptr;

  // (sub-)routine requests

  // (sub-)routine results
  ngap_handover_resource_allocation_response response;
};

} // namespace srs_cu_cp
} // namespace srsran
