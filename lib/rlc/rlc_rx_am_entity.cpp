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

#include "rlc_rx_am_entity.h"
#include "srsran/adt/scope_exit.h"

using namespace srsran;

rlc_rx_am_entity::rlc_rx_am_entity(du_ue_index_t                     du_index,
                                   rb_id_t                           rb_id,
                                   const rlc_rx_am_config&           config,
                                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                                   timer_factory                     timers,
                                   task_executor&                    ue_executor_) :
  rlc_rx_entity(du_index, rb_id, upper_dn_),
  cfg(config),
  mod(cardinality(to_number(cfg.sn_field_length))),
  am_window_size(window_size(to_number(cfg.sn_field_length))),
  rx_window(create_rx_window(cfg.sn_field_length)),
  status_report(cfg.sn_field_length),
  status_prohibit_timer(timers.create_timer()),
  reassembly_timer(timers.create_timer()),
  ue_executor(ue_executor_)
{
  metrics.metrics_set_mode(rlc_mode::am);

  // check status_prohibit_timer
  srsran_assert(status_prohibit_timer.is_valid(), "Cannot create RLC RX AM, status_prohibit_timer not configured.");
  // check reassembly_timer
  srsran_assert(reassembly_timer.is_valid(), "Cannot create RLC RX AM, reassembly_timer not configured.");

  // configure status_prohibit_timer
  if (cfg.t_status_prohibit > 0) {
    status_prohibit_timer.set(std::chrono::milliseconds{cfg.t_status_prohibit},
                              [this](timer_id_t tid) { on_expired_status_prohibit_timer(); });
  }

  // configure reassembly_timer
  if (cfg.t_reassembly > 0) {
    reassembly_timer.set(std::chrono::milliseconds{cfg.t_reassembly},
                         [this](timer_id_t tid) { on_expired_reassembly_timer(); });
  }

  // initialize status report
  status_report.ack_sn = st.rx_next_highest;
  status_report_size.store(status_report.get_packed_size(), std::memory_order_relaxed);

  logger.log_info("RLC AM configured. {}", cfg);
}

// Interfaces for lower layers
void rlc_rx_am_entity::handle_pdu(byte_buffer_slice buf)
{
  metrics.metrics_add_pdus(1, buf.length());
  if (buf.empty()) {
    logger.log_warning("Dropped empty PDU.");
    return;
  }
  if (rlc_am_status_pdu::is_control_pdu(buf.view())) {
    metrics.metrics_add_ctrl_pdus(1, buf.length());
    handle_control_pdu(std::move(buf));
  } else {
    handle_data_pdu(std::move(buf));
  }
}

void rlc_rx_am_entity::handle_control_pdu(byte_buffer_slice buf)
{
  logger.log_debug(buf.begin(), buf.end(), "RX control PDU. pdu_len={}", buf.length());
  rlc_am_status_pdu status_pdu(cfg.sn_field_length);
  if (status_pdu.unpack(buf.view())) {
    logger.log_info("RX status PDU. {}", status_pdu);
    status_handler->on_status_pdu(std::move(status_pdu));
  } else {
    logger.log_warning(buf.begin(), buf.end(), "Failed to unpack control PDU. pdu_len={}", buf.length());
  }
}

