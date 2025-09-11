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

#include "rlc_tx_um_entity.h"
#include "rlc_um_pdu.h"
#include "srsran/pdcp/pdcp_sn_util.h"
#include "srsran/ran/pdsch/pdsch_constants.h"

using namespace srsran;

rlc_tx_um_entity::rlc_tx_um_entity(gnb_du_id_t                          du_id,
                                   du_ue_index_t                        ue_index,
                                   rb_id_t                              rb_id_,
                                   const rlc_tx_um_config&              config,
                                   rlc_tx_upper_layer_data_notifier&    upper_dn_,
                                   rlc_tx_upper_layer_control_notifier& upper_cn_,
                                   rlc_tx_lower_layer_notifier&         lower_dn_,
                                   rlc_bearer_metrics_collector&        metrics_coll_,
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
                metrics_coll_,
                pcap_,
                pcell_executor_,
                ue_executor_,
                timers),
  cfg(config),
  sdu_queue(cfg.queue_size, cfg.queue_size_bytes, logger),
  mod(cardinality(to_number(cfg.sn_field_length))),
  head_len_full(rlc_um_pdu_header_size_complete_sdu),
  head_len_first(rlc_um_pdu_header_size_no_so(cfg.sn_field_length)),
  head_len_not_first(rlc_um_pdu_header_size_with_so(cfg.sn_field_length)),
  pcap_context(ue_index, rb_id_, config)
{
  metrics_low.metrics_set_mode(rlc_mode::um_bidir);

  // check PDCP SN length
  srsran_assert(config.pdcp_sn_len == pdcp_sn_size::size12bits || config.pdcp_sn_len == pdcp_sn_size::size18bits,
                "Cannot create RLC TX AM, unsupported pdcp_sn_len={}. du={} ue={} {}",
                config.pdcp_sn_len,
                fmt::underlying(du_id),
                fmt::underlying(ue_index),
                rb_id);

  logger.log_info("RLC UM configured. {}", cfg);
}

// TS 38.322 v16.2.0 Sec. 5.2.2.1
void rlc_tx_um_entity::handle_sdu(byte_buffer sdu_buf, bool is_retx)
{
  rlc_sdu sdu_;
  sdu_.time_of_arrival = std::chrono::steady_clock::now();

  sdu_.buf     = std::move(sdu_buf);
  sdu_.pdcp_sn = get_pdcp_sn(sdu_.buf, cfg.pdcp_sn_len, /* is_srb = */ false, logger.get_basic_logger());

  // Sanity check for PDCP ReTx in RLC UM
  if (SRSRAN_UNLIKELY(is_retx)) {
    logger.log_error("Ignored unexpected PDCP retransmission flag in RLC UM SDU");
  }

  size_t sdu_length = sdu_.buf.length();
  if (sdu_queue.write(sdu_)) {
    logger.log_info(sdu_.buf.begin(),
                    sdu_.buf.end(),
                    "TX SDU. sdu_len={} pdcp_sn={} {}",
                    sdu_.buf.length(),
                    sdu_.pdcp_sn,
                    sdu_queue.get_state());
    metrics_high.metrics_add_sdus(1, sdu_length);
    handle_changed_buffer_state();
  } else {
    logger.log_info("Dropped SDU. sdu_len={} pdcp_sn={} {}", sdu_length, sdu_.pdcp_sn, sdu_queue.get_state());
    metrics_high.metrics_add_lost_sdus(1);
  }
}

// TS 38.322 v16.2.0 Sec. 5.4
void rlc_tx_um_entity::discard_sdu(uint32_t pdcp_sn)
{
  if (sdu_queue.try_discard(pdcp_sn)) {
    logger.log_info("Discarded SDU. pdcp_sn={}", pdcp_sn);
    metrics_high.metrics_add_discard(1);
    handle_changed_buffer_state();
  } else {
    logger.log_info("Could not discard SDU. pdcp_sn={}", pdcp_sn);
    metrics_high.metrics_add_discard_failure(1);
  }
}

// TS 38.322 v16.2.0 Sec. 5.2.2.1
size_t rlc_tx_um_entity::pull_pdu(span<uint8_t> mac_sdu_buf) noexcept SRSRAN_RTSAN_NONBLOCKING

