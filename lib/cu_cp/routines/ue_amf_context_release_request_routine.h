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

#include "../ue_manager/ue_manager_impl.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Routine to handle the request to the AMF to remove the UE context from the CU-CP.
///
/// If the UE does not have an NG context, this procedure fallbacks to a removal of the UE context from the DU, CU-UP
/// and CU-CP without AMF involvement.
class ue_amf_context_release_request_routine
{
public:
  ue_amf_context_release_request_routine(const cu_cp_ue_context_release_request& request_,
                                         ngap_control_message_handler&           ng_release_handler_,
                                         cu_cp_ue_context_release_handler&       cu_cp_ue_ctxt_release_handler_,
                                         srslog::basic_logger&                   logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const cu_cp_ue_context_release_request request;
  ngap_control_message_handler&          ng_release_handler;
  cu_cp_ue_context_release_handler&      cu_cp_ue_ctxt_release_handler;
  srslog::basic_logger&                  logger;

  bool amf_ue_release_requested = false;
};

} // namespace srs_cu_cp
} // namespace srsran