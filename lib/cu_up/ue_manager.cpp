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

#include "ue_manager.h"

using namespace srsran;
using namespace srs_cu_up;

ue_manager::ue_manager(network_interface_config&            net_config_,
                       e1ap_control_message_handler&        e1ap_,
                       timer_manager&                       timers_,
                       f1u_cu_up_gateway&                   f1u_gw_,
                       gtpu_tunnel_tx_upper_layer_notifier& gtpu_tx_notifier_,
                       gtpu_demux_ctrl&                     gtpu_rx_demux_,
                       gtpu_teid_pool&                      f1u_teid_allocator_,
                       task_executor&                       ue_exec_,
                       srslog::basic_logger&                logger_) :
  net_config(net_config_),
  e1ap(e1ap_),
  f1u_gw(f1u_gw_),
  gtpu_tx_notifier(gtpu_tx_notifier_),
  gtpu_rx_demux(gtpu_rx_demux_),
  f1u_teid_allocator(f1u_teid_allocator_),
  timers(timers_),
  ue_exec(ue_exec_),
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

  // Create UE object
  std::unique_ptr<ue_context> new_ctx = std::make_unique<ue_context>(new_idx,
                                                                     ue_cfg,
                                                                     e1ap,
                                                                     net_config,
                                                                     logger,
                                                                     timer_factory{timers, ue_exec},
                                                                     f1u_gw,
                                                                     f1u_teid_allocator,
                                                                     gtpu_tx_notifier,
                                                                     gtpu_rx_demux);

  // add to DB
  ue_db.emplace(new_idx, std::move(new_ctx));
  return ue_db[new_idx].get();
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  logger.debug("Scheduling ue_index={} deletion", ue_index);
  srsran_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ue_index={}", ue_index);

  // TODO: remove lookup maps

  // remove UE from database
  ue_db.erase(ue_index);

  logger.info("Removed ue_index={}", ue_index);
  return;
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

size_t ue_manager::get_nof_ues()
{
  return ue_db.size();
}
