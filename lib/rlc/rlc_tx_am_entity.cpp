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

#include "rlc_tx_am_entity.h"
#include "../support/sdu_window_impl.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/event_tracing.h"
#include "srsran/support/srsran_assert.h"
#include <set>

using namespace srsran;

rlc_tx_am_entity::rlc_tx_am_entity(uint32_t                             du_index,
                                   du_ue_index_t                        ue_index,
                                   rb_id_t                              rb_id,
                                   const rlc_tx_am_config&              config,
                                   rlc_tx_upper_layer_data_notifier&    upper_dn_,
                                   rlc_tx_upper_layer_control_notifier& upper_cn_,
                                   rlc_tx_lower_layer_notifier&         lower_dn_,
                                   timer_factory                        timers,
                                   task_executor&                       pcell_executor_,
                                   task_executor&                       ue_executor_,
                                   bool                                 metrics_enabled_,
                                   rlc_pcap&                            pcap_) :
  rlc_tx_entity(du_index, ue_index, rb_id, upper_dn_, upper_cn_, lower_dn_, metrics_enabled_, pcap_),
  cfg(config),
  sdu_queue(cfg.queue_size),
  retx_queue(window_size(to_number(cfg.sn_field_length))),
  mod(cardinality(to_number(cfg.sn_field_length))),
  am_window_size(window_size(to_number(cfg.sn_field_length))),
  tx_window(create_tx_window(cfg.sn_field_length)),
  pdu_recycler(window_size(to_number(cfg.sn_field_length)), logger),
  head_min_size(rlc_am_pdu_header_min_size(cfg.sn_field_length)),
  head_max_size(rlc_am_pdu_header_max_size(cfg.sn_field_length)),
  poll_retransmit_timer(timers.create_timer()),
  is_poll_retransmit_timer_expired(false),
  pcell_executor(pcell_executor_),
  ue_executor(ue_executor_),
  pcap_context(ue_index, rb_id, config)
{
  metrics.metrics_set_mode(rlc_mode::am);

  // check timer t_poll_retransmission timer
  srsran_assert(poll_retransmit_timer.is_valid(), "Cannot create RLC TX AM, timers not configured.");

  //  configure t_poll_retransmission timer
  if (cfg.t_poll_retx > 0) {
    poll_retransmit_timer.set(std::chrono::milliseconds(cfg.t_poll_retx),
                              [this](timer_id_t tid) { on_expired_poll_retransmit_timer(); });
  }

  logger.log_info("RLC AM configured. {}", cfg);
}

// TS 38.322 v16.2.0 Sec. 5.2.3.1
void rlc_tx_am_entity::handle_sdu(rlc_sdu sdu)
{
  size_t sdu_length = sdu.buf.length();
  if (sdu_queue.write(sdu)) {
    logger.log_info(
        sdu.buf.begin(), sdu.buf.end(), "TX SDU. sdu_len={} pdcp_sn={} {}", sdu.buf.length(), sdu.pdcp_sn, sdu_queue);
    metrics.metrics_add_sdus(1, sdu_length);
    handle_changed_buffer_state();
  } else {
    logger.log_warning("Dropped SDU. sdu_len={} pdcp_sn={} {}", sdu_length, sdu.pdcp_sn, sdu_queue);
    metrics.metrics_add_lost_sdus(1);
  }
}

// TS 38.322 v16.2.0 Sec. 5.4
void rlc_tx_am_entity::discard_sdu(uint32_t pdcp_sn)
{
  if (sdu_queue.discard(pdcp_sn)) {
    logger.log_info("Discarded SDU. pdcp_sn={}", pdcp_sn);
    metrics.metrics_add_discard(1);
    handle_changed_buffer_state();
  } else {
    logger.log_info("Could not discard SDU. pdcp_sn={}", pdcp_sn);
    metrics.metrics_add_discard_failure(1);
  }
}

// TS 38.322 v16.2.0 Sec. 5.2.3.1
size_t rlc_tx_am_entity::pull_pdu(span<uint8_t> rlc_pdu_buf)
{
  std::lock_guard<std::mutex> lock(mutex);

  const size_t grant_len = rlc_pdu_buf.size();
  logger.log_debug("MAC opportunity. grant_len={} tx_window_size={}", grant_len, tx_window->size());

  // TX STATUS if requested
  if (status_provider->status_report_required()) {
    if (grant_len < rlc_am_nr_status_pdu_sizeof_header_ack_sn) {
      logger.log_debug("Cannot fit status PDU into small grant_len={}.", grant_len);
      return 0;
    }
    rlc_am_status_pdu& status_pdu = status_provider->get_status_pdu(); // this also resets status_report_required

    if (status_pdu.get_packed_size() > grant_len) {
      if (not status_pdu.trim(grant_len)) {
        logger.log_warning("Could not trim status PDU down to grant_len={}.", grant_len);
        return 0;
      }
      logger.log_info("Trimmed status PDU to fit into grant_len={}.", grant_len);
      logger.log_debug("Trimmed status PDU. {}", status_pdu);
    }
    size_t pdu_len = status_pdu.pack(rlc_pdu_buf);
    if (pdu_len == 0) {
      logger.log_error("Could not pack status pdu. {}", status_pdu);
      return 0;
    }
    logger.log_info(rlc_pdu_buf.data(), pdu_len, "TX status PDU. pdu_len={} grant_len={}", pdu_len, grant_len);

    // Update metrics
    metrics.metrics_add_ctrl_pdus(1, pdu_len);

    // Log state
    log_state(srslog::basic_levels::debug);

    pcap.push_pdu(pcap_context, rlc_pdu_buf.subspan(0, pdu_len));
    return pdu_len;
  }

  // Retransmit if required
  if (not retx_queue.empty()) {
    logger.log_debug("Re-transmission required. retx_queue_size={}", retx_queue.size());

    size_t pdu_len = build_retx_pdu(rlc_pdu_buf);
    pcap.push_pdu(pcap_context, rlc_pdu_buf.subspan(0, pdu_len));
    return pdu_len;
  }

  // Send remaining segment, if it exists
  if (sn_under_segmentation != INVALID_RLC_SN) {
    if (tx_window->has_sn(sn_under_segmentation)) {
      size_t pdu_len = build_continued_sdu_segment(rlc_pdu_buf, (*tx_window)[sn_under_segmentation]);
      pcap.push_pdu(pcap_context, rlc_pdu_buf.subspan(0, pdu_len));
      return pdu_len;
    }
    sn_under_segmentation = INVALID_RLC_SN;
    logger.log_error("SDU under segmentation does not exist in tx_window. sn={}", sn_under_segmentation);
    // attempt to send next SDU
  }

  // Check whether there is something to TX
  if (sdu_queue.is_empty()) {
    logger.log_debug("SDU queue empty. grant_len={}", grant_len);
    return 0;
  }

  size_t pdu_len = build_new_pdu(rlc_pdu_buf);
  pcap.push_pdu(pcap_context, rlc_pdu_buf.subspan(0, pdu_len));
  return pdu_len;
}

