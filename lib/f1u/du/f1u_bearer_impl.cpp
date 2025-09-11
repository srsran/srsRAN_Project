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
  dl_tnl_info(dl_tnl_info_),
  buffering(cfg.buffer_ul_on_startup),
  ul_buffer(cfg.ul_buffer_size),
  rx_sdu_notifier(rx_sdu_notifier_),
  tx_pdu_notifier(tx_pdu_notifier_),
  ue_executor(ue_executor_),
  ul_notif_timer(timers.create_timer()),
  ul_buffer_timer(timers.create_timer()),
  desired_buffer_size_for_data_radio_bearer(cfg.rlc_queue_bytes_limit),
  notif_desired_buffer_size_for_data_radio_bearer(
      0) // make sure that we send an initial buffer report, even if there is no data
{
  ul_notif_timer.set(std::chrono::milliseconds(cfg.t_notify), [this](timer_id_t tid) { on_expired_ul_notif_timer(); });
  ul_notif_timer.run();

  ul_buffer_timer.set(cfg.ul_buffer_timeout, [this](timer_id_t tid) {
    logger.log_warning("UL buffering timed out, flushing PDUs. ul_buffer_size={} ul_buffer_timeout={}ms",
                       cfg.ul_buffer_size,
                       cfg.ul_buffer_timeout.count());
    flush_ul_buffer();
  });
  if (cfg.buffer_ul_on_startup) {
    ul_buffer_timer.run();
  }
  logger.log_info("F1-U bearer configured. {} {}", cfg, dl_tnl_info);
}

void f1u_bearer_impl::handle_sdu(byte_buffer_chain sdu)
{
  logger.log_debug("F1-U bearer received SDU with size={}", sdu.length());
  nru_ul_message msg = {};

  // attach the SDU
  msg.t_pdu = std::move(sdu);

  // piggy-back latest data delivery status (ignore if anything has changed)
  fill_data_delivery_status(msg);

  if (buffering) {
    logger.log_debug("Buffering UL PDU. ul_buffer_size={}", ul_buffer.size());
    if (not ul_buffer.try_push(std::move(msg))) {
      if (cfg.warn_on_drop) {
        logger.log_warning("Dropped UL PDU, UL buffer full. ul_buffer_size={}", ul_buffer.size());
      } else {
        logger.log_debug("Dropped UL PDU. ul_buffer_size={}", ul_buffer.size());
      }
    } else {
      logger.log_debug("Buffering UL PDU. ul_buffer_size={}", ul_buffer.size());
    }
  } else {
    tx_pdu_notifier.on_new_pdu(std::move(msg));
  }
}

void f1u_bearer_impl::handle_pdu(nru_dl_message msg)
{
  if (!ue_executor.execute([this, m = std::move(msg)]() mutable { handle_pdu_impl(std::move(m)); })) {
    if (!cfg.warn_on_drop) {
      logger.log_info("Dropped F1-U PDU, queue is full");
    } else {
      logger.log_warning("Dropped F1-U PDU, queue is full");
    }
  }
}

void f1u_bearer_impl::stop()
{
  if (not stopped) {
    ul_notif_timer.stop();
  }
  stopped = true;
}

