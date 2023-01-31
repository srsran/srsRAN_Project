/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_rx_am_entity.h"
#include "srsgnb/adt/scope_exit.h"

using namespace srsgnb;

rlc_rx_am_entity::rlc_rx_am_entity(du_ue_index_t                     du_index,
                                   rb_id_t                           rb_id,
                                   const rlc_rx_am_config&           config,
                                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                                   timer_manager&                    timers,
                                   task_executor&                    ue_executor) :
  rlc_rx_entity(du_index, rb_id, upper_dn_),
  cfg(config),
  mod(cardinality(to_number(cfg.sn_field_length))),
  am_window_size(window_size(to_number(cfg.sn_field_length))),
  rx_window(create_rx_window(cfg.sn_field_length)),
  status_report(cfg.sn_field_length),
  status_prohibit_timer(timers.create_unique_timer()),
  reassembly_timer(timers.create_unique_timer())
{
  metrics.metrics_set_mode(rlc_mode::am);

  // check status_prohibit_timer
  srsgnb_assert(status_prohibit_timer.is_valid(), "Cannot create RLC RX AM: status_prohibit_timer not configured");
  // check reassembly_timer
  srsgnb_assert(reassembly_timer.is_valid(), "Cannot create RLC RX AM: reassembly_timer not configured");

  // configure status_prohibit_timer
  if (cfg.t_status_prohibit > 0) {
    status_prohibit_timer.set(static_cast<uint32_t>(cfg.t_status_prohibit),
                              [this](uint32_t tid) { on_expired_status_prohibit_timer(tid); });
  }

  // configure reassembly_timer
  if (cfg.t_reassembly > 0) {
    reassembly_timer.set(static_cast<uint32_t>(cfg.t_reassembly), [this, &ue_executor](uint32_t tid) {
      ue_executor.execute([this, tid]() { on_expired_reassembly_timer(tid); });
    });
  }

  // initialize status report
  status_report.ack_sn = st.rx_next_highest;
}

// Interfaces for lower layers
void rlc_rx_am_entity::handle_pdu(byte_buffer_slice buf)
{
  metrics.metrics_add_pdus(1, buf.length());
  metrics.metrics_add_sdus(1, buf.length());
  if (rlc_am_status_pdu::is_control_pdu(buf.view())) {
    logger.log_info(buf.begin(), buf.end(), "RX control PDU: pdu_len={}", buf.length());
    handle_control_pdu(std::move(buf));
  } else {
    logger.log_info(buf.begin(), buf.end(), "RX AMD PDU: pdu_len={}", buf.length());
    handle_data_pdu(std::move(buf));
  }
}

