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
      return build_continuation_sdu_segment((*tx_window)[sn_under_segmentation], nof_bytes);
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
    return build_new_sdu_segment(tx_pdu, nof_bytes);
  }
  logger.log_info("Creating PDU. Tx SDU ({} B), nof_bytes={} B ", tx_pdu.sdu.length(), nof_bytes);

  // Prepare header
  rlc_am_pdu_header hdr = {};
  hdr.dc                = rlc_dc_field::data;
  hdr.p                 = get_polling_bit(st.tx_next, false, sdu.buf.length());
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

  // Update TX Next
  st.tx_next = (st.tx_next + 1) % mod;

  // Assemble PDU
  byte_buffer_slice_chain pdu_buf = {};
  pdu_buf.push_front(std::move(header_buf));
  pdu_buf.push_back(sdu.buf);
  logger.log_debug("Created RLC PDU - {} bytes", pdu_buf.length());

  return pdu_buf;
}

byte_buffer_slice_chain rlc_tx_am_entity::build_new_sdu_segment(rlc_tx_amd_pdu_box& tx_pdu, uint32_t nof_bytes)
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
  tx_pdu.next_so += segment_payload_len; // Store segmentation progress

  // Save SN of PDU under segmentation
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
  hdr.so                = 0;
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
  pdu_buf.push_back(tx_pdu.sdu.make_slice(hdr.so, hdr.so + segment_payload_len));
  logger.log_debug("Created RLC PDU segment - {} bytes", pdu_buf.length());

  return pdu_buf;
}

byte_buffer_slice_chain rlc_tx_am_entity::build_continuation_sdu_segment(rlc_tx_amd_pdu_box& tx_pdu, uint32_t nof_bytes)
{
  // TODO
  return {};
}

byte_buffer_slice_chain rlc_tx_am_entity::build_retx_pdu(uint32_t nof_bytes)
{
  // TODO
  return {};
}

byte_buffer_slice_chain rlc_tx_am_entity::build_retx_pdu_without_segmentation(rlc_tx_amd_retx& retx, uint32_t nof_bytes)
{
  // TODO
  return {};
}

byte_buffer_slice_chain rlc_tx_am_entity::build_retx_pdu_with_segmentation(rlc_tx_amd_retx& retx, uint32_t nof_bytes)
{
  // TODO
  return {};
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
  // TODO: SDU under segmentation

  // TODO: retx bytes
  uint32_t retx_bytes = 0;

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
  // TODO
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
