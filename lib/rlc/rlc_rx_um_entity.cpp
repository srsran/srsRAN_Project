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

#include "rlc_rx_um_entity.h"
#include "../support/sdu_window_impl.h"

using namespace srsran;

rlc_rx_um_entity::rlc_rx_um_entity(uint32_t                          du_index,
                                   du_ue_index_t                     ue_index,
                                   rb_id_t                           rb_id,
                                   const rlc_rx_um_config&           config,
                                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                                   timer_factory                     timers,
                                   task_executor&                    ue_executor,
                                   bool                              metrics_enable,
                                   rlc_pcap&                         pcap_) :
  rlc_rx_entity(du_index, ue_index, rb_id, upper_dn_, metrics_enable, pcap_),
  cfg(config),
  mod(cardinality(to_number(cfg.sn_field_length))),
  um_window_size(window_size(to_number(cfg.sn_field_length))),
  rx_window(create_rx_window(cfg.sn_field_length)),
  reassembly_timer(timers.create_timer()),
  pcap_context(ue_index, rb_id, config)
{
  metrics.metrics_set_mode(rlc_mode::um_bidir);

  // check reassembly_timer
  srsran_assert(reassembly_timer.is_valid(), "Cannot create RLC RX UM: reassembly_timer not configured.");

  // configure reassembly_timer
  if (cfg.t_reassembly > 0) {
    reassembly_timer.set(std::chrono::milliseconds(cfg.t_reassembly),
                         [this](timer_id_t tid) { on_expired_reassembly_timer(); });
  }
  logger.log_info("RLC UM configured. {}", cfg);
}

