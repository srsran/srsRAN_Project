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
#include "srsgnb/support/srsgnb_assert.h"

using namespace srsgnb;

rlc_tx_am_entity::rlc_tx_am_entity(du_ue_index_t                        du_index,
                                   lcid_t                               lcid,
                                   const rlc_tx_am_config&              config,
                                   rlc_tx_upper_layer_data_notifier&    upper_dn,
                                   rlc_tx_upper_layer_control_notifier& upper_cn,
                                   rlc_tx_buffer_state_update_notifier& buffer_state_notif,
                                   timer_manager&                       timers) :
  rlc_tx_entity(du_index, lcid, upper_dn, upper_cn, buffer_state_notif),
  cfg(config),
  mod(cardinality(to_number(cfg.sn_field_length))),
  tx_window_size(window_size(to_number(cfg.sn_field_length))),
  tx_window(create_tx_window(cfg.sn_field_length)),
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
  std::lock_guard<std::mutex> lock(mutex);

  logger.log_debug("MAC opportunity - bytes={}, tx_window size={} PDUs", nof_bytes, tx_window->size());

  // Tx STATUS if requested
  if (status_provider->status_report_required()) {
    rlc_am_status_pdu status_pdu = status_provider->get_status_pdu();
    byte_buffer       pdu;
    status_pdu.pack(pdu);
    logger.log_debug("Status PDU built - {} bytes", pdu.length());
    return byte_buffer_slice_chain(std::move(pdu));
  }

  // Retransmit if required
  if (not retx_queue.empty()) {
    logger.log_info("Re-transmission required. Retransmission queue size: {}", retx_queue.size());
    return build_retx_pdu(nof_bytes);
  }

  // Send remaining segment, if it exists
  if (sn_under_segmentation != INVALID_RLC_SN) {
    if (tx_window->has_sn(sn_under_segmentation)) {
      return build_continuation_pdu_segment((*tx_window)[sn_under_segmentation], nof_bytes);
    } else {
      sn_under_segmentation = INVALID_RLC_SN;
      logger.log_error("SDU currently being segmented does not exist in tx_window. Aborting segmentation SN={}",
                       sn_under_segmentation);
      // attempt to send next SDU
    }
  }

  // Check whether there is something to TX
  if (sdu_queue.is_empty()) {
    logger.log_debug("No data available to be sent");
    return {};
  }

  return build_new_pdu(nof_bytes);
}

byte_buffer_slice_chain rlc_tx_am_entity::build_new_pdu(uint32_t nof_bytes)
{
  if (nof_bytes <= head_min_size) {
    logger.log_debug("Not enough bytes for payload plus header. nof_bytes={}", nof_bytes);
    return {};
  }

  // do not build any more PDU if window is already full
  if (tx_window->full()) {
    logger.log_warning("Cannot build data PDU - Tx window full.");
    return {};
  }

  // Read new SDU from TX queue
  rlc_sdu sdu;
  logger.log_debug(
      "Reading from RLC SDU queue. Queue size {} SDUs ({} bytes)", sdu_queue.size_sdus(), sdu_queue.size_bytes());
  if (not sdu_queue.read(sdu)) {
    logger.log_debug("No SDUs left in the tx queue.");
    return {};
  }
  logger.log_debug("Read RLC SDU - RLC_SN={}, PDCP_SN={}, {} bytes", st.tx_next, sdu.pdcp_sn, sdu.buf.length());

  // insert newly assigned SN into window and use reference for in-place operations
  // NOTE: from now on, we can't return from this function anymore before increasing tx_next
  rlc_tx_amd_pdu_box& tx_pdu = tx_window->add_pdu(st.tx_next);
  tx_pdu.pdcp_sn             = sdu.pdcp_sn;
  tx_pdu.sdu                 = std::move(sdu.buf); // Move SDU into TX window SDU box

  // Segment new SDU if necessary
  if (tx_pdu.sdu.length() + head_min_size > nof_bytes) {
    logger.log_info("Trying to build PDU segment from SDU.");
    return build_new_pdu_segment(tx_pdu, nof_bytes);
  }
  logger.log_info("Creating PDU. Tx SDU ({} B), nof_bytes={} B ", tx_pdu.sdu.length(), nof_bytes);

  // Prepare header
  rlc_am_pdu_header hdr = {};
  hdr.dc                = rlc_dc_field::data;
  hdr.p                 = get_polling_bit(st.tx_next, /* is_retx = */ false, sdu.buf.length());
  hdr.si                = rlc_si_field::full_sdu;
  hdr.sn_size           = cfg.sn_field_length;
  hdr.sn                = st.tx_next;
  tx_pdu.header         = hdr;
  logger.log_debug("AMD PDU header: {}", hdr);

  // Pack header
  byte_buffer header_buf = {};
  if (!rlc_am_write_data_pdu_header(hdr, header_buf)) {
    // TODO: actually, pack function always returns true, so we never come here
    logger.log_error("Failed to pack AMD PDU header");
  }

  // Assemble PDU
  byte_buffer_slice_chain pdu_buf = {};
  pdu_buf.push_front(std::move(header_buf));
  pdu_buf.push_back(sdu.buf);
  logger.log_debug("Created RLC PDU - {} bytes", pdu_buf.length());

  // Update TX Next
  st.tx_next = (st.tx_next + 1) % mod;

  return pdu_buf;
}

