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

#include "f1u_bearer_impl.h"

using namespace srsran;
using namespace srs_du;

f1u_bearer_impl::f1u_bearer_impl(uint32_t                       ue_index,
                                 drb_id_t                       drb_id_,
                                 const up_transport_layer_info& dl_tnl_info_,
                                 const f1u_config&              config,
                                 f1u_rx_sdu_notifier&           rx_sdu_notifier_,
                                 f1u_tx_pdu_notifier&           tx_pdu_notifier_,
                                 timer_factory                  timers,
                                 task_executor&                 ue_executor_) :
  logger("DU-F1-U", {ue_index, drb_id_, dl_tnl_info_}),
  cfg(config),
  rx_sdu_notifier(rx_sdu_notifier_),
  tx_pdu_notifier(tx_pdu_notifier_),
  ue_executor(ue_executor_),
  ul_notif_timer(timers.create_timer())
{
  ul_notif_timer.set(std::chrono::milliseconds(cfg.t_notify), [this](timer_id_t tid) { on_expired_ul_notif_timer(); });
  ul_notif_timer.run();

  logger.log_info("F1-U bearer configured. {}", cfg);
}

void f1u_bearer_impl::handle_sdu(byte_buffer_chain sdu)
{
  logger.log_debug("F1-U bearer received SDU with size={}", sdu.length());
  nru_ul_message msg = {};

  // attach the SDU
  msg.t_pdu = std::move(sdu);

  // attach data delivery status (if anything has changed)
  fill_data_delivery_status(msg);

  tx_pdu_notifier.on_new_pdu(std::move(msg));
}

void f1u_bearer_impl::handle_pdu(nru_dl_message msg)
{
  auto fn = [this, m = std::move(msg)]() mutable { handle_pdu_impl(std::move(m)); };
  if (!ue_executor.execute(std::move(fn))) {
    logger.log_warning("Dropped F1-U PDU, queue is full");
  }
}

void f1u_bearer_impl::handle_pdu_impl(nru_dl_message msg)
{
  logger.log_debug("F1-U bearer received PDU");
  // handle T-PDU
  if (!msg.t_pdu.empty()) {
    logger.log_debug("Delivering T-PDU. size={} pdcp_sn={}", msg.t_pdu.length(), msg.pdcp_sn);
    pdcp_tx_pdu tx_sdu = {};
    tx_sdu.buf         = std::move(msg.t_pdu);
    tx_sdu.pdcp_sn     = msg.pdcp_sn;
    rx_sdu_notifier.on_new_sdu(tx_sdu);
  }
  // handle discard notifications
  if (msg.dl_user_data.discard_blocks.has_value()) {
    nru_pdcp_sn_discard_blocks& blocks     = msg.dl_user_data.discard_blocks.value();
    size_t                      nof_blocks = blocks.size();
    for (size_t block_idx = 0; block_idx < nof_blocks; block_idx++) {
      const nru_pdcp_sn_discard_block& block       = blocks[block_idx];
      uint32_t                         pdcp_sn_end = block.pdcp_sn_start + block.block_size;
      for (uint32_t pdcp_sn = block.pdcp_sn_start; pdcp_sn < pdcp_sn_end; pdcp_sn++) {
        logger.log_debug("Notifying PDU discard for pdcp_sn={}", pdcp_sn);
        rx_sdu_notifier.on_discard_sdu(pdcp_sn);
      }
    }
  }
}

void f1u_bearer_impl::handle_transmit_notification(uint32_t highest_pdcp_sn)
{
  // This function may be called from pcell_executor, since it only writes to an atomic variable
  logger.log_debug("Storing highest transmitted pdcp_sn={}", highest_pdcp_sn);
  highest_transmitted_pdcp_sn.store(highest_pdcp_sn, std::memory_order_relaxed);
}

void f1u_bearer_impl::handle_delivery_notification(uint32_t highest_pdcp_sn)
{
  // This function may be called from pcell_executor, since it only writes to an atomic variable
  logger.log_debug("Storing highest successfully delivered pdcp_sn={}", highest_pdcp_sn);
  highest_delivered_pdcp_sn.store(highest_pdcp_sn, std::memory_order_relaxed);
}

bool f1u_bearer_impl::fill_highest_transmitted_pdcp_sn(nru_dl_data_delivery_status& status)
{
  uint32_t cur_highest_transmitted_pdcp_sn = highest_transmitted_pdcp_sn.load(std::memory_order_relaxed);
  if (cur_highest_transmitted_pdcp_sn != notif_highest_transmitted_pdcp_sn) {
    logger.log_debug("Adding highest transmitted pdcp_sn={}", cur_highest_transmitted_pdcp_sn);
    notif_highest_transmitted_pdcp_sn  = cur_highest_transmitted_pdcp_sn;
    status.highest_transmitted_pdcp_sn = cur_highest_transmitted_pdcp_sn;
    return true;
  }
  return false;
}

bool f1u_bearer_impl::fill_highest_delivered_pdcp_sn(nru_dl_data_delivery_status& status)
{
  uint32_t cur_highest_delivered_pdcp_sn = highest_delivered_pdcp_sn.load(std::memory_order_relaxed);
  if (cur_highest_delivered_pdcp_sn != notif_highest_delivered_pdcp_sn) {
    logger.log_debug("Adding highest delivered pdcp_sn={}", cur_highest_delivered_pdcp_sn);
    notif_highest_delivered_pdcp_sn  = cur_highest_delivered_pdcp_sn;
    status.highest_delivered_pdcp_sn = cur_highest_delivered_pdcp_sn;
    return true;
  }
  return false;
}

void f1u_bearer_impl::fill_data_delivery_status(nru_ul_message& msg)
{
  nru_dl_data_delivery_status status         = {};
  bool                        status_changed = false;

  status_changed |= fill_highest_transmitted_pdcp_sn(status);
  status_changed |= fill_highest_delivered_pdcp_sn(status);

  if (status_changed) {
    logger.log_debug("Adding data delivery status to NR-U message");
    msg.data_delivery_status = std::move(status);
  }

  // restart UL notification timer
  ul_notif_timer.run();
}

void f1u_bearer_impl::on_expired_ul_notif_timer()
{
  nru_ul_message msg = {};
  fill_data_delivery_status(msg);
  if (msg.data_delivery_status.has_value()) {
    logger.log_debug("Sending data delivery status due to expired UL notification timer");
    tx_pdu_notifier.on_new_pdu(std::move(msg));
  }
}