size_t rlc_tx_am_entity::build_new_pdu(span<uint8_t> rlc_pdu_buf)
{
  const size_t grant_len = rlc_pdu_buf.size();

  if (grant_len <= head_min_size) {
    logger.log_debug("Cannot fit SDU into grant_len={}. head_min_size={}", grant_len, head_min_size);
    return 0;
  }

  // do not build any more PDU if window is already full
  if (is_tx_window_full()) {
    logger.log_warning("Cannot build data PDU, tx_window is full. grant_len={}", grant_len);
    return 0;
  }

  // Read new SDU from TX queue
  rlc_sdu sdu;
  logger.log_debug("Reading SDU from sdu_queue. {}", sdu_queue);
  if (not sdu_queue.read(sdu)) {
    logger.log_debug("SDU queue empty. grant_len={}", grant_len);
    return 0;
  }
  logger.log_debug("Read SDU. sn={} pdcp_sn={} sdu_len={}", st.tx_next, sdu.pdcp_sn, sdu.buf.length());

  // insert newly assigned SN into window and use reference for in-place operations
  // NOTE: from now on, we can't return from this function anymore before increasing tx_next
  rlc_tx_am_sdu_info& sdu_info = tx_window->add_sn(st.tx_next);
  sdu_info.pdcp_sn             = sdu.pdcp_sn;
  sdu_info.sdu                 = std::move(sdu.buf); // Move SDU into TX window SDU info

  // Notify the upper layer about the beginning of the transfer of the current SDU
  if (sdu.pdcp_sn.has_value()) {
    upper_dn.on_transmitted_sdu(sdu.pdcp_sn.value());
  }

  // Segment new SDU if necessary
  if (sdu_info.sdu.length() + head_min_size > grant_len) {
    return build_first_sdu_segment(rlc_pdu_buf, sdu_info);
  }
  logger.log_debug("Creating PDU with full SDU. sdu_len={} grant_len={}", sdu_info.sdu.length(), grant_len);

  // Prepare header
  rlc_am_pdu_header hdr = {};
  hdr.dc                = rlc_dc_field::data;
  hdr.p                 = get_polling_bit(st.tx_next, /* is_retx = */ false, sdu.buf.length());
  hdr.si                = rlc_si_field::full_sdu;
  hdr.sn_size           = cfg.sn_field_length;
  hdr.sn                = st.tx_next;

  // Pack header
  size_t header_len = rlc_am_write_data_pdu_header(rlc_pdu_buf, hdr);
  if (header_len == 0) {
    logger.log_error("Could not pack RLC header. hdr={}", hdr);
    return 0;
  }

  // Assemble PDU
  size_t payload_len = copy_segments(sdu_info.sdu, rlc_pdu_buf.subspan(header_len, rlc_pdu_buf.size() - header_len));
  if (payload_len == 0) {
    logger.log_error(
        "Could not write PDU payload. {} payload_len={} grant_len={}", hdr, sdu_info.sdu.length(), grant_len);
    return 0;
  }

  size_t pdu_len = header_len + payload_len;
  logger.log_info(rlc_pdu_buf.data(), pdu_len, "TX PDU. {} pdu_len={} grant_len={}", hdr, pdu_len, grant_len);

  // Update TX Next
  st.tx_next = (st.tx_next + 1) % mod;

  // Update metrics
  metrics.metrics_add_pdus_no_segmentation(1, pdu_len);

  // Log state
  log_state(srslog::basic_levels::debug);

  return pdu_len;
}

size_t rlc_tx_am_entity::build_first_sdu_segment(span<uint8_t> rlc_pdu_buf, rlc_tx_am_sdu_info& sdu_info)
{
  const size_t grant_len = rlc_pdu_buf.size();
  logger.log_debug("Creating PDU with first SDU segment. sdu_len={} grant_len={}", sdu_info.sdu.length(), grant_len);

  // Sanity check: can this SDU be sent this in a single PDU?
  if ((sdu_info.sdu.length() + head_min_size) <= grant_len) {
    logger.log_error("Unnecessary segmentation. sdu_len={} grant_len={}", sdu_info.sdu.length(), grant_len);
    return {};
  }

  // Sanity check: can this SDU be sent considering header overhead?
  if (grant_len <= head_min_size) { // Small header, since first segment has no SO field, ref: TS 38.322 Sec. 6.2.2.4
    logger.log_debug("Cannot fit first SDU segment into grant_len={}. head_min_size={}", grant_len, head_min_size);
    return {};
  }

  uint32_t segment_payload_len = grant_len - head_min_size;

  // Save SN of SDU under segmentation
  // This needs to be done before calculating the polling bit
  // To make sure we check correctly that the buffers are empty.
  sn_under_segmentation = st.tx_next;

  // Prepare header
  rlc_am_pdu_header hdr = {};
  hdr.dc                = rlc_dc_field::data;
  hdr.p                 = get_polling_bit(st.tx_next, false, segment_payload_len);
  hdr.si                = rlc_si_field::first_segment;
  hdr.sn_size           = cfg.sn_field_length;
  hdr.sn                = st.tx_next;
  hdr.so                = sdu_info.next_so;

  // Pack header
  size_t header_len = rlc_am_write_data_pdu_header(rlc_pdu_buf, hdr);
  if (header_len == 0) {
    logger.log_error("Could not pack RLC header. hdr={}", hdr);
    return 0;
  }

  // Assemble PDU
  size_t payload_len = copy_segments(byte_buffer_view{sdu_info.sdu, hdr.so, segment_payload_len},
                                     rlc_pdu_buf.subspan(header_len, rlc_pdu_buf.size() - header_len));
  if (payload_len == 0 || payload_len != segment_payload_len) {
    logger.log_error(
        "Could not write PDU payload. {} payload_len={} grant_len={}", hdr, sdu_info.sdu.length(), grant_len);
    return 0;
  }

  size_t pdu_len = header_len + payload_len;
  logger.log_info(rlc_pdu_buf.data(), pdu_len, "TX PDU. {} pdu_len={} grant_len={}", hdr, pdu_len, grant_len);

  // Store segmentation progress
  sdu_info.next_so += segment_payload_len;

  // Update metrics
  metrics.metrics_add_pdus_with_segmentation_am(1, pdu_len);

  // Log state
  log_state(srslog::basic_levels::debug);

  return pdu_len;
}

