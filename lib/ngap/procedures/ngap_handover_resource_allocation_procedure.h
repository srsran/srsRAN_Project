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
  ngap_handover_resource_allocation_procedure(const ngap_handover_request&       request_,
                                              const amf_ue_id_t                  amf_ue_id_,
                                              ngap_ue_context_list&              ue_ctxt_list_,
                                              ngap_cu_cp_du_repository_notifier& du_repository_notif_,
                                              ngap_message_notifier&             amf_notif_,
                                              srslog::basic_logger&              logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "NG Handover Target GNB Routine"; }

private:
  // results senders
  void send_handover_request_ack(ue_index_t ue_index, ran_ue_id_t ran_ue_id);
  void send_handover_failure();

  const ngap_handover_request&       request;
  const amf_ue_id_t                  amf_ue_id;
  ngap_ue_context_list&              ue_ctxt_list;
  ngap_cu_cp_du_repository_notifier& du_repository_notifier;
  ngap_message_notifier&             amf_notifier;
  srslog::basic_logger&              logger;

  // (sub-)routine requests

  // (sub-)routine results
  ngap_handover_resource_allocation_response response;
};

} // namespace srs_cu_cp
} // namespace srsran