void rlc_rx_am_entity::handle_data_pdu(byte_buffer_slice buf)
{
  bool status_changed   = false;
  bool status_requested = false;
  auto on_function_exit = make_scope_exit([&]() {
    logger.log_debug(
        "Post-processing for AMD PDU: status_changed={} status_requested={}", status_changed, status_requested);
    if (status_changed) {
      refresh_status_report();
    }
    if (status_requested) {
      do_status.store(true, std::memory_order_relaxed);
    }
    if (status_changed || status_requested) {
      notify_status_report_changed();
    }
  });

  rlc_am_pdu_header header = {};
  if (not rlc_am_read_data_pdu_header(buf.view(), cfg.sn_field_length, &header)) {
    logger.log_warning(buf.begin(), buf.end(), "RX PDU with malformed header. pdu_len={}", buf.length());
    metrics.metrics_add_malformed_pdus(1);
    return;
  }
  logger.log_info(buf.begin(), buf.end(), "RX PDU. pdu_len={} {}", buf.length(), header);

  // length check: there must be at least one payload byte
  size_t header_len = header.get_packed_size();
  if (buf.length() <= header_len) {
    logger.log_warning("Dropped malformed PDU without payload. pdu_len={} header_len={}", buf.length(), header_len);
    return;
  }
  // strip header, extract payload
  byte_buffer_slice payload = buf.make_slice(header_len, buf.length() - header_len);

  // Store the poll bit for later evaluation on_function_exit.
  // We do this before checking if the PDU is inside the RX window,
  // as the RX window may have advanced without the TX having received the ACKs.
  // This can cause a data stall, whereby the TX keeps retransmiting
  // a PDU outside of the RX window.
  // Also, we do this before discarding duplicate SDUs/SDU segments
  // because t-PollRetransmit may transmit a PDU that was already
  // received.
  if (header.p != 0U) {
    logger.log_debug("Status report requested via polling bit.");
    status_requested = true;
  }

  // Check whether PDU SN is within RX Window
  if (!inside_rx_window(header.sn)) {
    logger.log_debug("Discarded PDU outside the RX window [{}:{}]. sn={}",
                     st.rx_next,
                     (st.rx_next + am_window_size) % mod,
                     header.sn);
    return;
  }

  // Section 5.2.3.2.2, discard duplicate PDUs
  if (rx_window->has_sn(header.sn) && (*rx_window)[header.sn].fully_received) {
    logger.log_debug("Discarded PDU duplicate. sn={}", header.sn);
    return;
  }

  // Write to rx window either full SDU or SDU segment
  if (header.si == rlc_si_field::full_sdu) {
    status_changed = handle_full_data_sdu(header, std::move(payload));
  } else {
    status_changed = handle_segment_data_sdu(header, std::move(payload));
  }

  // Log state
  log_state(srslog::basic_levels::debug);

  // 5.2.3.2.3 Actions when an AMD PDU is placed in the reception buffer
  /*
   * - if x >= RX_Next_Highest
   *   - update RX_Next_Highest to x+ 1.
   */
  if (rx_mod_base(header.sn) >= rx_mod_base(st.rx_next_highest)) {
    st.rx_next_highest = (header.sn + 1) % mod;
    logger.log_debug("Updated rx_next_highest={}.", st.rx_next_highest);
  }

  /*
   * - if all bytes of the RLC SDU with SN = x are received:
   */
  if (rx_window->has_sn(header.sn) && (*rx_window)[header.sn].fully_received) {
    /*
     * - reassemble the RLC SDU from AMD PDU(s) with SN = x, remove RLC headers when doing so and deliver
     *   the reassembled RLC SDU to upper layer;
     */
    rlc_rx_am_sdu_info& rx_sdu = (*rx_window)[header.sn];
    logger.log_info("RX SDU. sn={} sdu_len={}", header.sn, rx_sdu.sdu.length());
    metrics.metrics_add_sdus(1, rx_sdu.sdu.length());
    upper_dn.on_new_sdu(std::move(rx_sdu.sdu));

    /*
     * - if x = RX_Highest_Status,
     *   - update RX_Highest_Status to the SN of the first RLC SDU with SN > current RX_Highest_Status for which not
     * all bytes have been received.
     */
    if (rx_mod_base(header.sn) == rx_mod_base(st.rx_highest_status)) {
      uint32_t sn_upd = 0;
      for (sn_upd = (st.rx_highest_status + 1) % mod; rx_mod_base(sn_upd) < rx_mod_base(st.rx_next_highest);
           sn_upd = (sn_upd + 1) % mod) {
        if (rx_window->has_sn(sn_upd)) {
          if (not(*rx_window)[sn_upd].fully_received) {
            break; // first SDU not fully received
          }
        } else {
          break; // first SDU not fully received
        }
      }
      // Update to the SN of the first SDU with missing bytes.
      // If it not exists, update to the end of the rx_window.
      st.rx_highest_status = sn_upd;
      logger.log_debug("Updated rx_highest_status={}.", st.rx_highest_status);
    }
    /*
     * - if x = RX_Next:
     *   - update RX_Next to the SN of the first RLC SDU with SN > current RX_Next for which not all bytes
     *     have been received.
     */
    if (rx_mod_base(header.sn) == rx_mod_base(st.rx_next)) {
      uint32_t sn_upd = 0;
      // move rx_next forward and remove all fully received SDUs from rx_window
      for (sn_upd = (st.rx_next) % mod; rx_mod_base(sn_upd) < rx_mod_base(st.rx_next_highest);
           sn_upd = (sn_upd + 1) % mod) {
        if (rx_window->has_sn(sn_upd)) {
          if (not(*rx_window)[sn_upd].fully_received) {
            break; // first SDU not fully received
          }
          // RX_Next serves as the lower edge of the receiving window
          // As such, we remove any SDU from the window if we update this value
          rx_window->remove_sn(sn_upd);
        } else {
          break; // first SDU not fully received
        }
      }
      // Update to the SN of the first SDU with missing bytes.
      // If it not exists, update to the end of the rx_window.
      st.rx_next = sn_upd;
      logger.log_debug("Updated rx_next={}.", st.rx_next);
    }
  }

  if (reassembly_timer.is_running()) {
    // if t-Reassembly is running:
    /*
     * - if RX_Next_Status_Trigger = RX_Next; or
     * - if RX_Next_Status_Trigger = RX_Next + 1 and there is no missing byte segment of the SDU associated with
     *   SN = RX_Next before the last byte of all received segments of this SDU; or
     * - if RX_Next_Status_Trigger falls outside of the receiving window and RX_Next_Status_Trigger is not equal
     *   to RX_Next + AM_Window_Size:
     * - stop and reset t-Reassembly.
     */
    bool stop_reassembly_timer = false;
    if (st.rx_next_status_trigger == st.rx_next) {
      stop_reassembly_timer = true;
    }
    if (rx_mod_base(st.rx_next_status_trigger) == rx_mod_base(st.rx_next + 1)) {
      if (not(*rx_window)[st.rx_next].has_gap) {
        stop_reassembly_timer = true;
      }
    }
    if (not inside_rx_window(st.rx_next_status_trigger)) {
      stop_reassembly_timer = true;
    }
    if (stop_reassembly_timer) {
      reassembly_timer.stop();
      logger.log_debug("Stopped reassembly timer.");
    }
  }

  if (not reassembly_timer.is_running()) {
    // if t-Reassembly is not running (includes the case t-Reassembly is stopped due to actions above):
    /*
     * - if RX_Next_Highest> RX_Next +1; or
     * - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
     *   with SN = RX_Next before the last byte of all received segments of this SDU:
     *   - start t-Reassembly;
     *   - set RX_Next_Status_Trigger to RX_Next_Highest.
     */
    bool restart_reassembly_timer = false;
    if (rx_mod_base(st.rx_next_highest) > rx_mod_base(st.rx_next + 1)) {
      restart_reassembly_timer = true;
    }
    if (rx_mod_base(st.rx_next_highest) == rx_mod_base(st.rx_next + 1)) {
      if (rx_window->has_sn(st.rx_next) && (*rx_window)[st.rx_next].has_gap) {
        restart_reassembly_timer = true;
      }
    }
    if (restart_reassembly_timer) {
      reassembly_timer.run();
      st.rx_next_status_trigger = st.rx_next_highest;
      logger.log_debug("Started reassembly timer, updated rx_next_status_trigger={}.", st.rx_next_status_trigger);
    }
  }
}