byte_buffer_slice_chain rlc_tx_am_entity::build_new_pdu_segment(rlc_tx_amd_pdu_box& tx_pdu, uint32_t nof_bytes)
{
  logger.log_info("Creating new PDU segment. Tx SDU ({} B), nof_bytes={} B ", tx_pdu.sdu.length(), nof_bytes);

  // Sanity check: can this SDU be sent this in a single PDU?
  if ((tx_pdu.sdu.length() + head_min_size) <= nof_bytes) {
    logger.log_error(
        "Calling build_new_sdu_segment(), but there are enough bytes to tx in a single PDU. Tx SDU ({} B), "
        "nof_bytes={} B",
        tx_pdu.sdu.length(),
        nof_bytes);
    return {};
  }

  // Sanity check: can this SDU be sent considering header overhead?
  if (nof_bytes <= head_min_size) { // Small header, since first segment has no SO field, ref: TS 38.322 Sec. 6.2.2.4
    logger.log_info(
        "Cannot build new sdu_segment, there are not enough bytes allocated to tx header plus data. nof_bytes={}, "
        "head_min_size={}",
        nof_bytes,
        head_min_size);
    return {};
  }

  uint32_t segment_payload_len = nof_bytes - head_min_size;

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
  hdr.so                = tx_pdu.next_so;
  tx_pdu.header         = hdr;
  logger.log_debug("AMD PDU header: {}", hdr);

  // Pack header
  byte_buffer header_buf = {};
  if (!rlc_am_write_data_pdu_header(hdr, header_buf)) {
    // TODO: actually, pack function always returns true, so we never come here
    logger.log_error("Failed to pack AMD PDU header");
  }

  // Assemble PDU
  byte_buffer_slice_chain pdu_buf = {};
  pdu_buf.push_front(std::move(header_buf));
  pdu_buf.push_back(tx_pdu.sdu.make_slice(hdr.so, segment_payload_len));
  logger.log_debug("Created RLC PDU segment - {} bytes", pdu_buf.length());

  // Store segmentation progress
  tx_pdu.next_so += segment_payload_len;

  return pdu_buf;
}

