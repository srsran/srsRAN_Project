/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_tx_tm_entity.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

rlc_tx_tm_entity::rlc_tx_tm_entity(uint32_t                             du_index,
                                   du_ue_index_t                        ue_index,
                                   rb_id_t                              rb_id,
                                   rlc_tx_upper_layer_data_notifier&    upper_dn_,
                                   rlc_tx_upper_layer_control_notifier& upper_cn_,
                                   rlc_tx_lower_layer_notifier&         lower_dn_,
                                   task_executor&                       pcell_executor_,
                                   pcap_rlc&                            pcap_) :
  rlc_tx_entity(du_index, ue_index, rb_id, upper_dn_, upper_cn_, lower_dn_, pcap_),
  pcell_executor(pcell_executor_),
  pcap_context(ue_index, rb_id, /* is_uplink */ false)
{
  metrics.metrics_set_mode(rlc_mode::tm);
  logger.log_info("RLC TM created.");
}

// TS 38.322 v16.2.0 Sec. 5.2.1.1
void rlc_tx_tm_entity::handle_sdu(rlc_sdu sdu)
{
  size_t sdu_length = sdu.buf.length();
  if (sdu_queue.write(sdu)) {
    logger.log_info(sdu.buf.begin(), sdu.buf.end(), "TX SDU. sdu_len={} {}", sdu.buf.length(), sdu_queue);
    metrics.metrics_add_sdus(1, sdu_length);
    handle_changed_buffer_state();
  } else {
    logger.log_info("Dropped SDU. sdu_len={} {}", sdu.buf.length(), sdu_queue);
    metrics.metrics_add_lost_sdus(1);
  }
}

// TS 38.322 v16.2.0 Sec. 5.4
void rlc_tx_tm_entity::discard_sdu(uint32_t pdcp_sn)
{
  logger.log_warning("Ignoring invalid attempt to discard SDU in TM. pdcp_sn={}", pdcp_sn);
  metrics.metrics_add_discard_failure(1);
}

// TS 38.322 v16.2.0 Sec. 5.2.1.1
byte_buffer_chain rlc_tx_tm_entity::pull_pdu(uint32_t grant_len)
{
  if (sdu_queue.is_empty()) {
    logger.log_debug("SDU queue empty. grant_len={}", grant_len);
    return {};
  }

  uint32_t front_len = {};
  if (not sdu_queue.front_size_bytes(front_len)) {
    logger.log_warning("Could not get sdu_len of SDU queue front. grant_len={}", grant_len);
    return {};
  }

  if (front_len > grant_len) {
    logger.log_info("SDU/PDU exeeds provided space. front_len={} grant_len={}", front_len, grant_len);
    metrics.metrics_add_small_alloc(1);
    return {};
  }

  rlc_sdu sdu = {};
  logger.log_debug("Reading SDU from sdu_queue. {}", sdu_queue);
  if (not sdu_queue.read(sdu)) {
    logger.log_warning("Could not read SDU from non-empty queue. grant_len={} {}", grant_len, sdu_queue);
    return {};
  }

  size_t sdu_len = sdu.buf.length();
  srsran_sanity_check(sdu_len == front_len, "Length mismatch. sdu_len={} front_len={}", sdu_len, front_len);

  // In TM there is no header, just pass the plain SDU
  byte_buffer_chain pdu_buf{std::move(sdu.buf)};

  logger.log_info(pdu_buf.begin(), pdu_buf.end(), "TX PDU. pdu_len={} grant_len={}", pdu_buf.length(), grant_len);

  // Update metrics
  metrics.metrics_add_pdus(1, pdu_buf.length());

  pcap.push_pdu(pcap_context, pdu_buf);

  return pdu_buf;
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
  return sdu_queue.size_bytes();
}