void rlc_rx_am_entity::handle_control_pdu(byte_buffer_slice buf)
{
  rlc_am_status_pdu status_pdu(cfg.sn_field_length);
  if (status_pdu.unpack(buf.view())) {
    logger.log_debug(buf.begin(), buf.end(), "Unpacked status PDU. pdu_len={}", buf.length());
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
        "Post-processing for AMD PDU: status_changed={}, status_requested={}", status_changed, status_requested);
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

  logger.log_debug(buf.begin(), buf.end(), "RX AMD PDU: pdu_len={}", buf.length());
  rlc_am_pdu_header header = {};
  if (not rlc_am_read_data_pdu_header(buf.view(), cfg.sn_field_length, &header)) {
    logger.log_warning(buf.begin(), buf.end(), "Failed to unpack AMD PDU header. pdu_len={}");
    metrics.metrics_add_malformed_pdus(1);
    return;
  }
  logger.log_debug("AMD PDU header: {}", header);

  // strip header, extract payload
  size_t            header_len = header.get_packed_size();
  byte_buffer_slice payload    = buf.make_slice(header_len, buf.length() - header_len);

  // Store the poll bit for later evaluation on_function_exit.
  // We do this before checking if the PDU is inside the RX window,
  // as the RX window may have advanced without the TX having received the ACKs.
  // This can cause a data stall, whereby the TX keeps retransmiting
  // a PDU outside of the RX window.
  // Also, we do this before discarding duplicate SDUs/SDU segments
  // because t-PollRetransmit may transmit a PDU that was already
  // received.
  if (header.p != 0U) {
    logger.log_info("Status report requested via polling bit");
    status_requested = true;
  }

  // Check whether PDU SN is within RX Window
  if (!inside_rx_window(header.sn)) {
    logger.log_info("Discarded PDU: SN={} is outside the RX window [{}:{}]",
                    header.sn,
                    st.rx_next,
                    (st.rx_next + am_window_size) % mod);
    return;
  }

  // Section 5.2.3.2.2, discard duplicate PDUs
  if (rx_window->has_sn(header.sn) && (*rx_window)[header.sn].fully_received) {
    logger.log_info("Discarded PDU: SN={} is fully received", header.sn);
    return;
  }

  // Section 5.2.3.2.2, discard segments with overlapping bytes
  if (rx_window->has_sn(header.sn) && header.si != rlc_si_field::full_sdu) {
    for (const rlc_rx_am_sdu_segment& segm : (*rx_window)[header.sn].segments) {
      uint32_t segm_last_byte = segm.header.so + segm.payload.length() - 1;
      uint32_t pdu_last_byte  = header.so + payload.length() - 1;
      if ((header.so >= segm.header.so && header.so <= segm_last_byte) ||
          (pdu_last_byte >= segm.header.so && pdu_last_byte <= segm_last_byte)) {
        logger.log_info("Discarded segment with overlapping bytes: SN={}, SO={}, last_byte={}, payload_len={}",
                        header.sn,
                        header.so,
                        pdu_last_byte,
                        payload.length());
        logger.log_info("Overlapped with SDU segment: SN={}, SO={}, last_byte={}, payload_len={}",
                        header.sn,
                        segm.header.so,
                        segm_last_byte,
                        segm.payload.length());
        return;
      }
    }
  }

  // Write to rx window either full SDU or SDU segment
  if (header.si == rlc_si_field::full_sdu) {
    handle_full_data_sdu(header, std::move(payload));
  } else {
    handle_segment_data_sdu(header, std::move(payload));
  }
  status_changed = true;

  logger.log_debug("State: {}", st);

  // 5.2.3.2.3 Actions when an AMD PDU is placed in the reception buffer
  /*
   * - if x >= RX_Next_Highest
   *   - update RX_Next_Highest to x+ 1.
   */
  if (rx_mod_base(header.sn) >= rx_mod_base(st.rx_next_highest)) {
    st.rx_next_highest = (header.sn + 1) % mod;
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
    logger.log_info("RX SDU ({} B)", rx_sdu.sdu.length());
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
    }
  }
}

void rlc_rx_am_entity::handle_full_data_sdu(const rlc_am_pdu_header& header, byte_buffer_slice payload)
{
  if (header.si != rlc_si_field::full_sdu) {
    logger.log_error("called {} with SDU segment. Header: {}", __FUNCTION__, header);
    return;
  }

  // Add new SN to RX window if no segments have been received yet
  rlc_rx_am_sdu_info& rx_sdu = rx_window->has_sn(header.sn) ? (*rx_window)[header.sn] : rx_window->add_sn(header.sn);

  // Full SDU received. Clear segments and use full payload as SDU.
  rx_sdu.segments.clear();
  rx_sdu.sdu            = std::move(payload);
  rx_sdu.fully_received = true;
  rx_sdu.has_gap        = false;
}