size_t rlc_tx_am_entity::build_continued_sdu_segment(span<uint8_t> rlc_pdu_buf, rlc_tx_am_sdu_info& sdu_info)
{
  const size_t grant_len = rlc_pdu_buf.size();
  logger.log_debug("Creating PDU with continued SDU segment. sn={} next_so={} sdu_len={} grant_len={}",
                   sn_under_segmentation,
                   sdu_info.next_so,
                   sdu_info.sdu.length(),
                   grant_len);

  // Sanity check: is there an initial SDU segment?
  if (sdu_info.next_so == 0) {
    logger.log_error(
        "Attempted to continue segmentation, but there was no initial segment. sn={} sdu_len={} grant_len={}",
        sn_under_segmentation,
        sdu_info.sdu.length(),
        grant_len);
    sn_under_segmentation = INVALID_RLC_SN;
    return {};
  }

  // Sanity check: last byte must be smaller than SDU size
  if (sdu_info.next_so >= sdu_info.sdu.length()) {
    logger.log_error("Segmentation progress next_so={} exceeds sdu_len={}. sn={} grant_len={}",
                     sdu_info.next_so,
                     sdu_info.sdu.length(),
                     sn_under_segmentation,
                     grant_len);
    sn_under_segmentation = INVALID_RLC_SN;
    return {};
  }

  // Sanity check: can this SDU be sent considering header overhead?
  if (grant_len <= head_max_size) { // Large header, since continued segment has SO field, ref: TS 38.322 Sec. 6.2.2.4
    logger.log_debug("Cannot fit continued SDU segment into grant_len={}. head_max_size={}", grant_len, head_max_size);
    return {};
  }

  uint32_t     segment_payload_len = sdu_info.sdu.length() - sdu_info.next_so;
  rlc_si_field si                  = {};

  if (segment_payload_len + head_max_size > grant_len) {
    si                  = rlc_si_field::middle_segment;
    segment_payload_len = grant_len - head_max_size;
  } else {
    si = rlc_si_field::last_segment;

    // Release SN of SDU under segmentation
    sn_under_segmentation = INVALID_RLC_SN;
  }

  // Prepare header
  rlc_am_pdu_header hdr = {};
  hdr.dc                = rlc_dc_field::data;
  hdr.p                 = get_polling_bit(st.tx_next, false, segment_payload_len);
  hdr.si                = si;
  hdr.sn_size           = cfg.sn_field_length;
  hdr.sn                = st.tx_next;
  hdr.so                = sdu_info.next_so;

  // Pack header
  size_t header_len = rlc_am_write_data_pdu_header(rlc_pdu_buf, hdr);
  if (header_len == 0) {
    logger.log_error("Could not pack RLC header. hdr={}", hdr);
    return 0;
  }

  // Assemble PDU
  size_t payload_len = copy_segments(byte_buffer_view{sdu_info.sdu, hdr.so, segment_payload_len},
                                     rlc_pdu_buf.subspan(header_len, rlc_pdu_buf.size() - header_len));
  if (payload_len == 0 || payload_len != segment_payload_len) {
    logger.log_error(
        "Could not write PDU payload. {} payload_len={} grant_len={}", hdr, sdu_info.sdu.length(), grant_len);
    return 0;
  }

  size_t pdu_len = header_len + payload_len;
  logger.log_info(rlc_pdu_buf.data(), pdu_len, "TX PDU. {} pdu_len={} grant_len={}", hdr, pdu_len, grant_len);

  // Store segmentation progress
  sdu_info.next_so += segment_payload_len;

  // Update TX Next (when segmentation has finished)
  if (si == rlc_si_field::last_segment) {
    st.tx_next = (st.tx_next + 1) % mod;
  }

  // Update metrics
  metrics.metrics_add_pdus_with_segmentation_am(1, pdu_len);

  // Log state
  log_state(srslog::basic_levels::debug);

  return pdu_len;
}

