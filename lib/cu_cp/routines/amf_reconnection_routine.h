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

#include "../ngap_repository.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

async_task<bool>
start_amf_reconnection(ngap_interface& ngap, timer_factory timers, std::chrono::milliseconds reconnection_retry_time);

/// \brief Handles the reconnection between the CU-CP and AMF.
class amf_reconnection_routine
{
public:
  amf_reconnection_routine(ngap_interface&           ngap_,
                           timer_factory             timers,
                           std::chrono::milliseconds reconnection_retry_time_);

  static std::string name() { return "AMF Reconnection Routine"; }

  void operator()(coro_context<async_task<bool>>& ctx);

private:
  ngap_interface&       ngap;
  srslog::basic_logger& logger;

  unique_timer              amf_tnl_connection_retry_timer;
  std::chrono::milliseconds reconnection_retry_time;

  ngap_ng_setup_result result_msg = {};
  bool                 success    = false;
};

} // namespace srs_cu_cp
} // namespace srsran