bool rlc_rx_am_entity::handle_full_data_sdu(const rlc_am_pdu_header& header, byte_buffer_slice payload)
{
  if (header.si != rlc_si_field::full_sdu) {
    logger.log_error("Called {} with SDU segment. payload_len={} {}", __FUNCTION__, payload.length(), header);
    return false;
  }

  // Log full SDU reception
  logger.log_debug("RX SDU. payload_len={} {}", payload.length(), header);

  // Add new SN to RX window if no segments have been received yet
  rlc_rx_am_sdu_info& rx_sdu = rx_window->has_sn(header.sn) ? (*rx_window)[header.sn] : rx_window->add_sn(header.sn);

  // Full SDU received. Clear segments and use full payload as SDU.
  rx_sdu.segments.clear();
  rx_sdu.sdu            = std::move(payload);
  rx_sdu.fully_received = true;
  rx_sdu.has_gap        = false;
  return true;
}

bool rlc_rx_am_entity::handle_segment_data_sdu(const rlc_am_pdu_header& header, byte_buffer_slice payload)
{
  if (header.si == rlc_si_field::full_sdu) {
    logger.log_error("Called {} with full SDU. payload_len={} {}", __FUNCTION__, payload.length(), header);
    return false;
  }

  // Log SDU segment reception
  logger.log_debug("RX SDU segment. payload_len={} {}", payload.length(), header);

  // Add new SN to RX window if no segments have been received yet
  rlc_rx_am_sdu_info& rx_sdu = rx_window->has_sn(header.sn) ? (*rx_window)[header.sn] : rx_window->add_sn(header.sn);

  // Create SDU segment, to be stored later
  rlc_rx_am_sdu_segment segment = {};
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
    for (const rlc_rx_am_sdu_segment& segm : rx_sdu.segments) {
      logger.log_debug("Chaining segment. so={} len={}", segm.so, segm.payload.length());
      rx_sdu.sdu.append(segm.payload.copy());
    }
    rx_sdu.segments.clear();
    logger.log_debug("Assembled SDU from segments. sn={} sdu_len={}", header.sn, rx_sdu.sdu.length());
  }
  return stored;
}