size_t rlc_tx_am_entity::build_retx_pdu(span<uint8_t> rlc_pdu_buf)
{
  const size_t grant_len = rlc_pdu_buf.size();

  // Check there is at least 1 element before calling front()
  if (retx_queue.empty()) {
    logger.log_error("Called build_retx_pdu() but retx_queue is empty.");
    return 0;
  }

  // Sanity check - drop any retx SNs not present in tx_window
  while (not tx_window->has_sn(retx_queue.front().sn)) {
    logger.log_info("Could not find sn={} in tx window, dropping RETX.", retx_queue.front().sn);
    retx_queue.pop();
    if (retx_queue.empty()) {
      logger.log_info("Empty retx_queue, cannot provide any PDU for retransmission.");
      return 0;
    }
  }

  const rlc_tx_amd_retx retx = retx_queue.front(); // local copy, since front may change below
  logger.log_debug("Processing RETX. {}", retx);
  retx_sn = retx.sn;

  // Get sdu_info info from tx_window
  rlc_tx_am_sdu_info& sdu_info = (*tx_window)[retx.sn];

  // Check RETX boundaries
  if (retx.so + retx.length > sdu_info.sdu.length()) {
    logger.log_error("Skipping invalid RETX that exceeds SDU boundaries. {} sdu_len={} grant_len={}",
                     retx,
                     sdu_info.sdu.length(),
                     grant_len);
    retx_queue.pop();
    return 0;
  }

  // Get expected header length
  size_t expected_hdr_len = get_retx_expected_hdr_len(retx);
  // Sanity check: can this RETX be sent considering header overhead?
  if (grant_len <= expected_hdr_len) {
    logger.log_debug("Cannot fit RETX SDU into grant_len={}. expected_hdr_len={}", grant_len, expected_hdr_len);
    return 0;
  }

  // Compute maximum payload length
  size_t retx_payload_len = std::min(static_cast<size_t>(retx.length), grant_len - expected_hdr_len);
  bool   retx_complete    = retx_payload_len == retx.length;
  bool   sdu_complete     = retx.so + retx_payload_len >= sdu_info.sdu.length();

  // Configure SI
  rlc_si_field si = rlc_si_field::full_sdu;
  if (retx.so == 0) {
    // either full SDU or first segment
    if (sdu_complete) {
      si = rlc_si_field::full_sdu;
    } else {
      si = rlc_si_field::first_segment;
    }
  } else {
    // either middle segment or last segment
    if (sdu_complete) {
      si = rlc_si_field::last_segment;
    } else {
      si = rlc_si_field::middle_segment;
    }
  }

  // Log RETX info
  logger.log_debug("Creating RETX PDU. {} si={} retx_payload_len={} expected_hdr_len={} grant_len={}",
                   retx,
                   si,
                   retx_payload_len,
                   expected_hdr_len,
                   grant_len);

  // Update RETX queue. This must be done before calculating
  // the polling bit, to make sure the poll bit is calculated correctly
  if (retx_complete) {
    // remove RETX from queue
    retx_queue.pop();
  } else {
    // update SO and length of front element
    rlc_tx_amd_retx retx_remainder = retx_queue.front();
    retx_remainder.so += retx_payload_len;
    retx_remainder.length -= retx_payload_len;
    retx_queue.replace_front(retx_remainder);
  }

  // Prepare header
  rlc_am_pdu_header hdr = {};
  hdr.dc                = rlc_dc_field::data;
  hdr.p                 = get_polling_bit(retx.sn, /* is_retx = */ true, 0);
  hdr.si                = si;
  hdr.sn_size           = cfg.sn_field_length;
  hdr.sn                = retx.sn;
  hdr.so                = retx.so;

  // Pack header
  size_t header_len = rlc_am_write_data_pdu_header(rlc_pdu_buf, hdr);
  if (header_len == 0) {
    logger.log_error("Could not pack RLC header. hdr={}", hdr);
    return 0;
  }
  srsran_assert(header_len == expected_hdr_len,
                "RETX header_len={} differs from expected_hdr_len={}",
                header_len,
                expected_hdr_len);

  // Assemble PDU
  size_t payload_len = copy_segments(byte_buffer_view{sdu_info.sdu, hdr.so, retx_payload_len},
                                     rlc_pdu_buf.subspan(header_len, rlc_pdu_buf.size() - header_len));
  if (payload_len == 0 || payload_len != retx_payload_len) {
    logger.log_error(
        "Could not write PDU payload. {} payload_len={} grant_len={}", hdr, sdu_info.sdu.length(), grant_len);
    return 0;
  }

  size_t pdu_len = header_len + payload_len;
  logger.log_info(rlc_pdu_buf.data(),
                  pdu_len,
                  "RETX PDU. {} pdu_len={} grant_len={} retx_count={}",
                  hdr,
                  pdu_len,
                  grant_len,
                  sdu_info.retx_count);

  // Update metrics
  metrics.metrics_add_retx_pdus(1, pdu_len);

  // Log state
  log_state(srslog::basic_levels::debug);

  return pdu_len;
}

void rlc_tx_am_entity::on_status_pdu(rlc_am_status_pdu status)
{
  // Redirect handling of status to pcell_executor
  auto handle_func = [this, status = std::move(status)]() mutable { handle_status_pdu(std::move(status)); };
  if (not pcell_executor.execute(std::move(handle_func))) {
    logger.log_error("Unable to handle status report");
  }
}

