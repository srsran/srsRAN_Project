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

#include "ue_manager_interfaces.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/timers.h"
#include <unordered_map>

namespace srsran {

namespace srs_cu_up {

class ue_manager : public ue_manager_ctrl
{
public:
  explicit ue_manager(network_interface_config&                   net_config_,
                      n3_interface_config&                        n3_config_,
                      cu_up_test_mode_config&                     test_mode_config,
                      e1ap_control_message_handler&               e1ap_,
                      timer_manager&                              timers_,
                      f1u_cu_up_gateway&                          f1u_gw_,
                      gtpu_tunnel_common_tx_upper_layer_notifier& gtpu_tx_notifier_,
                      gtpu_demux_ctrl&                            gtpu_rx_demux_,
                      gtpu_teid_pool&                             n3_teid_allocator_,
                      gtpu_teid_pool&                             f1u_teid_allocator_,
                      cu_up_executor_pool&                        exec_pool_,
                      task_executor&                              ctrl_executor_,
                      dlt_pcap&                                   gtpu_pcap_,
                      srslog::basic_logger&                       logger_);

  using ue_db_t              = std::unordered_map<ue_index_t, std::unique_ptr<ue_context>>;
  using ue_task_schedulers_t = slotted_array<fifo_async_task_scheduler, MAX_NOF_UES>;
  const ue_db_t& get_ues() const { return ue_db; }

  ue_context*      add_ue(const ue_context_cfg& cfg) override;
  async_task<void> remove_ue(ue_index_t ue_index) override;
  ue_context*      find_ue(ue_index_t ue_index) override;
  size_t           get_nof_ues() const override { return ue_db.size(); };

  void schedule_ue_async_task(ue_index_t ue_index, async_task<void> task);

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index();

  network_interface_config&                   net_config;
  n3_interface_config&                        n3_config;
  cu_up_test_mode_config&                     test_mode_config;
  e1ap_control_message_handler&               e1ap;
  f1u_cu_up_gateway&                          f1u_gw;
  gtpu_tunnel_common_tx_upper_layer_notifier& gtpu_tx_notifier;
  gtpu_demux_ctrl&                            gtpu_rx_demux;
  gtpu_teid_pool&                             n3_teid_allocator;
  gtpu_teid_pool&                             f1u_teid_allocator;
  cu_up_executor_pool&                        exec_pool;
  task_executor&                              ctrl_executor;
  dlt_pcap&                                   gtpu_pcap;
  timer_manager&                              timers;
  ue_db_t                                     ue_db;
  ue_task_schedulers_t                        ue_task_schedulers;
  srslog::basic_logger&                       logger;
};

} // namespace srs_cu_up

} // namespace srsran
