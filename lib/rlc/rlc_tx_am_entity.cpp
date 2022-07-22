/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_tx_am_entity.h"

using namespace srsgnb;

rlc_tx_am_entity::rlc_tx_am_entity(du_ue_index_t                        du_index,
                                   lcid_t                               lcid,
                                   const rlc_tx_am_config&              config,
                                   rlc_tx_upper_layer_control_notifier& upper_cn,
                                   rlc_tx_buffer_state_update_notifier& buffer_state_notif,
                                   timer_manager&                       timers) :
  rlc_tx_entity(du_index, lcid, upper_cn, buffer_state_notif),
  cfg(config),
  mod(cardinality(to_number(cfg.sn_field_length))),
  head_min_size(rlc_am_pdu_header_min_size(cfg.sn_field_length)),
  head_max_size(rlc_am_pdu_header_max_size(cfg.sn_field_length)),
  poll_retransmit_timer(timers.create_unique_timer())
{
  // check timer t_poll_retransmission timer
  srsgnb_assert(poll_retransmit_timer.is_valid(), "Cannot create RLC RX AM: timers not configured");

  //  configure t_poll_retransmission timer
  if (cfg.t_poll_retx > 0) {
    poll_retransmit_timer.set(static_cast<uint32_t>(cfg.t_poll_retx),
                              [this](uint32_t timerid) { timer_expired(timerid); });
  }
}

// TS 38.322 v16.2.0 Sec. 5.2.3.1
void rlc_tx_am_entity::handle_sdu(rlc_sdu sdu)
{
  size_t sdu_length = sdu.buf.length();
  logger.log_info(sdu.buf.begin(),
                  sdu.buf.end(),
                  "Tx SDU (length: {} B, PDCP SN: {}, enqueued SDUs: {}",
                  sdu.buf.length(),
                  sdu.pdcp_sn,
                  sdu_queue.size_sdus());
  if (sdu_queue.write(sdu)) {
    metrics_add_sdus(1, sdu_length);
  } else {
    logger.log_warning(
        "Dropped Tx SDU (length: {} B, PDCP SN: {}, enqueued SDUs: {}", sdu_length, sdu.pdcp_sn, sdu_queue.size_sdus());
    metrics_add_lost_sdus(1);
  }
}

// TS 38.322 v16.2.0 Sec. 5.2.3.1
byte_buffer_slice_chain rlc_tx_am_entity::pull_pdu(uint32_t nof_bytes)
{
  // TODO
  (void)sn_under_segmentation;
  byte_buffer_slice_chain pdu = {};
  return pdu;
}

// TS 38.322 v16.2.0 Sec 5.5
uint32_t rlc_tx_am_entity::get_buffer_state()
{
  std::lock_guard<std::mutex> lock(mutex);
  return get_buffer_state_nolock();
}

void rlc_tx_am_entity::handle_buffer_state_update()
{
  std::lock_guard<std::mutex> lock(mutex);
  uint32_t                    bytes = get_buffer_state_nolock();
  buffer_state_notifier.on_buffer_state_update(bytes);
}

uint32_t rlc_tx_am_entity::get_buffer_state_nolock()
{
  // minimum bytes needed to tx all queued SDUs + each header
  uint32_t queue_bytes = 0;
  // TODO: SDU queue

  // minimum bytes needed to tx SDU under segmentation + header (if applicable)
  uint32_t segment_bytes = 0;
  // TODO: SDU under segmentation

  // TODO: retx bytes
  uint32_t retx_bytes = 0;

  // TODO: status report size
  uint32_t status_bytes = 0;

  return queue_bytes + segment_bytes + retx_bytes + status_bytes;
}

void rlc_tx_am_entity::timer_expired(uint32_t timeout_id)
{
  // TODO
}