void rlc_tx_am_entity::handle_status_pdu(rlc_am_status_pdu status)
{
  trace_point status_tp = l2_tracer.now();
  auto        t_start   = std::chrono::high_resolution_clock::now();

  std::lock_guard<std::mutex> lock(mutex);

  /*
   * Sanity check the received status report.
   * 1. Checking if the ACK_SN is inside the valid ACK_SN window (the TX window "off-by-one")
   * makes sure we discard out of order status reports (with different ACN_SNs).
   * 2. Checking if ACK_SN > Tx_Next + 1 makes sure we do not receive a ACK/NACK for something we did not TX
   * ACK_SN may be equal to TX_NEXT + 1, if not all SDU segments with SN=TX_NEXT have been transmitted.
   * 3. Checking if all NACK_SNs are valid. These can be invalid either due to issues on the sender,
   * or due to out-of-order status reports with the same ACK_SN.
   *
   * Note: dropping out-of-order status report may lose information as the more recent status report could
   * be trimmed. But if that is the case, the peer can always request another status report later on.
   */
  if (not valid_ack_sn(status.ack_sn)) {
    logger.log_info("Ignoring status report with ack_sn={} outside TX window. {}", status.ack_sn, st);
    return;
  }

  if (tx_mod_base(status.ack_sn) > tx_mod_base(st.tx_next + 1)) {
    logger.log_error("Ignoring status report with ack_sn={} > tx_next. {}", status.ack_sn, st);
    return;
  }

  if (!status.get_nacks().empty()) {
    for (const auto& nack : status.get_nacks()) {
      if (not valid_nack(status.ack_sn, nack)) {
        return;
      }
    }
  }

  auto on_function_exit = make_scope_exit([&]() {
    auto t_end    = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
    logger.log_info("Handled status report. t={}us {}", duration.count(), status);
  });

  /**
   * Section 5.3.3.3: Reception of a STATUS report
   * - if the STATUS report comprises a positive or negative acknowledgement for the RLC SDU with sequence
   *   number equal to POLL_SN:
   *   - if t-PollRetransmit is running:
   *     - stop and reset t-PollRetransmit.
   */
  if (tx_mod_base(st.poll_sn) < tx_mod_base(status.ack_sn)) {
    if (poll_retransmit_timer.is_running()) {
      logger.log_debug("Received ACK or NACK for poll_sn={}. Stopping t-PollRetransmit.", st.poll_sn);
      poll_retransmit_timer.stop();
    } else {
      logger.log_debug("Received ACK or NACK for poll_sn={}. t-PollRetransmit already notify_stop.", st.poll_sn);
    }
  } else {
    logger.log_debug("poll_sn={} > ack_sn={}. Not stopping t-PollRetransmit.", st.poll_sn, status.ack_sn);
  }

  /*
   * - if the SN of the corresponding RLC SDU falls within the range
   *   TX_Next_Ack <= SN < = the highest SN of the AMD PDU among the AMD PDUs submitted to lower layer:
   *   - consider the RLC SDU or the RLC SDU segment for which a negative acknowledgement was received for
   *     retransmission.
   */
  // Process ACKs
  uint32_t stop_sn = status.get_nacks().size() == 0
                         ? status.ack_sn
                         : status.get_nacks()[0].nack_sn; // Stop processing ACKs at the first NACK, if it exists.

  optional<uint32_t> max_deliv_pdcp_sn = {}; // initialize with not value set
  bool               recycle_bin_full  = false;
  for (uint32_t sn = st.tx_next_ack; tx_mod_base(sn) < tx_mod_base(stop_sn); sn = (sn + 1) % mod) {
    if (tx_window->has_sn(sn)) {
      rlc_tx_am_sdu_info& sdu_info = (*tx_window)[sn];
      if (sdu_info.pdcp_sn.has_value()) {
        max_deliv_pdcp_sn = (*tx_window)[sn].pdcp_sn;
      }
      // move the PDU's byte_buffer from tx_window into pdu_recycler (if possible) for deletion off the critical path.
      if (!pdu_recycler.add_discarded_pdu(std::move(sdu_info.sdu))) {
        // recycle bin is full and the PDU was deleted on the spot, which may slow down this worker. Warn later.
        recycle_bin_full = true;
      }
      tx_window->remove_sn(sn); // remove the from tx_window
      st.tx_next_ack = (sn + 1) % mod;
    } else {
      logger.log_error("Could not find ACK'ed sn={} in TX window.", sn);
      break;
    }
  }
  if (max_deliv_pdcp_sn.has_value()) {
    upper_dn.on_delivered_sdu(max_deliv_pdcp_sn.value());
  }
  logger.log_debug("Processed status report ACKs. ack_sn={} tx_next_ack={}", status.ack_sn, st.tx_next_ack);
  if (recycle_bin_full) {
    logger.log_warning("Could not postpone recycling of PDU byte_buffers. Performance can be impaired.");
  }

  // Clear ReTx queue and rewind retx_counter (once per SN unless not started to ReTx)
  while (!retx_queue.empty()) {
    const rlc_tx_amd_retx& retx = retx_queue.front();
    if (retx_sn != retx.sn) {
      if (tx_window->has_sn(retx.sn)) {
        decrement_retx_count(retx.sn);
      }
      retx_sn = retx.sn;
    }
    retx_queue.pop();
  }
  retx_sn = INVALID_RLC_SN;

  // Rebuild ReTx queue by processing the NACKs
  uint32_t prev_retx_sn = INVALID_RLC_SN; // Stores the most recent SN that was considered for ReTx
  for (uint32_t nack_idx = 0; nack_idx < status.get_nacks().size(); nack_idx++) {
    if (status.get_nacks()[nack_idx].has_nack_range) {
      for (uint32_t range_sn = status.get_nacks()[nack_idx].nack_sn;
           range_sn != (status.get_nacks()[nack_idx].nack_sn + status.get_nacks()[nack_idx].nack_range) % mod;
           range_sn = (range_sn + 1) % mod) {
        // Sanity check
        if (range_sn == status.ack_sn) {
          logger.log_warning(
              "Truncating invalid NACK range at ack_sn={}. nack={}", status.ack_sn, status.get_nacks()[nack_idx]);
          break;
        }
        rlc_am_status_nack nack = {};
        nack.nack_sn            = range_sn;
        if (status.get_nacks()[nack_idx].has_so) {
          // Apply so_start to first range item
          if (range_sn == status.get_nacks()[nack_idx].nack_sn) {
            nack.so_start = status.get_nacks()[nack_idx].so_start;
          }
          // Apply so_end to last range item
          if (range_sn == (status.get_nacks()[nack_idx].nack_sn + status.get_nacks()[nack_idx].nack_range - 1) % mod) {
            nack.so_end = status.get_nacks()[nack_idx].so_end;
          }
          // Enable has_so only if the offsets do not span the whole SDU
          nack.has_so = (nack.so_start != 0) || (nack.so_end != rlc_am_status_nack::so_end_of_sdu);
        }
        if (handle_nack(nack)) {
          // increment retx_count (only once per SN) and inform upper layers when needed
          uint32_t nack_sn = nack.nack_sn;
          if (prev_retx_sn != nack_sn) {
            increment_retx_count(nack_sn);
            prev_retx_sn = nack_sn;
          }
        }
      }
    } else {
      if (handle_nack(status.get_nacks()[nack_idx])) {
        // increment retx_count (only once per SN) and inform upper layers when needed
        uint32_t nack_sn = status.get_nacks()[nack_idx].nack_sn;
        if (prev_retx_sn != nack_sn) {
          increment_retx_count(nack_sn);
          prev_retx_sn = nack_sn;
        }
      }
    }
  }

  l2_tracer << trace_event{"handle_status", status_tp};

  update_mac_buffer_state(/* is_locked = */ true);

  // Trigger recycling of discarded PDUs in ue_executor
  pdu_recycler.clear_by_executor(ue_executor);
}

