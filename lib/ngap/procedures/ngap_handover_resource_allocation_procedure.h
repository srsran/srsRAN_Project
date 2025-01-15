/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

namespace srsran::srs_cu_cp {

async_task<void> start_ngap_handover_resource_allocation(const ngap_handover_request& request,
                                                         const amf_ue_id_t            amf_ue_id,
                                                         ngap_ue_context_list&        ue_ctxt_list,
                                                         ngap_cu_cp_notifier&         cu_cp_notifier,
                                                         ngap_message_notifier&       amf_notifier,
                                                         timer_manager&               timers,
                                                         task_executor&               task_exec,
                                                         srslog::basic_logger&        logger);

} // namespace srsran::srs_cu_cp
