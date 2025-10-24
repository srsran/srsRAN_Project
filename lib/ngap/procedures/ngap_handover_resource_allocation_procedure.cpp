/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "ngap_handover_resource_allocation_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<void> srsran::srs_cu_cp::start_ngap_handover_resource_allocation(const ngap_handover_request& request,
                                                                            const amf_ue_id_t            amf_ue_id,
                                                                            ngap_ue_context_list&        ue_ctxt_list,
                                                                            ngap_cu_cp_notifier&         cu_cp_notifier,
                                                                            ngap_message_notifier&       amf_notifier,
                                                                            srslog::basic_logger&        logger)
{
  logger.info("NG Handover resource allocation failed. Cause: NG handover not supported");
  auto err_function = [](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  };
  return launch_async(std::move(err_function));
}

#endif // SRSRAN_HAS_ENTERPRISE