{
  uint32_t grant_len = mac_sdu_buf.size();
  logger.log_debug("MAC opportunity. grant_len={}", grant_len);

  std::chrono::time_point<std::chrono::steady_clock> pull_begin;
  if (metrics_low.is_enabled()) {
    pull_begin = std::chrono::steady_clock::now();
  }

  // Check available space -- we need at least the minimum header + 1 payload Byte
  if (grant_len <= head_len_full) {
    logger.log_debug("Cannot fit SDU into grant_len={}. head_len_full={}", grant_len, head_len_full);
    return 0;
  }

  // Get a new SDU, if none is currently being transmitted
  if (sdu.buf.empty()) {
    srsran_sanity_check(next_so == 0, "New TX SDU, but next_so={} > 0.", next_so);

    // Read new SDU
    if (not sdu_queue.read(sdu)) {
      logger.log_debug("SDU queue empty. grant_len={}", grant_len);
      return {};
    }
    rlc_sdu_queue_lockfree::state_t queue_state = sdu_queue.get_state();
    logger.log_debug("Read SDU. sn={} pdcp_sn={} sdu_len={} queue_state=[{}]",
                     st.tx_next,
                     sdu.pdcp_sn,
                     sdu.buf.length(),
                     queue_state);

    // Notify the upper layer about the beginning of the transfer of the current SDU
    if (sdu.pdcp_sn.has_value()) {
      // Use size of SDU queue for desired_buf_size
      //
      // From TS 38.425 Sec. 5.4.2.1:
      // - If the value of the desired buffer size is 0, the hosting node shall stop sending any data per bearer.
      // - If the value of the desired buffer size in b) above is greater than 0, (...) the hosting node may send up to
      //   this amount of data per bearer beyond the "Highest Transmitted NR PDCP SN" for RLC UM.
      upper_dn.on_transmitted_sdu(sdu.pdcp_sn.value(), cfg.queue_size_bytes);
    }
  }

  // Prepare header
  rlc_um_pdu_header header = {};
  header.sn                = st.tx_next;
  header.sn_size           = cfg.sn_field_length;
  header.so                = next_so;

  // Get SI and expected header size
  uint32_t expected_hdr_len = 0;
  if (not get_si_and_expected_header_size(next_so, sdu.buf.length(), grant_len, header.si, expected_hdr_len)) {
    logger.log_debug(
        "Cannot fit any payload into grant_len={}. expected_hdr_len={} si={}", grant_len, expected_hdr_len, header.si);
    return 0;
  }

  // Pack header
  size_t header_len = rlc_um_write_data_pdu_header(mac_sdu_buf, header);
  srsran_sanity_check(header_len = expected_hdr_len,
                      "Failed to write header. header_len={} expected_hdr_len={}",
                      header_len,
                      expected_hdr_len);

  // Calculate the amount of data to move
  uint32_t space       = grant_len - header_len;
  uint32_t payload_len = space >= sdu.buf.length() - next_so ? sdu.buf.length() - next_so : space;

  // Log PDU info
  logger.log_debug("Creating PDU. si={} payload_len={} header_len={} sdu_len={} grant_len={}",
                   header.si,
                   payload_len,
                   header_len,
                   sdu.buf.length(),
                   grant_len);

  // Assemble PDU
  size_t nwritten = copy_segments(byte_buffer_view{sdu.buf, next_so, payload_len},
                                  mac_sdu_buf.subspan(header_len, mac_sdu_buf.size() - header_len));
  if (nwritten == 0 || nwritten != payload_len) {
    logger.log_error("Could not write PDU payload. {} payload_len={} grant_len={}", header, payload_len, grant_len);
    return 0;
  }

  size_t pdu_size = header_len + nwritten;
  logger.log_info(mac_sdu_buf.data(), pdu_size, "TX PDU. {} pdu_size={} grant_len={}", header, pdu_size, grant_len);

  // Release SDU if needed
  if (header.si == rlc_si_field::full_sdu || header.si == rlc_si_field::last_segment) {
    // Recycle SDU buffer in non real-time UE executor
    if (!ue_executor.defer([sdu = std::move(sdu.buf)]() mutable {
          // leaving this scope will implicitly delete the SDU
        })) {
      logger.log_warning("Cannot release transmitted SDU in UE executor. Releasing from pcell executor.");
      sdu.buf.clear();
    }
    next_so = 0;
    if (metrics_low.is_enabled()) {
      auto sdu_latency =
          std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - sdu.time_of_arrival);
      metrics_low.metrics_add_sdu_latency_us(sdu_latency.count() / 1000);
      metrics_low.metrics_add_pulled_sdus(1);
    }
  } else {
    // advance SO offset
    next_so += payload_len;
  }

  // Update SN if needed
  if (header.si == rlc_si_field::last_segment) {
    st.tx_next = (st.tx_next + 1) % mod;
  }

  // Log state
  log_state(srslog::basic_levels::debug);

  // Write PCAP
  pcap.push_pdu(pcap_context, mac_sdu_buf.subspan(0, pdu_size));

  // Update metrics
  if (header.si == rlc_si_field::full_sdu) {
    metrics_low.metrics_add_pdus_no_segmentation(1, pdu_size);
  } else {
    metrics_low.metrics_add_pdus_with_segmentation_um(1, pdu_size);
  }
  if (metrics_low.is_enabled()) {
    auto pdu_latency =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - pull_begin);
    metrics_low.metrics_add_pdu_latency_ns(pdu_latency.count());
  }

  return pdu_size;
}