void rlc_tx_am_entity::on_status_report_changed()
{
  handle_changed_buffer_state();
}

bool rlc_tx_am_entity::handle_nack(rlc_am_status_nack nack)
{
  if (nack.has_nack_range) {
    logger.log_error("handle_nack must not be called with nacks that have a nack range. Ignoring nack={}.", nack);
    return false;
  }

  logger.log_debug("Handling nack={}.", nack);

  // Check if NACK applies to a SN within tx window
  if (!(tx_mod_base(st.tx_next_ack) <= tx_mod_base(nack.nack_sn) &&
        tx_mod_base(nack.nack_sn) <= tx_mod_base(st.tx_next))) {
    logger.log_info("Invalid nack_sn={}. tx_next_ack={} tx_next={}", nack.nack_sn, st.tx_next_ack, st.tx_next);
    return false;
  }

  uint32_t sdu_length = (*tx_window)[nack.nack_sn].sdu.length();

  // Convert NACK for full SDUs into NACK with segment offset and length
  if (!nack.has_so) {
    nack.so_start = 0;
    nack.so_end   = sdu_length - 1;
  }
  // Replace "end"-mark with actual SDU length
  if (nack.so_end == rlc_am_status_nack::so_end_of_sdu) {
    nack.so_end = sdu_length - 1;
  }
  // Sanity checks
  if (nack.so_start > nack.so_end) {
    logger.log_warning("Invalid NACK with so_start > so_end. nack={}, sdu_length={}", nack, sdu_length);
    nack.so_start = 0;
  }
  if (nack.so_start >= sdu_length) {
    logger.log_warning("Invalid NACK with so_start >= sdu_length. nack={} sdu_length={}.", nack, sdu_length);
    nack.so_start = 0;
  }
  if (nack.so_end >= sdu_length) {
    logger.log_warning("Invalid NACK: so_end >= sdu_length. nack={}, sdu_length={}.", nack, sdu_length);
    nack.so_end = sdu_length - 1;
  }

  // Enqueue RETX
  rlc_tx_amd_retx retx = {};
  retx.so              = nack.so_start;
  retx.sn              = nack.nack_sn;
  retx.length          = nack.so_end - nack.so_start + 1;
  if (!retx_queue.try_push(retx)) {
    logger.log_warning(
        "Failed to schedule RETX for nack={}. Queue is full. {} retx_queue_len={}", nack, retx, retx_queue.size());
    return false;
  }
  logger.log_debug("Scheduled RETX for nack={}. {} retx_queue_len={}", nack, retx, retx_queue.size());
  return true;
}

void rlc_tx_am_entity::increment_retx_count(uint32_t sn)
{
  auto& pdu = (*tx_window)[sn];
  // Increment retx_count
  if (pdu.retx_count == RETX_COUNT_NOT_STARTED) {
    // Set retx_count = 0 on first RE-transmission of associated SDU (38.322 Sec. 5.3.2)
    pdu.retx_count = 0;
  } else {
    // Increment otherwise
    pdu.retx_count++;
  }

  // Inform upper layers if needed
  check_sn_reached_max_retx(sn);
}

void rlc_tx_am_entity::decrement_retx_count(uint32_t sn)
{
  auto& pdu = (*tx_window)[sn];
  if (pdu.retx_count == RETX_COUNT_NOT_STARTED) {
    return;
  }
  if (pdu.retx_count == 0) {
    pdu.retx_count = RETX_COUNT_NOT_STARTED;
    return;
  }
  pdu.retx_count--;
}

void rlc_tx_am_entity::check_sn_reached_max_retx(uint32_t sn)
{
  if ((*tx_window)[sn].retx_count == cfg.max_retx_thresh) {
    logger.log_warning("Reached maximum number of RETX. sn={} retx_count={}", sn, (*tx_window)[sn].retx_count);
    upper_cn.on_max_retx();
  }
}

void rlc_tx_am_entity::handle_changed_buffer_state()
{
  if (not pending_buffer_state.test_and_set(std::memory_order_seq_cst)) {
    logger.log_debug("Triggering buffer state update to lower layer");
    // Redirect handling of status to pcell_executor
    if (not pcell_executor.defer([this]() { update_mac_buffer_state(/* is_locked = */ false); })) {
      logger.log_error("Failed to enqueue buffer state update");
    }
  } else {
    logger.log_debug("Avoiding redundant buffer state update to lower layer");
  }
}

void rlc_tx_am_entity::update_mac_buffer_state(bool is_locked)
{
  pending_buffer_state.clear(std::memory_order_seq_cst);
  unsigned bs = is_locked ? get_buffer_state_nolock() : get_buffer_state();
  if (not(bs > MAX_DL_PDU_LENGTH && prev_buffer_state > MAX_DL_PDU_LENGTH)) {
    logger.log_debug("Sending buffer state update to lower layer. bs={}", bs);
    lower_dn.on_buffer_state_update(bs);
  } else {
    logger.log_debug(
        "Buffer state very large. Avoiding sending buffer state to lower layer. bs={} prev_buffer_state={}",
        bs,
        prev_buffer_state);
  }
  prev_buffer_state = bs;
}

// TS 38.322 v16.2.0 Sec 5.5
uint32_t rlc_tx_am_entity::get_buffer_state()
{
  std::lock_guard<std::mutex> lock(mutex);
  return get_buffer_state_nolock();
}

