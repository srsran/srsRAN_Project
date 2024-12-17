/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_handover_resource_allocation_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<void> srsran::srs_cu_cp::start_ngap_handover_resource_allocation(const ngap_handover_request& request,
                                                                            const amf_ue_id_t            amf_ue_id,
                                                                            ngap_ue_context_list&        ue_ctxt_list,
                                                                            ngap_cu_cp_notifier&         cu_cp_notifier,
                                                                            ngap_message_notifier&       amf_notifier,
                                                                            timer_manager&               timers,
                                                                            task_executor&               task_exec,
                                                                            srslog::basic_logger&        logger)
{
  logger.error("NG Handover resource allocation failed. Cause: NG handover not supported");
  auto err_function = [](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  };
  return launch_async(std::move(err_function));
}

#endif // SRSRAN_HAS_ENTERPRISE