/// Helper to get SI of an PDU
bool rlc_tx_um_entity::get_si_and_expected_header_size(uint32_t      so,
                                                       uint32_t      sdu_len,
                                                       uint32_t      grant_len,
                                                       rlc_si_field& si,
                                                       uint32_t&     head_len) const
{
  if (so == 0) {
    // Can we transmit the SDU fully?
    if (sdu_len <= grant_len - head_len_full) {
      // We can transmit the SDU fully
      si       = rlc_si_field::full_sdu;
      head_len = head_len_full;
    } else {
      // We can't transmit the SDU fully
      si       = rlc_si_field::first_segment;
      head_len = head_len_first;
      // Sanity check, do we have enough bytes for header?
      if (grant_len <= head_len_first) {
        return false;
      }
    }
  } else {
    // Sanity check, do we have enough bytes for header?
    if (grant_len <= head_len_not_first) {
      return false;
    }
    head_len = head_len_not_first;
    // Can we transmit the SDU segment fully?
    if (sdu_len - next_so <= grant_len - head_len_not_first) {
      // We can transmit the SDU fully
      si = rlc_si_field::last_segment;
    } else {
      // We can't transmit the SDU fully
      si = rlc_si_field::middle_segment;
    }
  }
  return true;
}

void rlc_tx_um_entity::handle_changed_buffer_state()
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

void rlc_tx_um_entity::update_mac_buffer_state() noexcept SRSRAN_RTSAN_NONBLOCKING
{
  pending_buffer_state.clear(std::memory_order_seq_cst);
  rlc_buffer_state bs = get_buffer_state();
  if (bs.pending_bytes <= MAX_DL_PDU_LENGTH || prev_buffer_state.pending_bytes <= MAX_DL_PDU_LENGTH ||
      suspend_bs_notif_barring) {
    logger.log_debug("Sending buffer state update to lower layer. bs={}", bs);
    suspend_bs_notif_barring = false;
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
rlc_buffer_state rlc_tx_um_entity::get_buffer_state()
{
  rlc_buffer_state bs = {};

  // minimum bytes needed to tx all queued SDUs + each header
  rlc_sdu_queue_lockfree::state_t queue_state = sdu_queue.get_state();
  uint32_t                        queue_bytes = queue_state.n_bytes + queue_state.n_sdus * head_len_full;

  // minimum bytes needed to tx SDU under segmentation + header (if applicable)
  uint32_t segment_bytes = 0;
  if (not sdu.buf.empty()) {
    segment_bytes = (sdu.buf.length() - next_so) + head_len_not_first;
    bs.hol_toa    = sdu.time_of_arrival;
  } else {
    const rlc_sdu* next_sdu = sdu_queue.front();
    if (next_sdu != nullptr) {
      bs.hol_toa = next_sdu->time_of_arrival;
    }
  }

  bs.pending_bytes = queue_bytes + segment_bytes;
  if (bs.pending_bytes <= MAX_DL_PDU_LENGTH) {
    suspend_bs_notif_barring = true;
  }
  return bs;
}
