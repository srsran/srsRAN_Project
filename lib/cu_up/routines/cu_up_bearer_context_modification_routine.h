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

#pragma once

#include "../ue_context.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

class cu_up_bearer_context_modification_routine
{
public:
  cu_up_bearer_context_modification_routine(ue_context& ue_ctxt, const e1ap_bearer_context_modification_request& msg);

  void operator()(coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx);

  static const char* name() { return "CU-UP bearer context modification routine"; }

private:
  ue_context&                                     ue_ctxt;
  const e1ap_bearer_context_modification_request& msg;
  srslog::basic_logger&                           logger;

  // Helper variables
  e1ap_bearer_context_modification_response response = {};
  security::sec_as_config                   security_info;
};

} // namespace srs_cu_up
} // namespace srsran