// TS 38.322 v16.2.0 Sec. 5.2.3.2.2
void rlc_rx_um_entity::handle_pdu(byte_buffer_slice buf)
{
  metrics.metrics_add_pdus(1, buf.length());

  pcap.push_pdu(pcap_context, buf);

  rlc_um_pdu_header header = {};
  if (not rlc_um_read_data_pdu_header(buf.view(), cfg.sn_field_length, &header)) {
    logger.log_warning(buf.begin(), buf.end(), "RX PDU with malformed header. pdu_len={}", buf.length());
    metrics.metrics_add_malformed_pdus(1);
    return;
  }
  logger.log_info(buf.begin(), buf.end(), "RX PDU. pdu_len={} {}", buf.length(), header);

  // length check: there must be at least one payload byte
  size_t header_len = rlc_um_nr_packed_length(header);
  if (buf.length() <= header_len) {
    logger.log_warning("Dropped malformed PDU without payload. pdu_len={} header_len={}", buf.length(), header_len);
    return;
  }

  // strip header, extract payload
  byte_buffer_slice payload = buf.make_slice(header_len, buf.length() - header_len);

  // check if PDU contains a SN
  if (header.si == rlc_si_field::full_sdu) {
    // deliver to upper layer
    logger.log_info("RX SDU. sdu_len={}", payload.length());
    metrics.metrics_add_sdus(1, payload.length());
    upper_dn.on_new_sdu(std::move(payload));
    // Nothing else to do here ...
    return;
  }
  if (sn_invalid_for_rx_buffer(header.sn)) {
    logger.log_info("Discarded PDU. sn={} payload_len={}", header.sn, payload.length());
    // Nothing else to do here ...
    return;
  }

  // We received an SDU segment
  handle_segment_data_sdu(header, std::move(payload));

  // Log state
  log_state(srslog::basic_levels::debug);

  // 5.2.2.2.3 Actions when an UMD PDU is placed in the reception buffer
  /*
   * - if all byte segments with SN = x are received:
   */
  if (rx_window->has_sn(header.sn) && (*rx_window)[header.sn].fully_received) {
    /*
     * - reassemble the RLC SDU from all byte segments with SN = x, remove RLC headers and deliver the reassembled
     *   RLC SDU to upper layer;
     */
    rlc_rx_um_sdu_info& rx_sdu = (*rx_window)[header.sn];
    logger.log_info("RX SDU. sn={} sdu_len={}", header.sn, rx_sdu.sdu.length());
    metrics.metrics_add_sdus(1, rx_sdu.sdu.length());
    upper_dn.on_new_sdu(std::move(rx_sdu.sdu));

    /*
     * - if x = RX_Next_Reassembly:
     */
    if (header.sn == st.rx_next_reassembly) {
      /*
       * - update RX_Next_Reassembly to the SN of the first SN > current RX_Next_Reassembly that has not been
       *   reassembled and delivered to upper layer.
       */
      uint32_t sn_upd = 0;
      // move rx_next_reassembly forward and remove all fully received SDUs from rx_window
      for (sn_upd = (st.rx_next_reassembly) % mod; rx_mod_base(sn_upd) < rx_mod_base(st.rx_next_highest);
           sn_upd = (sn_upd + 1) % mod) {
        if (rx_window->has_sn(sn_upd)) {
          if (not(*rx_window)[sn_upd].fully_received) {
            break; // first SDU not fully received
          }
          // rx_next_reassembly serves as the lower edge of the receiving window
          // As such, we remove any SDU from the window if we update this value
          rx_window->remove_sn(sn_upd);
        } else {
          break; // first SDU not fully received
        }
      }
      // Update to the SN of the first SDU with missing bytes.
      // If it not exists, update to the upper end of the rx_window.
      st.rx_next_reassembly = sn_upd;
      logger.log_debug("Updated rx_next_reassembly={}.", st.rx_next_reassembly);
    }
  }
  /*
   * - else if x falls outside of the reassembly window:
   */
  else if (not sn_in_reassembly_window(header.sn)) {
    /*
     * - update RX_Next_Highest to x + 1;
     * - discard any UMD PDUs with SN that falls outside of the reassembly window;
     */
    st.rx_next_highest = (header.sn + 1) % mod;
    logger.log_debug("Updated rx_next_highest={}.", st.rx_next_highest);

    // clean anything outside reassembly window
    uint32_t sn_upd = 0;
    for (sn_upd = (st.rx_next_reassembly) % mod; not sn_in_reassembly_window(sn_upd); sn_upd = (sn_upd + 1) % mod) {
      logger.log_info("Discarding SDU outside RX window [{}:{}]. sn={}",
                      (st.rx_next_highest - um_window_size) % mod,
                      st.rx_next_highest,
                      sn_upd);
      if (rx_window->has_sn(sn_upd)) {
        if (not(*rx_window)[sn_upd].fully_received) {
          // count incomplete SDUs as lost
          logger.log_info("Discarding incomplete SDU. sn={}", sn_upd);
          metrics.metrics_add_lost_pdus(1);
        }
        rx_window->remove_sn(sn_upd);
      } else {
        // count non-existing SDUs as lost
        logger.log_info("Discarding SDU. sn={}", sn_upd);
        metrics.metrics_add_lost_pdus(1);
      }
    }
    /*
     * - if RX_Next_Reassembly falls outside of the reassembly window:
     *   - set RX_Next_Reassembly to the SN of the first SN >= (RX_Next_Highest – UM_Window_Size) that has not been
     *     reassembled and delivered to upper layer.
     */
    // Since sn_upd just entered the reassembly window in the previous loop (and everything below was cleaned),
    // we continue (and clean) until we face any SN that is not fully received
    for (; rx_mod_base(sn_upd) < rx_mod_base(st.rx_next_highest); sn_upd = (sn_upd + 1) % mod) {
      if (rx_window->has_sn(sn_upd) && (*rx_window)[sn_upd].fully_received) {
        // rx_next_reassembly serves as the lower edge of the receiving window
        // As such, we remove any SDU from the window if we update this value
        rx_window->remove_sn(sn_upd);
      } else {
        break; // first SDU not fully received
      }
    }

    // Update to the SN of the first SDU with missing bytes.
    // If it not exists, update to the upper end of the rx_window.
    st.rx_next_reassembly = sn_upd;
    logger.log_debug("Updated rx_next_reassembly={}.", st.rx_next_reassembly);
  }

  /*
   * - if t-Reassembly is running:
   */
  if (reassembly_timer.is_running()) {
    /*
     * - if RX_Timer_Trigger <= RX_Next_Reassembly; or
     * - if RX_Timer_Trigger falls outside of the reassembly window and RX_Timer_Trigger is not equal to
     *   RX_Next_Highest; or
     * - if RX_Next_Highest = RX_Next_Reassembly + 1 and there is no missing byte segment of the RLC SDU associated with
     *   SN = RX_Next_Reassembly before the last byte of all received segments of this RLC SDU:
     *   - stop and reset t-Reassembly.
     */
    bool stop_reassembly_timer = false;
    if (st.rx_timer_trigger <= st.rx_next_reassembly) {
      stop_reassembly_timer = true;
    }
    if (not sn_in_reassembly_window(st.rx_timer_trigger) && st.rx_timer_trigger != st.rx_next_highest) {
      stop_reassembly_timer = true;
    }
    if (st.rx_next_highest == st.rx_next_reassembly + 1) {
      if (rx_window->has_sn(st.rx_next_highest) && not(*rx_window)[st.rx_next_highest].has_gap) {
        stop_reassembly_timer = true;
      }
    }
    if (stop_reassembly_timer) {
      reassembly_timer.stop();
      logger.log_debug("Stopped reassembly_timer at sn={}.", header.sn);
    }
  }

  /*
   * - if t-Reassembly is not running (includes the case when t-Reassembly is stopped due to actions above):
   */
  if (not reassembly_timer.is_running()) {
    /*
     * - if RX_Next_Highest > RX_Next_Reassembly + 1; or
     * - if RX_Next_Highest = RX_Next_Reassembly + 1 and there is at least one missing byte segment of the RLC SDU
     *   associated with SN = RX_Next_Reassembly before the last byte of all received segments of this RLC SDU:
     *   - start t-Reassembly;
     *   - set RX_Timer_Trigger to RX_Next_Highest.
     */
    bool restart_reassembly_timer = false;
    if (rx_mod_base(st.rx_next_highest) > rx_mod_base(st.rx_next_reassembly + 1)) {
      restart_reassembly_timer = true;
    }
    if (rx_mod_base(st.rx_next_highest) == rx_mod_base(st.rx_next_reassembly + 1)) {
      if (rx_window->has_sn(st.rx_next_highest) && (*rx_window)[st.rx_next_highest].has_gap) {
        restart_reassembly_timer = true;
      }
    }
    if (restart_reassembly_timer) {
      reassembly_timer.run();
      st.rx_timer_trigger = st.rx_next_highest;
      logger.log_debug(
          "Started reassembly_timer at sn={}, updated rx_timer_trigger={}.", header.sn, st.rx_timer_trigger);
    }
  }
}

