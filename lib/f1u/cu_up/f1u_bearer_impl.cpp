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
using namespace srs_cu_up;

f1u_bearer_impl::f1u_bearer_impl(uint32_t                       ue_index,
                                 drb_id_t                       drb_id_,
                                 const up_transport_layer_info& ul_tnl_info_,
                                 f1u_tx_pdu_notifier&           tx_pdu_notifier_,
                                 f1u_rx_delivery_notifier&      rx_delivery_notifier_,
                                 f1u_rx_sdu_notifier&           rx_sdu_notifier_,
                                 timer_factory                  timers,
                                 f1u_bearer_disconnector&       disconnector_) :
  logger("CU-F1-U", {ue_index, drb_id_, ul_tnl_info_}),
  tx_pdu_notifier(tx_pdu_notifier_),
  rx_delivery_notifier(rx_delivery_notifier_),
  rx_sdu_notifier(rx_sdu_notifier_),
  disconnector(disconnector_),
  ul_tnl_info(ul_tnl_info_),
  dl_notif_timer(timers.create_timer())
{
  dl_notif_timer.set(std::chrono::milliseconds(f1u_dl_notif_time_ms),
                     [this](timer_id_t tid) { on_expired_dl_notif_timer(); });
}

void f1u_bearer_impl::handle_pdu(nru_ul_message msg)
{
  logger.log_debug("F1-U bearer received PDU");
  // handle T-PDU
  if (!msg.t_pdu.empty()) {
    logger.log_debug("Delivering T-PDU of size={}", msg.t_pdu.length());
    rx_sdu_notifier.on_new_sdu(std::move(msg.t_pdu));
  }
  // handle transmit notifications
  if (msg.data_delivery_status.has_value()) {
    nru_dl_data_delivery_status& status = msg.data_delivery_status.value();
    // Highest transmitted PDCP SN
    if (status.highest_transmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_transmitted_pdcp_sn.value();
      logger.log_debug("Notifying highest transmitted pdcp_sn={}", pdcp_sn);
      rx_delivery_notifier.on_transmit_notification(pdcp_sn);
    }
    // Highest successfully delivered PDCP SN
    if (status.highest_delivered_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_delivered_pdcp_sn.value();
      logger.log_debug("Notifying highest successfully delivered pdcp_sn={}", pdcp_sn);
      rx_delivery_notifier.on_delivery_notification(pdcp_sn);
    }
    // Highest successfully delivered retransmitted PDCP SN
    if (status.highest_delivered_retransmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_delivered_retransmitted_pdcp_sn.value();
      logger.log_warning("Unhandled highest successfully delivered retransmitted pdcp_sn={}", pdcp_sn);
      // TODO
    }
    // Highest retransmitted PDCP SN
    if (status.highest_retransmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_retransmitted_pdcp_sn.value();
      logger.log_warning("Unhandled highest retransmitted pdcp_sn={}", pdcp_sn);
      // TODO
    }
  }
}

void f1u_bearer_impl::handle_sdu(pdcp_tx_pdu sdu)
{
  logger.log_debug("F1-U bearer received SDU with pdcp_sn={}, size={}", sdu.pdcp_sn, sdu.buf.length());
  nru_dl_message msg = {};

  // attach the SDU
  msg.t_pdu   = std::move(sdu.buf);
  msg.pdcp_sn = sdu.pdcp_sn;

  // attach discard blocks (if any)
  fill_discard_blocks(msg);

  // stop backoff timer
  dl_notif_timer.stop();

  tx_pdu_notifier.on_new_pdu(std::move(msg));
}

void f1u_bearer_impl::discard_sdu(uint32_t pdcp_sn)
{
  // start backoff timer
  if (!dl_notif_timer.is_running()) {
    dl_notif_timer.run();
  }
  if (discard_blocks.empty()) {
    discard_blocks.push_back(nru_pdcp_sn_discard_block{});
    nru_pdcp_sn_discard_block& block = discard_blocks.back();
    block.pdcp_sn_start              = pdcp_sn;
    block.block_size                 = 1;
    logger.log_debug("Queued first SDU discard block with pdcp_sn={}", pdcp_sn);
  } else {
    nru_pdcp_sn_discard_block& last_block = discard_blocks.back();
    if (last_block.pdcp_sn_start + last_block.block_size == pdcp_sn) {
      last_block.block_size++;
      logger.log_debug("Expanded previous SDU discard block with pdcp_sn={}. pdcp_sn_start={} block_size={}",
                       pdcp_sn,
                       last_block.pdcp_sn_start,
                       last_block.block_size);
    } else {
      discard_blocks.push_back(nru_pdcp_sn_discard_block{});
      nru_pdcp_sn_discard_block& block = discard_blocks.back();
      block.pdcp_sn_start              = pdcp_sn;
      block.block_size                 = 1;
      logger.log_debug("Queued next SDU discard block with pdcp_sn={}", pdcp_sn);
      if (discard_blocks.full()) {
        logger.log_debug("Flushing SDU discard block notification. nof_blocks={}", discard_blocks.capacity());
        flush_discard_blocks();
      }
    }
  }
}

void f1u_bearer_impl::fill_discard_blocks(nru_dl_message& msg)
{
  if (!discard_blocks.empty()) {
    msg.dl_user_data.discard_blocks = std::move(discard_blocks);
    discard_blocks                  = {};
  }
}

void f1u_bearer_impl::on_expired_dl_notif_timer()
{
  logger.log_debug("DL notification timer expired");
  flush_discard_blocks();
}

void f1u_bearer_impl::flush_discard_blocks()
{
  nru_dl_message msg = {};

  // attach discard blocks (if any)
  fill_discard_blocks(msg);

  // stop backoff timer
  dl_notif_timer.stop();

  if (msg.dl_user_data.discard_blocks.has_value()) {
    logger.log_debug("Sending discard blocks");
    tx_pdu_notifier.on_new_pdu(std::move(msg));
  }
}