byte_buffer_slice_chain rlc_tx_am_entity::build_continuation_pdu_segment(rlc_tx_amd_pdu_box& tx_pdu, uint32_t nof_bytes)
{
  logger.log_info("Creating continued PDU segment. Tx SDU ({} B), nof_bytes={} B ", tx_pdu.sdu.length(), nof_bytes);

  // Sanity check: is there an initial SDU segment?
  if (tx_pdu.next_so == 0) {
    logger.log_error(
        "build_continuation_sdu_segment was called, but there was no initial segment. SN={}, Tx SDU ({} B), "
        "nof_bytes={} B ",
        sn_under_segmentation,
        tx_pdu.sdu.length(),
        nof_bytes);
    sn_under_segmentation = INVALID_RLC_SN;
    return {};
  }

  logger.log_debug("continuing SDU segment. SN={}, next_so={}", sn_under_segmentation, tx_pdu.next_so);

  // Sanity check: last byte must be smaller than SDU size
  if (tx_pdu.next_so >= tx_pdu.sdu.length()) {
    logger.log_error(
        "segmentation progress exceeds SDU length. SDU len={} B, next_so={} B", tx_pdu.sdu.length(), tx_pdu.next_so);
    sn_under_segmentation = INVALID_RLC_SN;
    return {};
  }

  // Sanity check: can this SDU be sent considering header overhead?
  if (nof_bytes <= head_max_size) { // Large header, since continued segment has SO field, ref: TS 38.322 Sec. 6.2.2.4
    logger.log_info(
        "Cannot build new sdu_segment, there are not enough bytes allocated to tx header plus data. nof_bytes={}, "
        "head_max_size={}",
        nof_bytes,
        head_max_size);
    return {};
  }

  uint32_t     segment_payload_len = tx_pdu.sdu.length() - tx_pdu.next_so;
  rlc_si_field si                  = {};

  if (segment_payload_len + head_max_size > nof_bytes) {
    logger.log_info(
        "grant is not large enough for remaining SDU bytes. SDU bytes left {}, head_max_size={}, nof_bytes {}",
        segment_payload_len,
        head_max_size,
        nof_bytes);
    si                  = rlc_si_field::middle_segment;
    segment_payload_len = nof_bytes - head_max_size;
  } else {
    logger.log_info("grant is large enough for remaining SDU bytes. SDU bytes left {}, head_max_size={}, nof_bytes {}",
                    segment_payload_len,
                    head_max_size,
                    nof_bytes);
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
  hdr.so                = tx_pdu.next_so;
  tx_pdu.header         = hdr;
  logger.log_debug("AMD PDU header: {}", hdr);

  // Pack header
  byte_buffer header_buf = {};
  if (!rlc_am_write_data_pdu_header(hdr, header_buf)) {
    // TODO: actually, pack function always returns true, so we never come here
    logger.log_error("Failed to pack AMD PDU header");
  }

  // Assemble PDU
  byte_buffer_slice_chain pdu_buf = {};
  pdu_buf.push_front(std::move(header_buf));
  pdu_buf.push_back(tx_pdu.sdu.make_slice(hdr.so, segment_payload_len));
  logger.log_debug("Created RLC PDU segment - {} bytes", pdu_buf.length());

  // Store segmentation progress
  tx_pdu.next_so += segment_payload_len;

  // Update TX Next (when segmentation has finished)
  if (si == rlc_si_field::last_segment) {
    st.tx_next = (st.tx_next + 1) % mod;
  }

  return pdu_buf;
}

byte_buffer_slice_chain rlc_tx_am_entity::build_retx_pdu(uint32_t nof_bytes)
{
  // Check there is at least 1 element before calling front()
  if (retx_queue.empty()) {
    logger.log_error("in build_retx_pdu(): retx_queue is empty");
    return {};
  }

  // Sanity check - drop any retx SNs not present in tx_window
  while (not tx_window->has_sn(retx_queue.front().sn)) {
    logger.log_info("SN={} not in tx window, probably already ACKed. Skip and remove from retx queue",
                    retx_queue.front().sn);
    retx_queue.pop();
    if (retx_queue.empty()) {
      logger.log_info("empty retx queue, cannot provide any retx PDU");
      return {};
    }
  }

  const rlc_tx_amd_retx retx = retx_queue.front(); // local copy, since front may change below
  logger.log_debug("RETX: {}", retx);

  // Get tx_pdu info from tx_window
  rlc_tx_amd_pdu_box& tx_pdu = (*tx_window)[retx.sn];

  // Check ReTx boundaries
  if (retx.so + retx.length > tx_pdu.sdu.length()) {
    logger.log_error(
        "Skipping invalid ReTx that exceeds SDU boundaries. SDU length={}, ReTx: {}", tx_pdu.sdu.length(), retx);
    retx_queue.pop();
    return {};
  }

  // Get expected header and payload len
  uint32_t expected_hdr_len = get_retx_expected_hdr_len(retx);
  uint32_t retx_payload_len = std::min(retx.length, nof_bytes - expected_hdr_len);
  bool     pdu_complete     = retx_payload_len == retx.length;

  // Configure SI
  rlc_si_field si = rlc_si_field::full_sdu;
  if (retx.so == 0) {
    // either full PDU or first segment
    if (pdu_complete) {
      si = rlc_si_field::full_sdu;
    } else {
      si = rlc_si_field::first_segment;
    }
  } else {
    // either middle segment or last segment
    if (pdu_complete) {
      si = rlc_si_field::last_segment;
    } else {
      si = rlc_si_field::middle_segment;
    }
  }

  // Log ReTx info
  logger.log_debug("Creating PDU{} ({}) for ReTx [{}], nof_bytes={}, expected_hdr_len={}, retx_payload_len={}",
                   si == rlc_si_field::full_sdu ? "" : " segment",
                   si,
                   retx,
                   nof_bytes,
                   expected_hdr_len,
                   retx_payload_len);

  // Update ReTx queue. This must be done before calculating
  // the polling bit, to make sure the poll bit is calculated correctly
  if (pdu_complete) {
    // remove ReTx from queue
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
  tx_pdu.header         = hdr;
  logger.log_debug("AMD PDU header: {}", hdr);

  // Pack header
  byte_buffer header_buf = {};
  if (!rlc_am_write_data_pdu_header(hdr, header_buf)) {
    // TODO: actually, pack function always returns true, so we never come here
    logger.log_error("Failed to pack AMD PDU header");
  }
  srsgnb_assert(header_buf.length() == expected_hdr_len,
                "ReTx header length ({}) differs from expected_hdr_len ({})",
                header_buf.length(),
                expected_hdr_len);

  // Assemble PDU
  byte_buffer_slice_chain pdu_buf = {};
  pdu_buf.push_front(std::move(header_buf));
  pdu_buf.push_back(tx_pdu.sdu.make_slice(hdr.so, retx_payload_len));
  logger.log_debug("Created RLC ReTx PDU{} ({}) - {} bytes",
                   hdr.si == rlc_si_field::full_sdu ? "" : " segment",
                   si,
                   pdu_buf.length());

  // Log state
  log_state(srslog::basic_levels::debug);

  return pdu_buf;
}

void rlc_tx_am_entity::handle_status_pdu(rlc_am_status_pdu status)
{
  std::lock_guard<std::mutex> lock(mutex);
  logger.log_info("Processing status PDU: {}", status);

  /*
   * Sanity check the received status report.
   * Checking if the ACK_SN is inside the valid ACK_SN window (the TX window "off-by-one")
   * makes sure we discard out of order status reports.
   * Checking if ACK_SN > Tx_Next + 1 makes sure we do not receive a ACK/NACK for something we did not TX
   * ACK_SN may be equal to TX_NEXT + 1, if not all SDU segments with SN=TX_NEXT have been transmitted.
   */
  if (not valid_ack_sn(status.ack_sn)) {
    logger.log_info(
        "Received ACK with SN outside of TX_WINDOW, ignoring status report. ACK_SN={}, st=[{}]", status.ack_sn, st);
    return;
  }

  if (tx_mod_base(status.ack_sn) > tx_mod_base(st.tx_next + 1)) {
    logger.log_warning(
        "Received ACK with SN larger than TX_NEXT, ignoring status report. ACK_SN={}, st=[{}]", status.ack_sn, st);
    return;
  }

  /**
   * Section 5.3.3.3: Reception of a STATUS report
   * - if the STATUS report comprises a positive or negative acknowledgement for the RLC SDU with sequence
   *   number equal to POLL_SN:
   *   - if t-PollRetransmit is running:
   *     - stop and reset t-PollRetransmit.
   */
  if (tx_mod_base(st.poll_sn) < tx_mod_base(status.ack_sn)) {
    if (poll_retransmit_timer.is_running()) {
      logger.log_debug("Received ACK or NACK for POLL_SN={}. Stopping t-PollRetransmit", st.poll_sn);
      poll_retransmit_timer.stop();
    } else {
      logger.log_debug("Received ACK or NACK for POLL_SN={}. t-PollRetransmit already stopped", st.poll_sn);
    }
  } else {
    logger.log_debug("POLL_SN={} > ACK_SN={}. Not stopping t-PollRetransmit ", st.poll_sn, status.ack_sn);
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
  for (uint32_t sn = st.tx_next_ack; tx_mod_base(sn) < tx_mod_base(stop_sn); sn = (sn + 1) % mod) {
    if (tx_window->has_sn(sn)) {
      upper_dn.on_delivered_sdu((*tx_window)[sn].pdcp_sn); // notify upper layer
      retx_queue.remove_sn(sn);                            // remove any pending retx for that SN
      tx_window->remove_pdu(sn);
      st.tx_next_ack = (sn + 1) % mod;
    } else {
      logger.log_error("Missing ACKed SN from TX window");
      break;
    }
  }
  logger.log_debug("Processed status report ACKs. ACK_SN={}. Tx_Next_Ack={}", status.ack_sn, st.tx_next_ack);

  // Process NACKs
  std::set<uint32_t> retx_sn_set; // Set of PDU SNs added for retransmission (no duplicates)
  for (uint32_t nack_idx = 0; nack_idx < status.get_nacks().size(); nack_idx++) {
    if (status.get_nacks()[nack_idx].has_nack_range) {
      for (uint32_t range_sn = status.get_nacks()[nack_idx].nack_sn;
           range_sn < status.get_nacks()[nack_idx].nack_sn + status.get_nacks()[nack_idx].nack_range;
           range_sn++) {
        rlc_am_status_nack nack = {};
        nack.nack_sn            = range_sn;
        if (status.get_nacks()[nack_idx].has_so) {
          // Apply so_start to first range item
          if (range_sn == status.get_nacks()[nack_idx].nack_sn) {
            nack.so_start = status.get_nacks()[nack_idx].so_start;
          }
          // Apply so_end to last range item
          if (range_sn == (status.get_nacks()[nack_idx].nack_sn + status.get_nacks()[nack_idx].nack_range - 1)) {
            nack.so_end = status.get_nacks()[nack_idx].so_end;
          }
          // Enable has_so only if the offsets do not span the whole SDU
          nack.has_so = (nack.so_start != 0) || (nack.so_end != rlc_am_status_nack::so_end_of_sdu);
        }
        if (handle_nack(nack)) {
          retx_sn_set.insert(nack.nack_sn);
        }
      }
    } else {
      if (handle_nack(status.get_nacks()[nack_idx])) {
        retx_sn_set.insert(status.get_nacks()[nack_idx].nack_sn);
      }
    }
  }

  // Process retx_count and inform upper layers if needed
  for (uint32_t retx_sn : retx_sn_set) {
    auto& pdu = (*tx_window)[retx_sn];
    // Increment retx_count
    if (pdu.retx_count == RETX_COUNT_NOT_STARTED) {
      // Set retx_count = 0 on first RE-transmission of associated SDU (38.322 Sec. 5.3.2)
      pdu.retx_count = 0;
    } else {
      // Increment otherwise
      pdu.retx_count++;
    }

    // Inform upper layers if needed
    check_sn_reached_max_retx(retx_sn);
  }
}

bool rlc_tx_am_entity::handle_nack(rlc_am_status_nack nack)
{
  if (nack.has_nack_range) {
    logger.log_error("handle_nack must not be called with nacks that have a nack range. Ignoring NACK={}", nack);
    return false;
  }

  logger.log_debug("Handling NACK={}", nack);

  // Check if NACK applies to a SN within tx window
  if (!(tx_mod_base(st.tx_next_ack) <= tx_mod_base(nack.nack_sn) &&
        tx_mod_base(nack.nack_sn) <= tx_mod_base(st.tx_next))) {
    logger.log_info(
        "NACK SN not in expected range. Tx_Next_Ack={}, Tx_Next={}, NACK={}", nack.nack_sn, st.tx_next_ack, st.tx_next);
    return false;
  }

  uint32_t sdu_length = (*tx_window)[nack.nack_sn].sdu.length();

  // Convert NACK for full PDUs into NACK with segment offset and length
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
    logger.log_warning(
        "Invalid NACK: so_start > so_end. sdu_length={}, NACK={}. Setting so_start to 0", sdu_length, nack);
    nack.so_start = 0;
  }
  if (nack.so_start >= sdu_length) {
    logger.log_warning(
        "Invalid NACK: so_start out of bounds. sdu_length={}, NACK={}. Setting so_start to 0", sdu_length, nack);
    nack.so_start = 0;
  }
  if (nack.so_end >= sdu_length) {
    logger.log_warning("Invalid NACK: so_end out of bounds. sdu_length={}, NACK={}. Setting so_end to {}",
                       sdu_length,
                       nack,
                       sdu_length - 1);
    nack.so_end = sdu_length - 1;
  }

  // Enqueue ReTx
  if (!retx_queue.has_sn(nack.nack_sn, nack.so_start, nack.so_end - nack.so_start + 1)) {
    rlc_tx_amd_retx retx = {};
    retx.so              = nack.so_start;
    retx.sn              = st.tx_next_ack;
    retx.length          = nack.so_end - nack.so_start + 1;
    retx_queue.push(retx);
    logger.log_debug("Scheduled ReTx=[{}]. NACK={}", retx, nack);
  } else {
    logger.log_info("NACK'ed PDU or PDU segment is already queued for ReTx. NACK={}", nack);
    return false;
  }

  return true;
}

void rlc_tx_am_entity::check_sn_reached_max_retx(uint32_t sn)
{
  if ((*tx_window)[sn].retx_count == cfg.max_retx_thresh) {
    logger.log_warning("Signaling max number of reTx={} for SN={}", (*tx_window)[sn].retx_count, sn);
    upper_cn.on_max_retx();
    // TODO: notify upper layer data plane of SDU failure
    // upper_dn.on_failed_sdu((*tx_window)[sn].pdcp_sn);

    metrics_add_lost_sdus(1);
  }
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
  uint32_t queue_bytes = sdu_queue.size_bytes() + sdu_queue.size_sdus() * head_min_size;

  // minimum bytes needed to tx SDU under segmentation + header (if applicable)
  uint32_t segment_bytes = 0;
  if (sn_under_segmentation != INVALID_RLC_SN) {
    if (tx_window->has_sn(sn_under_segmentation)) {
      rlc_tx_amd_pdu_box& tx_pdu = (*tx_window)[sn_under_segmentation];
      segment_bytes              = tx_pdu.sdu.length() - tx_pdu.next_so + head_max_size;
    } else {
      logger.log_info("Buffer state: ignore SN under segmentation: SN={} not in tx_window", sn_under_segmentation);
    }
  }

  // minimum bytes needed to tx all queued ReTx + each header; ReTx can also be segments
  rlc_retx_queue_state retx_state = retx_queue.state();
  uint32_t             retx_bytes = retx_state.get_retx_bytes() + retx_state.get_n_retx_so_zero() * head_min_size +
                        retx_state.get_n_retx_so_nonzero() * head_max_size;

  // TODO: status report size
  uint32_t status_bytes = 0;

  return queue_bytes + segment_bytes + retx_bytes + status_bytes;
}

uint8_t rlc_tx_am_entity::get_polling_bit(uint32_t sn, bool is_retx, uint32_t payload_size)
{
  logger.log_debug("Checking poll bit requirements for PDU. SN={}, retx={}, sdu_bytes={}, POLL_SN={}",
                   sn,
                   is_retx ? "true" : "false",
                   payload_size,
                   st.poll_sn);
  /* For each AMD PDU or AMD PDU segment that has not been previoulsy tranmitted:
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
      logger.log_debug("Setting poll bit due to PollPDU. SN={}, POLL_SN={}", sn, st.poll_sn);
    }
    if (cfg.poll_byte > 0 && st.byte_without_poll >= (uint32_t)cfg.poll_byte) {
      poll = 1;
      logger.log_debug("Setting poll bit due to PollBYTE. SN={}, POLL_SN={}", sn, st.poll_sn);
    }
  }

  /*
   * - if both the transmission buffer and the retransmission buffer becomes empty
   *   (excluding transmitted RLC SDUs or RLC SDU segments awaiting acknowledgements)
   *   after the transmission of the AMD PDU; or
   * - if no new RLC SDU can be transmitted after the transmission of the AMD PDU (e.g. due to window stalling);
   *   - include a poll in the AMD PDU as described below.
   */
  if ((sdu_queue.is_empty() && retx_queue.empty() && sn_under_segmentation == INVALID_RLC_SN) || tx_window->full()) {
    logger.log_debug("Setting poll bit due to empty buffers/inablity to TX. SN={}, POLL_SN={}", sn, st.poll_sn);
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
      logger.log_debug("Setting new POLL_SN. POLL_SN={}", sn);
    }
    if (cfg.t_poll_retx > 0) {
      if (not poll_retransmit_timer.is_running()) {
        poll_retransmit_timer.run();
      } else {
        poll_retransmit_timer.stop();
        poll_retransmit_timer.run();
      }
      logger.log_debug("Started t-PollRetransmit. POLL_SN={}", st.poll_sn);
    }
  }
  return poll;
}

void rlc_tx_am_entity::timer_expired(uint32_t timeout_id)
{
  std::unique_lock<std::mutex> lock(mutex);

  // t-PollRetransmit
  if (poll_retransmit_timer.is_valid() && poll_retransmit_timer.id() == timeout_id) {
    logger.log_debug("Poll retransmission timer expired after {}ms", poll_retransmit_timer.duration());
    log_state(srslog::basic_levels::debug);
    /*
     * - if both the transmission buffer and the retransmission buffer are empty
     *   (excluding transmitted RLC SDU or RLC SDU segment awaiting acknowledgements); or
     * - if no new RLC SDU or RLC SDU segment can be transmitted (e.g. due to window stalling):
     *   - consider the RLC SDU with the highest SN among the RLC SDUs submitted to lower layer for
     *   retransmission; or
     *   - consider any RLC SDU which has not been positively acknowledged for retransmission.
     * - include a poll in an AMD PDU as described in section 5.3.3.2.
     */
    if ((sdu_queue.is_empty() && retx_queue.empty()) || tx_window->full()) {
      if (tx_window->empty()) {
        logger.log_error(
            "t-PollRetransmit expired, but the tx_window is empty. st=[{}], tx_window_size={}", st, tx_window->size());
        return;
      }
      if (not tx_window->has_sn(st.tx_next_ack)) {
        logger.log_error(
            "t-PollRetransmit expired, but Tx_Next_Ack is not in the tx_window. st=[{}], tx_window_size={}",
            st,
            tx_window->size());
        return;
      }
      // ReTx first RLC SDU that has not been ACKed
      // or first SDU segment of the first RLC SDU
      // that has not been acked
      rlc_tx_amd_retx retx  = {};
      retx.so               = 0;
      retx.sn               = st.tx_next_ack;
      retx.length           = (*tx_window)[st.tx_next_ack].sdu.length();
      retx_queue.push(retx);
      //
      // TODO: Revise this: shall we send a minimum-sized segment instead?
      //

      logger.log_debug("Retransmission because of t-PollRetransmit. ReTx: {}", retx);
      //
      // TODO: Increment ReTx counter, handle max_retx
      //
    }
  }
}

std::unique_ptr<rlc_pdu_window_base<rlc_tx_amd_pdu_box>> rlc_tx_am_entity::create_tx_window(rlc_am_sn_size sn_size)
{
  std::unique_ptr<rlc_pdu_window_base<rlc_tx_amd_pdu_box>> tx_window;
  switch (sn_size) {
    case rlc_am_sn_size::size12bits:
      tx_window =
          std::make_unique<rlc_pdu_window<rlc_tx_amd_pdu_box, window_size(to_number(rlc_am_sn_size::size12bits))>>();
      break;
    case rlc_am_sn_size::size18bits:
      tx_window =
          std::make_unique<rlc_pdu_window<rlc_tx_amd_pdu_box, window_size(to_number(rlc_am_sn_size::size18bits))>>();
      break;
    default:
      srsgnb_assertion_failure("Cannot create tx_window: unsupported SN field length");
  }
  return tx_window;
}

bool rlc_tx_am_entity::inside_tx_window(uint32_t sn) const
{
  // TX_Next_Ack <= SN < TX_Next_Ack + AM_Window_Size
  return tx_mod_base(sn) < tx_window_size;
}

bool rlc_tx_am_entity::valid_ack_sn(uint32_t sn) const
{
  // Tx_Next_Ack < SN <= TX_Next + AM_Window_Size
  return (0 < tx_mod_base(sn)) && (tx_mod_base(sn) <= tx_window_size);
}