uint32_t rlc_tx_am_entity::get_buffer_state_nolock()
{
  // minimum bytes needed to tx all queued SDUs + each header
  uint32_t queue_bytes = sdu_queue.size_bytes() + sdu_queue.size_sdus() * head_min_size;

  // minimum bytes needed to tx SDU under segmentation + header (if applicable)
  uint32_t segment_bytes = 0;
  if (sn_under_segmentation != INVALID_RLC_SN) {
    if (tx_window->has_sn(sn_under_segmentation)) {
      rlc_tx_am_sdu_info& sdu_info = (*tx_window)[sn_under_segmentation];
      segment_bytes                = sdu_info.sdu.length() - sdu_info.next_so + head_max_size;
    } else {
      logger.log_info("Buffer state ignores SDU under segmentation. sn={} not in tx_window.", sn_under_segmentation);
    }
  }

  // minimum bytes needed to tx all queued RETX + each header; RETX can also be segments
  rlc_retx_queue_state retx_state = retx_queue.state();
  uint32_t             retx_bytes = retx_state.get_retx_bytes() + retx_state.get_n_retx_so_zero() * head_min_size +
                        retx_state.get_n_retx_so_nonzero() * head_max_size;

  // status report size
  uint32_t status_bytes = 0;
  if (status_provider->status_report_required()) {
    status_bytes = status_provider->get_status_pdu_length();
  }

  return queue_bytes + segment_bytes + retx_bytes + status_bytes;
}

uint8_t rlc_tx_am_entity::get_polling_bit(uint32_t sn, bool is_retx, uint32_t payload_size)
{
  logger.log_debug("Checking poll bit requirements for PDU. sn={} is_retx={} sdu_bytes={} poll_sn={}",
                   sn,
                   is_retx ? "true" : "false",
                   payload_size,
                   st.poll_sn);
  /* For each AMD PDU containing a SDU or SDU segment that has not been previoulsy tranmitted:
   * - increment PDU_WITHOUT_POLL by one;
   * - increment BYTE_WITHOUT_POLL by every new byte of Data field element that it maps to the Data field of the AMD
   * PDU;
   *   - if PDU_WITHOUT_POLL >= pollPDU; or
   *   - if BYTE_WITHOUT_POLL >= pollByte:
   *   	- include a poll in the AMD PDU as described below.
   */
  uint8_t poll = 0;
  if (!is_retx) {
    st.pdu_without_poll++;
    st.byte_without_poll += payload_size;
    if (cfg.poll_pdu > 0 && st.pdu_without_poll >= (uint32_t)cfg.poll_pdu) {
      poll = 1;
      logger.log_debug("Setting poll bit due to PollPDU. sn={} poll_sn={}", sn, st.poll_sn);
    }
    if (cfg.poll_byte > 0 && st.byte_without_poll >= (uint32_t)cfg.poll_byte) {
      poll = 1;
      logger.log_debug("Setting poll bit due to PollBYTE. sn={} poll_sn={}", sn, st.poll_sn);
    }
  }

  /*
   * - if both the transmission buffer and the retransmission buffer becomes empty
   *   (excluding transmitted RLC SDUs or RLC SDU segments awaiting acknowledgements)
   *   after the transmission of the AMD PDU; or
   * - if no new RLC SDU can be transmitted after the transmission of the AMD PDU (e.g. due to window stalling);
   *   - include a poll in the AMD PDU as described below.
   */
  if ((sdu_queue.is_empty() && retx_queue.empty() && sn_under_segmentation == INVALID_RLC_SN) || is_tx_window_full()) {
    logger.log_debug("Setting poll bit due to empty buffers/inablity to TX. sn={} poll_sn={}", sn, st.poll_sn);
    poll = 1;
  }

  /*
   * From Sec. 5.3.3.4 Expiry of t-PollRetransmit
   * [...]
   * - include a poll in an AMD PDU as described in clause 5.3.3.2.
   */
  if (is_poll_retransmit_timer_expired.exchange(false, std::memory_order_relaxed)) {
    logger.log_debug("Setting poll bit due to expired poll retransmit timer. sn={} poll_sn={}", sn, st.poll_sn);
    poll = 1;
  }

  /*
   * - If poll bit is included:
   *     - set PDU_WITHOUT_POLL to 0;
   *     - set BYTE_WITHOUT_POLL to 0.
   */
  if (poll == 1) {
    st.pdu_without_poll  = 0;
    st.byte_without_poll = 0;
    /*
     * - set POLL_SN to the highest SN of the AMD PDU among the AMD PDUs submitted to lower layer;
     * - if t-PollRetransmit is not running:
     *   - start t-PollRetransmit.
     * - else:
     *   - restart t-PollRetransmit.
     */
    if (!is_retx) {
      // This is not an RETX, but a new transmission
      // As such it should be the highest SN submitted to the lower layers
      st.poll_sn = sn;
      logger.log_debug("Updated poll_sn={}.", sn);
    }
    if (cfg.t_poll_retx > 0) {
      if (not poll_retransmit_timer.is_running()) {
        poll_retransmit_timer.run();
      } else {
        poll_retransmit_timer.stop();
        poll_retransmit_timer.run();
      }
      logger.log_debug("Started poll retransmit timer. poll_sn={}", st.poll_sn);
    }
  }
  return poll;
}