void rlc_rx_am_entity::handle_segment_data_sdu(const rlc_am_pdu_header& header, byte_buffer_slice payload)
{
  if (header.si == rlc_si_field::full_sdu) {
    logger.log_error("called {} with full SDU. Header: {}", __FUNCTION__, header);
    return;
  }

  // Log SDU segment reception
  logger.log_debug("PDU SI: {}", header.sn);

  // Add new SN to RX window if no segments have been received yet
  rlc_rx_am_sdu_info& rx_sdu = rx_window->has_sn(header.sn) ? (*rx_window)[header.sn] : rx_window->add_sn(header.sn);

  // Create SDU segment, to be stored later
  rlc_rx_am_sdu_segment segment = {};
  segment.header                = header;
  segment.payload               = std::move(payload);

  // Store SDU segment. Sort by SO and check for duplicate bytes.
  rx_sdu.segments.insert(std::move(segment));

  // Check whether all segments have been received
  update_segment_inventory(rx_sdu);
  if (rx_sdu.fully_received) {
    logger.log_info("Segmented SDU completed. SN={}.", header.sn);

    // Assemble SDU from segments
    for (const rlc_rx_am_sdu_segment& segm : rx_sdu.segments) {
      if (segm.header.so + segm.payload.length() > rx_sdu.sdu.length()) {
        uint32_t piece_length = segm.header.so + segm.payload.length() - rx_sdu.sdu.length();
        rx_sdu.sdu.push_back(segm.payload.make_slice(segm.payload.length() - piece_length, piece_length));
      }
    }
  }
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
    if (segm.header.so != next_byte) {
      // Found gap: set flags and return
      rx_sdu.has_gap        = true;
      rx_sdu.fully_received = false;
      return;
    }
    if (segm.header.si == rlc_si_field::last_segment) {
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
  std::unique_lock<std::mutex> lock(status_report_mutex);
  status_report.reset();
  /*
   * - for the RLC SDUs with SN such that RX_Next <= SN < RX_Highest_Status that has not been completely
   *   received yet, in increasing SN order of RLC SDUs and increasing byte segment order within RLC SDUs,
   *   starting with SN = RX_Next up to the point where the resulting STATUS PDU still fits to the total size of RLC
   *   PDU(s) indicated by lower layer:
   */
  logger.log_debug("Generating status PDU");
  for (uint32_t i = st.rx_next; rx_mod_base(i) < rx_mod_base(st.rx_highest_status); i = (i + 1) % mod) {
    if ((rx_window->has_sn(i) && (*rx_window)[i].fully_received)) {
      logger.log_debug("SDU SN={} is fully received", i);
    } else {
      if (not rx_window->has_sn(i)) {
        // No segment received, NACK the whole SDU
        rlc_am_status_nack nack;
        nack.nack_sn = i;
        nack.has_so  = false;
        logger.log_debug("Adding NACK={}", nack);
        status_report.push_nack(nack);
      } else if (not(*rx_window)[i].fully_received) {
        // Some segments were received, but not all.
        // NACK non consecutive missing bytes
        uint32_t last_so           = 0;
        bool     have_last_segment = false;
        for (auto segm = (*rx_window)[i].segments.begin(); segm != (*rx_window)[i].segments.end(); segm++) {
          if (segm->header.so != last_so) {
            // Some bytes were not received
            rlc_am_status_nack nack;
            nack.nack_sn  = i;
            nack.has_so   = true;
            nack.so_start = last_so;
            nack.so_end   = segm->header.so - 1; // set to last missing byte
            logger.log_debug("Adding NACK={}", nack);
            status_report.push_nack(nack);

            // Sanity check
            if (nack.so_start > nack.so_end) {
              // Print segment list
              for (auto segm_it = (*rx_window)[i].segments.begin(); segm_it != (*rx_window)[i].segments.end();
                   segm_it++) {
                logger.log_error("Segment: so={}, length={}", segm_it->header.so, segm_it->payload.length());
              }
              logger.log_error("Error: so_start > so_end in NACK={}, Segment: so={}", nack, segm->header.so);
              srsgnb_assert(nack.so_start <= nack.so_end,
                            "Error: so_start > so_end in NACK={}, Segment: so={}",
                            nack,
                            segm->header.so);
            }
          }
          if (segm->header.si == rlc_si_field::last_segment) {
            have_last_segment = true;
          }
          last_so = segm->header.so + segm->payload.length();
        } // Segment loop

        // Check for last segment
        if (not have_last_segment) {
          rlc_am_status_nack nack;
          nack.nack_sn  = i;
          nack.has_so   = true;
          nack.so_start = last_so;
          nack.so_end   = rlc_am_status_nack::so_end_of_sdu;
          logger.log_debug("Adding NACK={}", nack);
          status_report.push_nack(nack);
          // Sanity check
          srsgnb_assert(nack.so_start <= nack.so_end, "Error: so_start > so_end in NACK={}", nack);
        }
      }
    }
  } // NACK loop

  /*
   * - set the ACK_SN to the SN of the next not received RLC SDU which is not
   * indicated as missing in the resulting STATUS PDU.
   */
  status_report.ack_sn = st.rx_highest_status;
  logger.log_debug("Refreshed status_report: {}", status_report);
}

rlc_am_status_pdu rlc_rx_am_entity::get_status_pdu()
{
  do_status.store(false, std::memory_order_relaxed);
  if (status_prohibit_timer.is_valid() && cfg.t_status_prohibit > 0) {
    status_prohibit_timer.run();
  }
  std::unique_lock<std::mutex> lock(status_report_mutex);
  return status_report;
}

uint32_t rlc_rx_am_entity::get_status_pdu_length()
{
  std::unique_lock<std::mutex> lock(status_report_mutex);
  return status_report.get_packed_size();
}

bool rlc_rx_am_entity::status_report_required()
{
  return do_status.load(std::memory_order_relaxed) && not status_prohibit_timer.is_running();
}

void rlc_rx_am_entity::notify_status_report_changed()
{
  if (status_report_required()) {
    logger.log_debug("Notifying TX that status report has changed");
    status_notifier->on_status_report_changed();
  }
}

std::unique_ptr<rlc_am_window_base<rlc_rx_am_sdu_info>> rlc_rx_am_entity::create_rx_window(rlc_am_sn_size sn_size)
{
  std::unique_ptr<rlc_am_window_base<rlc_rx_am_sdu_info>> rx_window_;
  switch (sn_size) {
    case rlc_am_sn_size::size12bits:
      rx_window_ =
          std::make_unique<rlc_am_window<rlc_rx_am_sdu_info, window_size(to_number(rlc_am_sn_size::size12bits))>>(
              logger);
      break;
    case rlc_am_sn_size::size18bits:
      rx_window_ =
          std::make_unique<rlc_am_window<rlc_rx_am_sdu_info, window_size(to_number(rlc_am_sn_size::size18bits))>>(
              logger);
      break;
    default:
      srsgnb_assertion_failure("Cannot create rx_window: unsupported SN field length");
  }
  return rx_window_;
}

/*
 * Timer handling functions
 */

void rlc_rx_am_entity::on_expired_status_prohibit_timer(uint32_t timeout_id)
{
  if (status_prohibit_timer.is_valid() && status_prohibit_timer.id() == timeout_id) {
    logger.log_debug("Status prohibit timer expired after {}ms", status_prohibit_timer.duration());
    notify_status_report_changed();
    return;
  }
}

void rlc_rx_am_entity::on_expired_reassembly_timer(uint32_t timeout_id)
{
  // Reassembly
  if (reassembly_timer.is_valid() && reassembly_timer.id() == timeout_id) {
    logger.log_debug("Reassembly timer expired after {}ms", reassembly_timer.duration());
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
    uint32_t sn_upd = {};
    for (sn_upd = st.rx_next_status_trigger; rx_mod_base(sn_upd) < rx_mod_base(st.rx_next_highest);
         sn_upd = (sn_upd + 1) % mod) {
      if (not rx_window->has_sn(sn_upd) || (rx_window->has_sn(sn_upd) && not(*rx_window)[sn_upd].fully_received)) {
        break;
      }
    }
    if (not valid_ack_sn(sn_upd)) {
      logger.log_error("rx_highest_status not inside RX window. sn_upd={}, st={}", sn_upd, st);
    }
    srsgnb_assert(
        valid_ack_sn(sn_upd), "Error: rx_highest_status assigned outside rx window. sn_upd={}, st={}", sn_upd, st);
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
    }

    /* 5.3.4 Status reporting:
     * - The receiving side of an AM RLC entity shall trigger a STATUS report when t-Reassembly expires.
     *   NOTE 2: The expiry of t-Reassembly triggers both RX_Highest_Status to be updated and a STATUS report to be
     *   triggered, but the STATUS report shall be triggered after RX_Highest_Status is updated.
     */
    refresh_status_report();
    do_status.store(true, std::memory_order_relaxed);
    notify_status_report_changed();

    logger.log_debug("State: {}", st);
    logger.log_debug("SDUs in rx_window: {}", st.rx_next, st.rx_next_highest, rx_window->size());
    return;
  }
}
