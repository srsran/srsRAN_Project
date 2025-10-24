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

#include "adapters/e1ap_adapters.h"
#include "adapters/gtpu_adapters.h"
#include "adapters/gw_adapters.h"
#include "ngu_session_manager.h"
#include "ue_manager.h"
#include "srsran/cu_up/cu_up.h"
#include "srsran/cu_up/cu_up_config.h"
#include "srsran/cu_up/cu_up_manager.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/gtpu/gtpu_echo.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include <memory>

namespace srsran::srs_cu_up {

class cu_up final : public cu_up_interface
{
public:
  explicit cu_up(const cu_up_config& cfg_, const cu_up_dependencies& dependencies);
  ~cu_up() override;

  // cu_up_interface
  void start() override;
  void stop() override;

  /// helper functions for testing
  std::optional<uint16_t> get_n3_bind_port() // TODO include index?
  {
    return ngu_sessions[0]->get_bind_port();
  }
  cu_up_manager* get_cu_up_manager() { return cu_up_mng.get(); }

private:
  async_task<void> handle_stop_command();

  void on_statistics_report_timer_expired();

  e1ap_bearer_context_modification_response
  handle_bearer_context_modification_request_impl(ue_context&                                     ue_ctxt,
                                                  const e1ap_bearer_context_modification_request& msg);

  cu_up_config   cfg;
  task_executor& ctrl_executor;
  timer_manager& timers;

  // logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-UP", false);

  // Holds DL executor for the control TEID.
  std::unique_ptr<ue_executor_mapper>        echo_exec_mapper;
  std::unique_ptr<gtpu_demux_dispatch_queue> echo_batched_queue;

  // Components
  std::atomic<bool>                                  e1ap_connected = {false};
  std::unique_ptr<e1ap_interface>                    e1ap;
  std::unique_ptr<ngu_session_manager>               ngu_session_mngr;
  std::vector<std::unique_ptr<gtpu_tnl_pdu_session>> ngu_sessions;
  std::unique_ptr<gtpu_demux>                        ngu_demux;
  std::unique_ptr<gtpu_echo>                         ngu_echo;
  std::unique_ptr<gtpu_teid_pool>                    n3_teid_allocator;
  std::unique_ptr<gtpu_teid_pool>                    f1u_teid_allocator;
  std::unique_ptr<cu_up_manager>                     cu_up_mng;

  // Adapters
  network_gateway_data_gtpu_demux_adapter gw_data_gtpu_demux_adapter;
  gtpu_network_gateway_adapter            gtpu_gw_adapter;
  e1ap_cu_up_manager_adapter              e1ap_cu_up_mng_adapter;

  std::mutex        mutex;
  bool              running{false};
  std::atomic<bool> stop_command{false};

  // Handler for CU-UP tasks.
  fifo_async_task_scheduler main_ctrl_loop;

  unique_timer statistics_report_timer;
};

} // namespace srsran::srs_cu_up