bool rlc_rx_um_entity::handle_segment_data_sdu(const rlc_um_pdu_header& header, byte_buffer_slice payload)
{
  if (header.si == rlc_si_field::full_sdu) {
    logger.log_error("Called {} with full SDU. payload_len={} {}", __FUNCTION__, payload.length(), header);
    return false;
  }

  // Log SDU segment reception
  logger.log_debug("RX SDU segment. payload_len={} {}", payload.length(), header);

  // Add new SN to RX window if no segments have been received yet
  rlc_rx_um_sdu_info& rx_sdu = rx_window->has_sn(header.sn) ? (*rx_window)[header.sn] : rx_window->add_sn(header.sn);

  // Create SDU segment, to be stored later
  rlc_rx_um_sdu_segment segment = {};
  segment.si                    = header.si;
  segment.so                    = header.so;
  segment.payload               = std::move(payload);

  // Store SDU segment. Sort by SO and check for duplicate bytes.
  bool stored = store_segment(rx_sdu, std::move(segment));

  // Check whether all segments have been received
  update_segment_inventory(rx_sdu);
  logger.log_debug("Updated segment inventory. {}", rx_sdu);
  if (rx_sdu.fully_received) {
    // Assemble SDU from segments
    for (const rlc_rx_um_sdu_segment& segm : rx_sdu.segments) {
      logger.log_debug("Chaining segment. so={} len={}", segm.so, segm.payload.length());
      rx_sdu.sdu.append(segm.payload.copy());
    }
    rx_sdu.segments.clear();
    logger.log_debug("Assembled SDU from segments. sn={} sdu_len={}", header.sn, rx_sdu.sdu.length());
  }
  return stored;
}