bool rlc_rx_am_entity::store_segment(rlc_rx_am_sdu_info& sdu_info, rlc_rx_am_sdu_segment new_segment)
{
  // Section 5.2.3.2.2, discard segments with overlapping bytes

  std::set<rlc_rx_am_sdu_segment, rlc_rx_am_sdu_segment_cmp>::iterator cur_segment = sdu_info.segments.begin();
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
      rlc_rx_am_sdu_segment cut_segment{*cur_segment};
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

void rlc_rx_am_entity::update_segment_inventory(rlc_rx_am_sdu_info& rx_sdu) const
{
  if (rx_sdu.segments.empty()) {
    rx_sdu.fully_received = false;
    rx_sdu.has_gap        = false;
    return;
  }

  // Check for gaps and if all segments have been received
  uint32_t next_byte = 0;
  for (const rlc_rx_am_sdu_segment& segm : rx_sdu.segments) {
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

void rlc_rx_am_entity::refresh_status_report()
{
  rlc_am_status_pdu tmp_status_report = {cfg.sn_field_length};
  /*
   * - for the RLC SDUs with SN such that RX_Next <= SN < RX_Highest_Status that has not been completely
   *   received yet, in increasing SN order of RLC SDUs and increasing byte segment order within RLC SDUs,
   *   starting with SN = RX_Next up to the point where the resulting STATUS PDU still fits to the total size of RLC
   *   PDU(s) indicated by lower layer:
   */
  logger.log_debug("Generating status PDU. rx_next={} rx_highest_status={}", st.rx_next, st.rx_highest_status);
  for (uint32_t i = st.rx_next; rx_mod_base(i) < rx_mod_base(st.rx_highest_status); i = (i + 1) % mod) {
    if ((rx_window->has_sn(i) && (*rx_window)[i].fully_received)) {
      logger.log_debug("SDU complete. sn={}", i);
    } else {
      if (not rx_window->has_sn(i)) {
        // No segment received, NACK the whole SDU
        rlc_am_status_nack nack;
        nack.nack_sn = i;
        nack.has_so  = false;
        logger.log_debug("Adding nack={}.", nack);
        tmp_status_report.push_nack(nack);
      } else if (not(*rx_window)[i].fully_received) {
        // Some segments were received, but not all.
        // NACK non consecutive missing bytes
        uint32_t last_so           = 0;
        bool     have_last_segment = false;
        for (auto segm = (*rx_window)[i].segments.begin(); segm != (*rx_window)[i].segments.end(); segm++) {
          if (segm->so != last_so) {
            // Some bytes were not received
            rlc_am_status_nack nack;
            nack.nack_sn  = i;
            nack.has_so   = true;
            nack.so_start = last_so;
            nack.so_end   = segm->so - 1; // set to last missing byte
            logger.log_debug("Adding nack={}.", nack);
            tmp_status_report.push_nack(nack);

            // Sanity check
            if (nack.so_start > nack.so_end) {
              // Print segment list
              for (auto segm_it = (*rx_window)[i].segments.begin(); segm_it != (*rx_window)[i].segments.end();
                   segm_it++) {
                logger.log_error("Segment: so={} len={}", segm_it->so, segm_it->payload.length());
              }
              logger.log_error("Invalid segment offsets in nack={} for segment so={}.", nack, segm->so);
              srsran_assert(nack.so_start <= nack.so_end,
                            "Invalid segment offsets in nack={} for segment so={}.",
                            nack,
                            segm->so);
            }
          }
          if (segm->si == rlc_si_field::last_segment) {
            have_last_segment = true;
          }
          last_so = segm->so + segm->payload.length();
        } // Segment loop

        // Check for last segment
        if (not have_last_segment) {
          rlc_am_status_nack nack;
          nack.nack_sn  = i;
          nack.has_so   = true;
          nack.so_start = last_so;
          nack.so_end   = rlc_am_status_nack::so_end_of_sdu;
          logger.log_debug("Adding nack={}.", nack);
          tmp_status_report.push_nack(nack);
          // Sanity check
          srsran_assert(nack.so_start <= nack.so_end, "Invalid segment offsets in nack={}.", nack);
        }
      }
    }
  } // NACK loop

  /*
   * - set the ACK_SN to the SN of the next not received RLC SDU which is not
   * indicated as missing in the resulting STATUS PDU.
   */
  tmp_status_report.ack_sn = st.rx_highest_status;

  store_status_report(std::move(tmp_status_report));

  logger.log_debug("Refreshed status_report. {}", status_report);
}

void rlc_rx_am_entity::store_status_report(rlc_am_status_pdu&& status)
{
  std::unique_lock<std::mutex> lock(status_report_mutex);
  status_report = std::move(status);
  status_report_size.store(status_report.get_packed_size(), std::memory_order_relaxed);
}

rlc_am_status_pdu rlc_rx_am_entity::get_status_pdu()
{
  do_status.store(false, std::memory_order_relaxed);
  if (status_prohibit_timer.is_valid() && cfg.t_status_prohibit > 0) {
    if (not ue_executor.defer([&]() { status_prohibit_timer.run(); })) {
      logger.log_error("Unable to start prohibit timer");
    }
    status_prohibit_timer_is_running.store(true, std::memory_order_relaxed);
  }
  std::unique_lock<std::mutex> lock(status_report_mutex);
  return status_report;
}

uint32_t rlc_rx_am_entity::get_status_pdu_length()
{
  return status_report_size.load(std::memory_order_relaxed);
}

bool rlc_rx_am_entity::status_report_required()
{
  return do_status.load(std::memory_order_relaxed) &&
         not status_prohibit_timer_is_running.load(std::memory_order_relaxed);
}

void rlc_rx_am_entity::notify_status_report_changed()
{
  if (status_report_required()) {
    logger.log_debug("Notifying TX that status report has changed.");
    status_notifier->on_status_report_changed();
  }
}

std::unique_ptr<rlc_sdu_window_base<rlc_rx_am_sdu_info>> rlc_rx_am_entity::create_rx_window(rlc_am_sn_size sn_size)
{
  std::unique_ptr<rlc_sdu_window_base<rlc_rx_am_sdu_info>> rx_window_;
  switch (sn_size) {
    case rlc_am_sn_size::size12bits:
      rx_window_ =
          std::make_unique<rlc_sdu_window<rlc_rx_am_sdu_info, window_size(to_number(rlc_am_sn_size::size12bits))>>(
              logger);
      break;
    case rlc_am_sn_size::size18bits:
      rx_window_ =
          std::make_unique<rlc_sdu_window<rlc_rx_am_sdu_info, window_size(to_number(rlc_am_sn_size::size18bits))>>(
              logger);
      break;
    default:
      srsran_assertion_failure("Cannot create rx_window for unsupported sn_size={}.", to_number(sn_size));
  }
  return rx_window_;
}

/*
 * Timer handling functions
 */

void rlc_rx_am_entity::on_expired_status_prohibit_timer()
{
  logger.log_debug("Status prohibit timer expired after {}ms.", status_prohibit_timer.duration().count());

  status_prohibit_timer_is_running.store(false, std::memory_order_relaxed);
  notify_status_report_changed();
}

void rlc_rx_am_entity::on_expired_reassembly_timer()
{
  // Reassembly
  logger.log_debug("Reassembly timer expired after {}ms.", reassembly_timer.duration().count());
  // Check if timer has been restarted by the PDU handling routine between expiration and execution of this handler.
  if (reassembly_timer.is_running()) {
    logger.log_info("Reassembly timer has been already restarted. Skipping outdated event. {}", st);
    return;
  }
  if (not valid_ack_sn(st.rx_next_status_trigger)) {
    logger.log_info("rx_next_status_trigger is outside RX window. Skipping outdated event. {}", st);
    return;
  }
  /*
   * 5.2.3.2.4 Actions when t-Reassembly expires:
   * - update RX_Highest_Status to the SN of the first RLC SDU with SN >= RX_Next_Status_Trigger for which not
   *   all bytes have been received;
   * - if RX_Next_Highest> RX_Highest_Status +1: or
   * - if RX_Next_Highest = RX_Highest_Status + 1 and there is at least one missing byte segment of the SDU
   *   associated with SN = RX_Highest_Status before the last byte of all received segments of this SDU:
   *   - start t-Reassembly;
   *   - set RX_Next_Status_Trigger to RX_Next_Highest.
   */
  uint32_t sn_upd = st.rx_next_status_trigger;
  for (; rx_mod_base(sn_upd) < rx_mod_base(st.rx_next_highest); sn_upd = (sn_upd + 1) % mod) {
    if (not rx_window->has_sn(sn_upd) || (rx_window->has_sn(sn_upd) && not(*rx_window)[sn_upd].fully_received)) {
      break;
    }
  }
  if (not valid_ack_sn(sn_upd)) {
    logger.log_error("Invalid rx_highest_status={} outside RX window. {}", sn_upd, st);
  }
  srsran_assert(valid_ack_sn(sn_upd), "Error: rx_highest_status={} outside RX window. {}", sn_upd, st);
  st.rx_highest_status = sn_upd;

  bool restart_reassembly_timer = false;
  if (rx_mod_base(st.rx_next_highest) > rx_mod_base(st.rx_highest_status + 1)) {
    restart_reassembly_timer = true;
  }
  if (rx_mod_base(st.rx_next_highest) == rx_mod_base(st.rx_highest_status + 1)) {
    if (rx_window->has_sn(st.rx_highest_status) && (*rx_window)[st.rx_highest_status].has_gap) {
      restart_reassembly_timer = true;
    }
  }
  if (restart_reassembly_timer) {
    reassembly_timer.run();
    st.rx_next_status_trigger = st.rx_next_highest;
    logger.log_debug("Restarted t-Reassmebly. {}", st);
  }

  /* 5.3.4 Status reporting:
   * - The receiving side of an AM RLC entity shall trigger a STATUS report when t-Reassembly expires.
   *   NOTE 2: The expiry of t-Reassembly triggers both RX_Highest_Status to be updated and a STATUS report to be
   *   triggered, but the STATUS report shall be triggered after RX_Highest_Status is updated.
   */
  refresh_status_report();
  do_status.store(true, std::memory_order_relaxed);
  notify_status_report_changed();

  log_state(srslog::basic_levels::debug);
  logger.log_debug("RX window state: nof_sdus={}", rx_window->size());
  return;
}
