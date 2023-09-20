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

#include "srsran/cu_up/cu_up_configuration.h"
#include "srsran/e1ap/common/e1_setup_messages.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

class du_cell_manager;
struct du_manager_params;

class initial_cu_up_setup_routine
{
public:
  initial_cu_up_setup_routine(const cu_up_configuration& cfg_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Initial CU-UP setup routine"; }

private:
  async_task<cu_up_e1_setup_response> start_cu_up_e1_setup_request();
  void                                handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& resp);

  const cu_up_configuration& cfg;

  srslog::basic_logger& logger;

  cu_up_e1_setup_response response_msg = {};
};

} // namespace srs_cu_up
} // namespace srsran
