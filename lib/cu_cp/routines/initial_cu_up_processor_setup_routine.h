/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/cu_cp/cu_up_processor.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class initial_cu_up_processor_setup_routine
{
public:
  initial_cu_up_processor_setup_routine(cu_up_processor_context&                   context_,
                                        cu_up_processor_e1ap_control_notifier&     e1ap_conn_notifier_,
                                        cu_up_processor_cu_up_management_notifier& cu_cp_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<cu_cp_e1_setup_response> start_cu_cp_e1_setup_request();
  void                                handle_cu_cp_e1_setup_response(const cu_cp_e1_setup_response& resp);

  cu_up_processor_context& context;

  cu_up_processor_e1ap_control_notifier&     e1ap_conn_notifier;
  cu_up_processor_cu_up_management_notifier& cu_cp_notifier;

  cu_cp_e1_setup_response response = {};
};

} // namespace srs_cu_cp
} // namespace srsran
