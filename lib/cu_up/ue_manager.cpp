/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_manager.h"

using namespace srsran;
using namespace srs_cu_up;

ue_manager::ue_manager(network_interface_config&            net_config_,
                       n3_interface_config&                 n3_config_,
                       e1ap_control_message_handler&        e1ap_,
                       timer_manager&                       timers_,
                       f1u_cu_up_gateway&                   f1u_gw_,
                       gtpu_tunnel_tx_upper_layer_notifier& gtpu_tx_notifier_,
                       gtpu_demux_ctrl&                     gtpu_rx_demux_,
                       gtpu_teid_pool&                      f1u_teid_allocator_,
                       cu_up_executor_pool&                 exec_pool_,
                       dlt_pcap&                            gtpu_pcap_,
                       srslog::basic_logger&                logger_) :
  net_config(net_config_),
  n3_config(n3_config_),
  e1ap(e1ap_),
  f1u_gw(f1u_gw_),
  gtpu_tx_notifier(gtpu_tx_notifier_),
  gtpu_rx_demux(gtpu_rx_demux_),
  f1u_teid_allocator(f1u_teid_allocator_),
  exec_pool(exec_pool_),
  gtpu_pcap(gtpu_pcap_),
  timers(timers_),
  logger(logger_)
{
}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_index);
  return ue_db.contains(ue_index) ? ue_db[ue_index].get() : nullptr;
}

ue_context* ue_manager::add_ue(const ue_context_cfg& ue_cfg)
{
  if (ue_db.size() >= MAX_NOF_UES) {
    logger.error("Can't add new UE. Max number of UEs reached.");
    return nullptr;
  }

  ue_index_t new_idx = get_next_ue_index();
  if (new_idx == INVALID_UE_INDEX) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE executors
  // TODO, these should be created within the same function, so that UL, DL and CTRL executors
  // can point to the same executor.
  std::unique_ptr<task_executor, unique_function<void(task_executor*)>> ue_dl_exec = exec_pool.create_dl_pdu_executor();
  std::unique_ptr<task_executor, unique_function<void(task_executor*)>> ue_ul_exec = exec_pool.create_ul_pdu_executor();
  std::unique_ptr<task_executor, unique_function<void(task_executor*)>> ue_ctrl_exec = exec_pool.create_ctrl_executor();

  // Create executor-specific timer factories
  timer_factory ue_dl_timer_factory   = {timers, *ue_dl_exec};
  timer_factory ue_ul_timer_factory   = {timers, *ue_ul_exec};
  timer_factory ue_ctrl_timer_factory = {timers, *ue_ctrl_exec};

  // Create UE object
  std::unique_ptr<ue_context> new_ctx = std::make_unique<ue_context>(new_idx,
                                                                     ue_cfg,
                                                                     e1ap,
                                                                     net_config,
                                                                     n3_config,
                                                                     std::move(ue_dl_exec),
                                                                     std::move(ue_ul_exec),
                                                                     std::move(ue_ctrl_exec),
                                                                     ue_dl_timer_factory,
                                                                     ue_ul_timer_factory,
                                                                     ue_ctrl_timer_factory,
                                                                     f1u_gw,
                                                                     f1u_teid_allocator,
                                                                     gtpu_tx_notifier,
                                                                     gtpu_rx_demux,
                                                                     gtpu_pcap);

  // add to DB
  ue_db.emplace(new_idx, std::move(new_ctx));
  return ue_db[new_idx].get();
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  logger.debug("Scheduling ue_index={} deletion", ue_index);
  srsran_assert(ue_db.contains(ue_index), "Remove UE called for nonexistent ue_index={}", ue_index);

  // TODO: remove lookup maps

  // remove UE from database
  ue_db.erase(ue_index);

  logger.info("Removed ue_index={}", ue_index);
}

ue_index_t ue_manager::get_next_ue_index()
{
  // Search unallocated UE index
  for (int i = 0; i < MAX_NOF_UES; i++) {
    if (not ue_db.contains(i)) {
      return int_to_ue_index(i);
      break;
    }
  }
  return INVALID_UE_INDEX;
}