void f1u_bearer_impl::handle_pdu_impl(nru_dl_message msg)
{
  logger.log_debug("F1-U bearer received PDU");
  // handle T-PDU
  if (!msg.t_pdu.empty()) {
    logger.log_debug("Delivering T-PDU. size={}", msg.t_pdu.length());
    rx_sdu_notifier.on_new_sdu(std::move(msg.t_pdu), msg.dl_user_data.retransmission_flag);
  }
  // handle polling of delivery status report
  if (msg.dl_user_data.report_polling) {
    if (send_data_delivery_status(/* force = */ true)) {
      logger.log_debug("Report polling flag is set. Sent data delivery status");
    } else {
      logger.log_warning("Report polling flag is set. No data to be sent in data delivery status");
    }
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

void f1u_bearer_impl::flush_ul_buffer()
{
  if (not buffering) {
    return;
  }
  logger.log_debug("Flushing UL PDUs. ul_buffer_size={}", ul_buffer.size());
  while (not ul_buffer.empty()) {
    nru_ul_message& ul_msg = ul_buffer.top();
    tx_pdu_notifier.on_new_pdu(std::move(ul_msg));
    ul_buffer.pop();
  }
  buffering = false;
  ul_buffer_timer.stop();
}
void f1u_bearer_impl::handle_transmit_notification(uint32_t highest_pdcp_sn, uint32_t desired_buf_size)
{
  // This function may be called from pcell_executor, since it only writes to an atomic variable
  logger.log_debug("Storing highest transmitted pdcp_sn={} and desired_buf_size={}", highest_pdcp_sn, desired_buf_size);
  highest_transmitted_pdcp_sn.store(highest_pdcp_sn, std::memory_order_relaxed);
  desired_buffer_size_for_data_radio_bearer.store(desired_buf_size, std::memory_order_relaxed);
}

void f1u_bearer_impl::handle_delivery_notification(uint32_t highest_pdcp_sn)
{
  // This function may be called from pcell_executor, since it only writes to an atomic variable
  logger.log_debug("Storing highest successfully delivered pdcp_sn={}", highest_pdcp_sn);
  highest_delivered_pdcp_sn.store(highest_pdcp_sn, std::memory_order_relaxed);
}

void f1u_bearer_impl::handle_retransmit_notification(uint32_t highest_pdcp_sn)
{
  // This function may be called from pcell_executor, since it only writes to an atomic variable
  logger.log_debug("Storing highest retransmitted pdcp_sn={}", highest_pdcp_sn);
  highest_retransmitted_pdcp_sn.store(highest_pdcp_sn, std::memory_order_relaxed);
}

void f1u_bearer_impl::handle_delivery_retransmitted_notification(uint32_t highest_pdcp_sn)
{
  // This function may be called from pcell_executor, since it only writes to an atomic variable
  logger.log_debug("Storing highest successfully delivered retransmitted pdcp_sn={}", highest_pdcp_sn);
  highest_delivered_retransmitted_pdcp_sn.store(highest_pdcp_sn, std::memory_order_relaxed);
}

bool f1u_bearer_impl::fill_desired_buffer_size_of_data_radio_bearer(nru_dl_data_delivery_status& status)
{
  uint32_t cur_desired_buffer_size_for_data_radio_bearer =
      desired_buffer_size_for_data_radio_bearer.load(std::memory_order_relaxed);
  logger.log_debug("Adding desired buffer size for DRB. bs={}", cur_desired_buffer_size_for_data_radio_bearer);
  status.desired_buffer_size_for_drb = cur_desired_buffer_size_for_data_radio_bearer;
  if (cur_desired_buffer_size_for_data_radio_bearer != notif_desired_buffer_size_for_data_radio_bearer) {
    notif_desired_buffer_size_for_data_radio_bearer = cur_desired_buffer_size_for_data_radio_bearer;
    return true;
  }
  return false;
}

bool f1u_bearer_impl::fill_highest_transmitted_pdcp_sn(nru_dl_data_delivery_status& status)
{
  uint32_t cur_highest_transmitted_pdcp_sn = highest_transmitted_pdcp_sn.load(std::memory_order_relaxed);
  // TS 38.425 Sec. 5.4.2.1
  // (...)
  // In case the DL DATA DELIVERY STATUS frame is sent before any NR PDCP PDU is transferred to lower layers, the
  // information on the highest NR PDCP PDU sequence number successfully delivered in sequence to the UE and the
  // highest NR PDCP PDU sequence number transmitted to the lower layers may not be provided.
  if (cur_highest_transmitted_pdcp_sn == unset_pdcp_sn) {
    return false;
  }
  logger.log_debug("Adding highest transmitted pdcp_sn={}", cur_highest_transmitted_pdcp_sn);
  status.highest_transmitted_pdcp_sn = cur_highest_transmitted_pdcp_sn;
  if (cur_highest_transmitted_pdcp_sn != notif_highest_transmitted_pdcp_sn) {
    notif_highest_transmitted_pdcp_sn = cur_highest_transmitted_pdcp_sn;
    return true;
  }
  return false;
}

bool f1u_bearer_impl::fill_highest_delivered_pdcp_sn(nru_dl_data_delivery_status& status)
{
  uint32_t cur_highest_delivered_pdcp_sn = highest_delivered_pdcp_sn.load(std::memory_order_relaxed);
  // TS 38.425 Sec. 5.4.2.1
  // (...)
  // When the corresponding node decides to trigger the Feedback for Downlink Data Delivery procedure it shall report
  // as specified in section 5.2:
  // a) in case of RLC AM, the highest NR PDCP PDU sequence number successfully delivered in sequence to the UE among
  // those NR PDCP PDUs received from the node hosting the NR PDCP entity i.e. excludes those retransmission NR PDCP
  // PDUs;
  // (...)
  // In case the DL DATA DELIVERY STATUS frame is sent before any NR PDCP PDU is transferred to lower layers, the
  // information on the highest NR PDCP PDU sequence number successfully delivered in sequence to the UE and the
  // highest NR PDCP PDU sequence number transmitted to the lower layers may not be provided.
  if (cur_highest_delivered_pdcp_sn == unset_pdcp_sn) {
    return false;
  }
  logger.log_debug("Adding highest delivered pdcp_sn={}", cur_highest_delivered_pdcp_sn);
  status.highest_delivered_pdcp_sn = cur_highest_delivered_pdcp_sn;
  if (cur_highest_delivered_pdcp_sn != notif_highest_delivered_pdcp_sn) {
    notif_highest_delivered_pdcp_sn = cur_highest_delivered_pdcp_sn;
    return true;
  }
  return false;
}

bool f1u_bearer_impl::fill_highest_retransmitted_pdcp_sn(nru_dl_data_delivery_status& status)
{
  uint32_t cur_highest_retransmitted_pdcp_sn = highest_retransmitted_pdcp_sn.load(std::memory_order_relaxed);
  // Only fill when value has changed since last time
  if (cur_highest_retransmitted_pdcp_sn != notif_highest_retransmitted_pdcp_sn) {
    logger.log_debug("Adding highest retransmitted pdcp_sn={}", cur_highest_retransmitted_pdcp_sn);
    notif_highest_retransmitted_pdcp_sn  = cur_highest_retransmitted_pdcp_sn;
    status.highest_retransmitted_pdcp_sn = cur_highest_retransmitted_pdcp_sn;
    return true;
  }
  return false;
}

bool f1u_bearer_impl::fill_highest_delivered_retransmitted_pdcp_sn(nru_dl_data_delivery_status& status)
{
  uint32_t cur_highest_delivered_retransmitted_pdcp_sn =
      highest_delivered_retransmitted_pdcp_sn.load(std::memory_order_relaxed);
  // Only fill when value has changed since last time
  if (cur_highest_delivered_retransmitted_pdcp_sn != notif_highest_delivered_retransmitted_pdcp_sn) {
    logger.log_debug("Adding highest delivered retransmitted pdcp_sn={}", cur_highest_delivered_retransmitted_pdcp_sn);
    notif_highest_delivered_retransmitted_pdcp_sn  = cur_highest_delivered_retransmitted_pdcp_sn;
    status.highest_delivered_retransmitted_pdcp_sn = cur_highest_delivered_retransmitted_pdcp_sn;
    return true;
  }
  return false;
}

bool f1u_bearer_impl::fill_data_delivery_status(nru_ul_message& msg)
{
  nru_dl_data_delivery_status status       = {};
  bool                        fresh_values = false;

  fresh_values |= fill_desired_buffer_size_of_data_radio_bearer(status);
  fresh_values |= fill_highest_transmitted_pdcp_sn(status);
  fresh_values |= fill_highest_delivered_pdcp_sn(status);
  fresh_values |= fill_highest_retransmitted_pdcp_sn(status);
  fresh_values |= fill_highest_delivered_retransmitted_pdcp_sn(status);

  logger.log_debug("Adding data delivery status to NR-U message");
  msg.data_delivery_status = std::move(status);

  // restart UL notification timer
  ul_notif_timer.run();

  return fresh_values;
}

bool f1u_bearer_impl::send_data_delivery_status(bool force)
{
  nru_ul_message msg          = {};
  bool           fresh_values = fill_data_delivery_status(msg);
  if (!fresh_values && !force) {
    return false;
  }
  tx_pdu_notifier.on_new_pdu(std::move(msg));
  return true;
}

void f1u_bearer_impl::on_expired_ul_notif_timer()
{
  if (send_data_delivery_status(/* force = */ false)) {
    logger.log_debug("UL notification timer expired. Sent data delivery status");
  } else {
    logger.log_debug("UL notification timer expired. No fresh data to be sent in data delivery status");
  }
}
