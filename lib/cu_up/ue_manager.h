/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ue_manager_interfaces.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/timers.h"

namespace srsran {

namespace srs_cu_up {

class ue_manager : public ue_manager_ctrl
{
public:
  explicit ue_manager(network_interface_config&                   net_config_,
                      n3_interface_config&                        n3_config_,
                      e1ap_control_message_handler&               e1ap_,
                      timer_manager&                              timers_,
                      f1u_cu_up_gateway&                          f1u_gw_,
                      gtpu_tunnel_common_tx_upper_layer_notifier& gtpu_tx_notifier_,
                      gtpu_demux_ctrl&                            gtpu_rx_demux_,
                      gtpu_teid_pool&                             n3_teid_allocator_,
                      gtpu_teid_pool&                             f1u_teid_allocator_,
                      cu_up_executor_pool&                        exec_pool_,
                      dlt_pcap&                                   gtpu_pcap_,
                      srslog::basic_logger&                       logger_);

  using ue_db_t = slotted_array<std::unique_ptr<ue_context>, MAX_NOF_UES>;
  const ue_db_t& get_ues() const { return ue_db; }

  ue_context* add_ue(const ue_context_cfg& cfg) override;
  void        remove_ue(ue_index_t ue_index) override;
  ue_context* find_ue(ue_index_t ue_index) override;
  size_t      get_nof_ues() const override { return ue_db.size(); };

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index();

  network_interface_config&                   net_config;
  n3_interface_config&                        n3_config;
  e1ap_control_message_handler&               e1ap;
  f1u_cu_up_gateway&                          f1u_gw;
  gtpu_tunnel_common_tx_upper_layer_notifier& gtpu_tx_notifier;
  gtpu_demux_ctrl&                            gtpu_rx_demux;
  gtpu_teid_pool&                             n3_teid_allocator;
  gtpu_teid_pool&                             f1u_teid_allocator;
  cu_up_executor_pool&                        exec_pool;
  dlt_pcap&                                   gtpu_pcap;
  timer_manager&                              timers;
  ue_db_t                                     ue_db;
  srslog::basic_logger&                       logger;

  fifo_async_task_scheduler task_sched;
};

} // namespace srs_cu_up

} // namespace srsran