void rlc_tx_am_entity::on_expired_poll_retransmit_timer()
{
  std::unique_lock<std::mutex> lock(mutex);

  // t-PollRetransmit
  logger.log_info("Poll retransmit timer expired after {}ms.", poll_retransmit_timer.duration().count());
  log_state(srslog::basic_levels::debug);
  /*
   * - if both the transmission buffer and the retransmission buffer are empty
   *   (excluding transmitted RLC SDU or RLC SDU segment awaiting acknowledgements); or
   * - if no new RLC SDU or RLC SDU segment can be transmitted (e.g. due to window stalling):
   *   - consider the RLC SDU with the highest SN among the RLC SDUs submitted to lower layer for
   *   retransmission; or
   *   - consider any RLC SDU which has not been positively acknowledged for retransmission.
   */
  if ((sdu_queue.is_empty() && retx_queue.empty() && sn_under_segmentation == INVALID_RLC_SN) || is_tx_window_full()) {
    if (tx_window->empty()) {
      logger.log_info(
          "Poll retransmit timer expired, but the TX window is empty. {} tx_window_size={}", st, tx_window->size());
      return;
    }
    if (not tx_window->has_sn(st.tx_next_ack)) {
      logger.log_info("Poll retransmit timer expired, but tx_next_ack is not in the TX window. {} tx_window_size={}",
                      st,
                      tx_window->size());
      return;
    }
    // RETX first RLC SDU that has not been ACKed
    // or first SDU segment of the first RLC SDU
    // that has not been acked
    rlc_tx_amd_retx retx = {};
    retx.so              = 0;
    retx.sn              = st.tx_next_ack;
    retx.length          = (*tx_window)[st.tx_next_ack].sdu.length();
    bool retx_enqueued   = retx_queue.try_push(retx);
    //
    // TODO: Revise this: shall we send a minimum-sized segment instead?
    //

    if (retx_enqueued) {
      logger.log_debug(
          "Scheduled RETX due to expired poll retransmit timer. {} retx_queue_len={}", retx, retx_queue.size());
      increment_retx_count(st.tx_next_ack);
    } else {
      logger.log_warning(
          "Failed to schedule RETX at expired poll retransmit timer. Queue is full. {} retx_queue_len={}",
          retx,
          retx_queue.size());
    }

    update_mac_buffer_state(/* is_locked = */ true);
  }
  /*
   * - include a poll in an AMD PDU as described in clause 5.3.3.2.
   */
  is_poll_retransmit_timer_expired.store(true, std::memory_order_relaxed);
}

std::unique_ptr<sdu_window<rlc_tx_am_sdu_info>> rlc_tx_am_entity::create_tx_window(rlc_am_sn_size sn_size)
{
  std::unique_ptr<sdu_window<rlc_tx_am_sdu_info>> tx_window_;
  switch (sn_size) {
    case rlc_am_sn_size::size12bits:
      tx_window_ = std::make_unique<
          sdu_window_impl<rlc_tx_am_sdu_info, window_size(to_number(rlc_am_sn_size::size12bits)), rlc_bearer_logger>>(
          logger);
      break;
    case rlc_am_sn_size::size18bits:
      tx_window_ = std::make_unique<
          sdu_window_impl<rlc_tx_am_sdu_info, window_size(to_number(rlc_am_sn_size::size18bits)), rlc_bearer_logger>>(
          logger);
      break;
    default:
      srsran_assertion_failure("Cannot create tx_window for unsupported sn_size={}.", to_number(sn_size));
  }
  return tx_window_;
}

bool rlc_tx_am_entity::inside_tx_window(uint32_t sn) const
{
  // TX_Next_Ack <= SN < TX_Next_Ack + AM_Window_Size
  return tx_mod_base(sn) < am_window_size;
}

bool rlc_tx_am_entity::is_tx_window_full() const
{
  // TX window is full, or we reached our virtual max window size
  return tx_window->full() || (cfg.max_window != 0 && tx_mod_base(st.tx_next) > cfg.max_window);
}

bool rlc_tx_am_entity::valid_ack_sn(uint32_t sn) const
{
  // Tx_Next_Ack < SN <= TX_Next + AM_Window_Size
  return (0 < tx_mod_base(sn)) && (tx_mod_base(sn) <= am_window_size);
}

bool rlc_tx_am_entity::valid_nack(uint32_t ack_sn, const rlc_am_status_nack& nack) const
{
  // NACK_SN >= ACK_SN
  if (tx_mod_base(nack.nack_sn) >= tx_mod_base(ack_sn)) {
    logger.log_info("Ignoring status report with nack_sn={} >= ack_sn={}. {}", nack.nack_sn, ack_sn, st);
    return false;
  }
  // NACK_SN + range >= ACK_SN
  if (nack.has_nack_range) {
    if (tx_mod_base(nack.nack_sn + nack.nack_range - 1) >= tx_mod_base(ack_sn)) {
      logger.log_info("Ignoring status report with nack_sn={} + nack_range={} - 1 >= ack_sn={}. {}",
                      nack.nack_sn,
                      nack.nack_range,
                      ack_sn,
                      st);
      return false;
    }
  }
  // NACK_SN within TX Window
  if (not inside_tx_window(nack.nack_sn)) {
    logger.log_info("Ignoring status report with nack_sn={} outside TX window. {}", nack.nack_sn, st);
    return false;
  }
  // NACK_SN + range within TX Window
  if (nack.has_nack_range) {
    if (not inside_tx_window(nack.nack_sn + nack.nack_range - 1)) {
      logger.log_info("Ignoring status report with nack_sn={} + nack_range={} - 1 outside TX window. {}",
                      nack.nack_sn,
                      nack.nack_range,
                      st);
      return false;
    }
  }

  // It should not be possible for NACK_SN to be larger than TX_NEXT,
  // since we check earlier if ACK_SN > tx_next +1
  // * ACK_SN > TX_NEXT + 1 => drop
  //     * implies ACK_SN <= TX_NEXT
  // * NACK_SN >= ACK_SN => drop
  //     * implies NACK_SN < ACK_SN
  // * Therefore:
  //     * NACK_SN < ACK_SN <= TX_NEXT

  // NACK_SN >= tx_next
  if (tx_mod_base(nack.nack_sn) > tx_mod_base(st.tx_next)) {
    logger.log_error("Ignoring status report with nack_sn={} >= tx_next. {}", nack.nack_sn, st);
    return false;
  }
  // NACK_SN + range >= tx_next
  if (nack.has_nack_range) {
    if (tx_mod_base(nack.nack_sn + nack.nack_range - 1) > tx_mod_base(st.tx_next)) {
      logger.log_error("Ignoring status report with nack_sn={} + nack_range={} - 1 >= tx_next. {}",
                       nack.nack_sn,
                       nack.nack_range,
                       st);
      return false;
    }
  }
  return true;
}
