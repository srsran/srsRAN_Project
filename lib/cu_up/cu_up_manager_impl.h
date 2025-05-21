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

#include "adapters/gtpu_adapters.h"
#include "ngu_session_manager.h"
#include "ue_manager.h"
#include "srsran/cu_up/cu_up_config.h"
#include "srsran/cu_up/cu_up_manager.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include <memory>

namespace srsran::srs_cu_up {

/// CU-UP manager implementation configuration.
struct cu_up_manager_impl_config {
  std::map<five_qi_t, cu_up_qos_config> qos;
  n3_interface_config                   n3_cfg;
  cu_up_test_mode_config                test_mode_cfg;
};

/// CU-UP manager implementation dependencies.
struct cu_up_manager_impl_dependencies {
  e1ap_interface&            e1ap;
  gtpu_demux&                ngu_demux;
  ngu_session_manager&       ngu_session_mngr;
  gtpu_teid_pool&            n3_teid_allocator;
  gtpu_teid_pool&            f1u_teid_allocator;
  cu_up_executor_mapper&     exec_mapper;
  f1u_cu_up_gateway&         f1u_gateway;
  timer_manager&             timers;
  dlt_pcap&                  gtpu_pcap;
  fifo_async_task_scheduler& cu_up_task_scheduler;
};

class cu_up_manager_impl final : public cu_up_manager
{
public:
  cu_up_manager_impl(const cu_up_manager_impl_config& config, const cu_up_manager_impl_dependencies& dependencies);

  async_task<void> stop() override;
  e1ap_bearer_context_setup_response
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override;

  async_task<e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override;

  async_task<void> handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) override;

  void schedule_cu_up_async_task(async_task<void> task);
  void schedule_ue_async_task(srs_cu_up::ue_index_t ue_index, async_task<void> task) override;

  // cu_up_e1ap_connection_notifier
  void on_e1ap_connection_establish() override { e1ap_connected = true; }
  void on_e1ap_connection_drop() override { e1ap_connected = false; }
  bool e1ap_is_connected() override { return e1ap_connected; }

  size_t get_nof_ues() override { return ue_mng->get_nof_ues(); }

  void trigger_enable_test_mode();
  void trigger_disable_test_mode();

private:
  void on_statistics_report_timer_expired();

  async_task<void> enable_test_mode() override;
  async_task<void> disable_test_mode();

  std::map<five_qi_t, cu_up_qos_config> qos;
  const network_interface_config        net_cfg;
  const n3_interface_config             n3_cfg;
  const cu_up_test_mode_config          test_mode_cfg;
  gtpu_demux&                           ngu_demux;
  cu_up_executor_mapper&                exec_mapper;
  timer_manager&                        timers;

  // Logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-UP", false);

  // Components
  std::atomic<bool>           e1ap_connected = {false};
  std::unique_ptr<ue_manager> ue_mng;

  unique_timer statistics_report_timer;

  // Test mode
  unique_timer test_mode_ue_timer;

  fifo_async_task_scheduler& cu_up_task_scheduler;
};

} // namespace srsran::srs_cu_up