bool rlc_rx_um_entity::store_segment(rlc_rx_um_sdu_info& sdu_info, rlc_rx_um_sdu_segment new_segment)
{
  // Section 5.2.2.2.2; Although not supposed to happen in UM, we check and discard segments with overlapping bytes
  // as described in Section 5.2.3.2.2 for AM.

  std::set<rlc_rx_um_sdu_segment, rlc_rx_um_sdu_segment_cmp>::iterator cur_segment = sdu_info.segments.begin();
  while (cur_segment != sdu_info.segments.end()) {
    uint32_t cur_last_byte = cur_segment->so + cur_segment->payload.length() - 1;
    uint32_t new_last_byte = new_segment.so + new_segment.payload.length() - 1;
    if (new_segment.so > cur_last_byte) {
      // new segment starts after the end of the current segment
      // cur: ...abcde
      // new:          ghij...
      // check next segment
      ++cur_segment;
      continue;
    }
    if (new_segment.so >= cur_segment->so) {
      // new segment starts within the current segment
      // cur: ...abcd...
      // new:     bcd...
      if (new_last_byte <= cur_last_byte) {
        // new segment ends before or at the end of the current segment
        // cur: ...abcdef
        // new:     bcde
        // discard new segment and return false
        return false;
      }
      // new segment ends after the end of the current segment
      // cur: ...abcdef
      // new:     bcdefghij...
      // trim new segment:
      // cur: ...abcdef
      // new':         ghij...
      new_segment.payload.advance(cur_last_byte + 1 - new_segment.so);
      new_segment.so = cur_last_byte + 1;
      // check next segment (it might overlap the new segment)
      // new':         ghij...
      // next:           ij...
      ++cur_segment;
      continue;
    }
    // new segment starts before current segment
    if (new_last_byte < cur_segment->so) {
      // new segment ends before the end of the current segment
      // cur:          ghij...
      // new: ...abcde
      // exit loop and insert new segment afterwards
      break;
    }
    // new segment ends within or after the current segment
    if (new_last_byte < cur_last_byte) {
      // new segment ends within current segment
      // cur:     bcdefghij...
      // new: ...abcdef
      // trim current segment
      // cur':         ghij...
      // new: ...abcdef
      rlc_rx_um_sdu_segment cut_segment{*cur_segment};
      cut_segment.payload.advance(new_last_byte + 1 - cur_segment->so);
      cut_segment.so = new_last_byte + 1;
      sdu_info.segments.erase(cur_segment++);
      // insert cut segment as close as possible before (next) current segment - this is faster than plain insert
      sdu_info.segments.insert(cur_segment, std::move(cut_segment));
      // exit loop and insert new segment afterwards
      break;
    }
    // new segment ends after the end of the current segment
    // cur:     bcde
    // new: ...abcdef...
    // remove current segment, check next segment
    sdu_info.segments.erase(cur_segment++);
  }
  // insert new segment as close as possible before current segment - this is faster than plain insert
  sdu_info.segments.insert(cur_segment, std::move(new_segment));
  return true;
}

void rlc_rx_um_entity::update_segment_inventory(rlc_rx_um_sdu_info& rx_sdu) const
{
  if (rx_sdu.segments.empty()) {
    rx_sdu.fully_received = false;
    rx_sdu.has_gap        = false;
    return;
  }

  // Check for gaps and if all segments have been received
  uint32_t next_byte = 0;
  for (const rlc_rx_um_sdu_segment& segm : rx_sdu.segments) {
    if (segm.so != next_byte) {
      // Found gap: set flags and return
      rx_sdu.has_gap        = true;
      rx_sdu.fully_received = false;
      return;
    }
    if (segm.si == rlc_si_field::last_segment) {
      // Reached last segment without any gaps: set flags and return
      rx_sdu.has_gap        = false;
      rx_sdu.fully_received = true;
      return;
    }
    next_byte += segm.payload.length();
  }
  // No gaps, but last segment not yet received
  rx_sdu.has_gap        = false;
  rx_sdu.fully_received = false;
}

