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

#include "rlc_tx_tm_entity.h"

using namespace srsran;

rlc_tx_tm_entity::rlc_tx_tm_entity(gnb_du_id_t                          du_id,
                                   du_ue_index_t                        ue_index,
                                   rb_id_t                              rb_id_,
                                   const rlc_tx_tm_config&              config,
                                   rlc_tx_upper_layer_data_notifier&    upper_dn_,
                                   rlc_tx_upper_layer_control_notifier& upper_cn_,
                                   rlc_tx_lower_layer_notifier&         lower_dn_,
                                   rlc_metrics_aggregator&              metrics_agg_,
                                   bool                                 metrics_enabled_,
                                   rlc_pcap&                            pcap_,
                                   task_executor&                       pcell_executor_,
                                   task_executor&                       ue_executor_,
                                   timer_manager&                       timers) :
  rlc_tx_entity(du_id,
                ue_index,
                rb_id_,
                upper_dn_,
                upper_cn_,
                lower_dn_,
                metrics_agg_,
                metrics_enabled_,
                pcap_,
                pcell_executor_,
                ue_executor_,
                timers),
  cfg(config),
  sdu_queue(cfg.queue_size, logger),
  pcap_context(ue_index, rb_id_, /* is_uplink */ false)
{
  metrics_low.metrics_set_mode(rlc_mode::tm);
  logger.log_info("RLC TM created. {}", cfg);
}

// TS 38.322 v16.2.0 Sec. 5.2.1.1
void rlc_tx_tm_entity::handle_sdu(byte_buffer sdu_buf, bool is_retx)
{
  rlc_sdu sdu_;

  sdu_.buf = std::move(sdu_buf);

  // Sanity check for PDCP ReTx in RLC TM
  if (SRSRAN_UNLIKELY(is_retx)) {
    logger.log_error("Ignored unexpected PDCP retransmission flag in RLC TM SDU");
  }

  size_t sdu_len = sdu_.buf.length();
  if (sdu_queue.write(sdu_)) {
    logger.log_info(sdu_.buf.begin(), sdu_.buf.end(), "TX SDU. sdu_len={} {}", sdu_len, sdu_queue.get_state());
    metrics_high.metrics_add_sdus(1, sdu_len);
    handle_changed_buffer_state();
  } else {
    logger.log_info("Dropped SDU. sdu_len={} {}", sdu_len, sdu_queue.get_state());
    metrics_high.metrics_add_lost_sdus(1);
  }
}

// TS 38.322 v16.2.0 Sec. 5.4
void rlc_tx_tm_entity::discard_sdu(uint32_t pdcp_sn)
{
  logger.log_warning("Ignoring invalid attempt to discard SDU in TM. pdcp_sn={}", pdcp_sn);
  metrics_high.metrics_add_discard_failure(1);
}

// TS 38.322 v16.2.0 Sec. 5.2.1.1
size_t rlc_tx_tm_entity::pull_pdu(span<uint8_t> mac_sdu_buf)
{
  size_t grant_len = mac_sdu_buf.size();
  logger.log_debug("MAC opportunity. grant_len={}", grant_len);

  // Get a new SDU, if none is currently being transmitted
  if (sdu.buf.empty()) {
    logger.log_debug("Reading SDU from sdu_queue. {}", sdu_queue.get_state());
    if (not sdu_queue.read(sdu)) {
      logger.log_debug("SDU queue empty. grant_len={}", grant_len);
      return 0;
    }
    logger.log_debug("Read SDU. pdcp_sn={} sdu_len={}", sdu.pdcp_sn, sdu.buf.length());
  }

  size_t sdu_len = sdu.buf.length();
  if (sdu_len > grant_len) {
    logger.log_info("SDU exeeds provided space. front_len={} grant_len={}", sdu_len, grant_len);
    metrics_low.metrics_add_small_alloc(1);
    return 0;
  }

  // Notify the upper layer about the beginning of the transfer of the current SDU
  if (sdu.pdcp_sn.has_value()) {
    upper_dn.on_transmitted_sdu(sdu.pdcp_sn.value());
  }

  // In TM there is no header, just pass the plain SDU
  auto* out_it = mac_sdu_buf.begin();
  for (span<uint8_t> seg : sdu.buf.segments()) {
    out_it = std::copy(seg.begin(), seg.end(), out_it);
  }
  size_t pdu_len = out_it - mac_sdu_buf.begin();
  logger.log_info(mac_sdu_buf.data(), pdu_len, "TX PDU. pdu_len={} grant_len={}", pdu_len, grant_len);

  // Release SDU
  sdu.buf.clear();

  // Update metrics
  metrics_low.metrics_add_pdus_no_segmentation(1, sdu_len);

  // Push PDU into PCAP.
  pcap.push_pdu(pcap_context, mac_sdu_buf.subspan(0, pdu_len));

  return pdu_len;
}

void rlc_tx_tm_entity::handle_changed_buffer_state()
{
  if (not pending_buffer_state.test_and_set(std::memory_order_seq_cst)) {
    logger.log_debug("Triggering buffer state update to lower layer");
    // Redirect handling of status to pcell_executor
    if (not pcell_executor.defer([this]() { update_mac_buffer_state(); })) {
      logger.log_error("Failed to enqueue buffer state update");
    }
  } else {
    logger.log_debug("Avoiding redundant buffer state update to lower layer");
  }
}

void rlc_tx_tm_entity::update_mac_buffer_state()
{
  pending_buffer_state.clear(std::memory_order_seq_cst);
  unsigned bs = get_buffer_state();
  logger.log_debug("Sending buffer state update to lower layer. bs={}", bs);
  lower_dn.on_buffer_state_update(bs);
}

uint32_t rlc_tx_tm_entity::get_buffer_state()
{
  return sdu_queue.get_state().n_bytes + sdu.buf.length();
}