// TS 38.322 v16.2.0 Sec. 5.2.2.2.4
void rlc_rx_um_entity::on_expired_reassembly_timer()
{
  logger.log_debug("Reassembly timer expired for sn={}.", st.rx_next_reassembly);

  /*
   * - update RX_Next_Reassembly to the SN of the first SN >= RX_Timer_Trigger that has not been reassembled;
   * - discard all segments with SN < updated RX_Next_Reassembly;
   */
  uint32_t sn_upd = st.rx_next_reassembly; // lowest element from which we have to clean the rx_window
  for (; rx_mod_base(sn_upd) < rx_mod_base(st.rx_next_highest); sn_upd = (sn_upd + 1) % mod) {
    if (rx_mod_base(sn_upd) < rx_mod_base(st.rx_timer_trigger)) {
      // remove anything below rx_timer_trigger
      if (rx_window->has_sn(sn_upd)) {
        if (not(*rx_window)[sn_upd].fully_received) {
          // count incomplete SDUs as lost
          logger.log_info("Discarding incomplete SDU. sn={}", sn_upd);
          metrics.metrics_add_lost_pdus(1);
        }
        rx_window->remove_sn(sn_upd);
      } else {
        // count non-existing SDUs as lost
        logger.log_info("Discarding SDU. sn={}", sn_upd);
        metrics.metrics_add_lost_pdus(1);
      }
    } else {
      // continue removing fully received SDUs starting from rx_timer_trigger; stop at first incomplete or unseen SDU.
      if (rx_window->has_sn(sn_upd) && (*rx_window)[sn_upd].fully_received) {
        rx_window->remove_sn(sn_upd);
      } else {
        break; // first SDU not fully received
      }
    }
  }
  st.rx_next_reassembly = sn_upd;
  logger.log_debug("Updated rx_next_reassembly={}.", st.rx_next_reassembly);

  /*
   * - if RX_Next_Highest > RX_Next_Reassembly + 1; or
   * - if RX_Next_Highest = RX_Next_Reassembly + 1 and there is at least one missing byte segment of the RLC SDU
   *   associated with SN = RX_Next_Reassembly before the last byte of all received segments of this RLC SDU:
   *   - start t-Reassembly;
   *   - set RX_Timer_Trigger to RX_Next_Highest.
   */
  bool restart_reassembly_timer = false;
  if (rx_mod_base(st.rx_next_highest) > rx_mod_base(st.rx_next_reassembly + 1)) {
    restart_reassembly_timer = true;
  }
  if (rx_mod_base(st.rx_next_highest) == rx_mod_base(st.rx_next_reassembly + 1)) {
    if (rx_window->has_sn(st.rx_next_highest) && (*rx_window)[st.rx_next_highest].has_gap) {
      restart_reassembly_timer = true;
    }
  }
  if (restart_reassembly_timer) {
    reassembly_timer.run();
    st.rx_timer_trigger = st.rx_next_highest;
    logger.log_debug(
        "Started reassembly_timer for sn={}, updated rx_timer_trigger={}.", st.rx_next_reassembly, st.rx_timer_trigger);
  }

  log_state(srslog::basic_levels::debug);
}

// TS 38.322 v16.2.0 Sec 5.2.2.2.1
bool rlc_rx_um_entity::sn_in_reassembly_window(const uint32_t sn)
{
  return (rx_mod_base(st.rx_next_highest - um_window_size) <= rx_mod_base(sn) &&
          rx_mod_base(sn) < rx_mod_base(st.rx_next_highest));
}

// TS 38.322 v16.2.0 Sec 5.2.2.2.2
bool rlc_rx_um_entity::sn_invalid_for_rx_buffer(const uint32_t sn)
{
  return (rx_mod_base(st.rx_next_highest - um_window_size) <= rx_mod_base(sn) &&
          rx_mod_base(sn) < rx_mod_base(st.rx_next_reassembly));
}

std::unique_ptr<sdu_window<rlc_rx_um_sdu_info>> rlc_rx_um_entity::create_rx_window(rlc_um_sn_size sn_size)
{
  std::unique_ptr<sdu_window<rlc_rx_um_sdu_info>> rx_window_;
  switch (sn_size) {
    case rlc_um_sn_size::size6bits:
      rx_window_ = std::make_unique<
          sdu_window_impl<rlc_rx_um_sdu_info, window_size(to_number(rlc_um_sn_size::size6bits)), rlc_bearer_logger>>(
          logger);
      break;
    case rlc_um_sn_size::size12bits:
      rx_window_ = std::make_unique<
          sdu_window_impl<rlc_rx_um_sdu_info, window_size(to_number(rlc_um_sn_size::size12bits)), rlc_bearer_logger>>(
          logger);
      break;
    default:
      srsran_assertion_failure("Cannot create rx_window for unsupported sn_size={}.", to_number(sn_size));
  }
  return rx_window_;
}
