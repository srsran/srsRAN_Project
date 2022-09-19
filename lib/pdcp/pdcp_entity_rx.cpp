/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_entity_rx.h"

using namespace srsgnb;

void pdcp_entity_rx::handle_pdu(byte_buffer buf)
{
  // Log PDU
  logger.log_info(buf.begin(),
                  buf.end(),
                  "RX PDU ({} B), integrity={}, ciphering={}",
                  buf.length(),
                  integrity_enabled,
                  ciphering_enabled);

  // TODO Config max HFN and notify RRC
  // if (rx_overflow) {
  //  logger.warning("Rx PDCP COUNTs have overflowed. Discarding SDU.");
  //  return;
  // }

  // Sanity check
  if (buf.length() <= hdr_len_bytes) {
    return;
  }
  logger.log_debug("Rx PDCP state - RX_NEXT={}, RX_DELIV={}, RX_REORD={}", st.rx_next, st.rx_deliv, st.rx_reord);

  // Extract RCVD_SN from header
  uint32_t rcvd_sn = {};
  if (not read_data_pdu_header(buf, rcvd_sn)) {
    logger.log_error("Error extracting PDCP SN");
    return;
  }

  /*
   * Calculate RCVD_COUNT:
   *
   * - if RCVD_SN < SN(RX_DELIV) – Window_Size:
   *   - RCVD_HFN = HFN(RX_DELIV) + 1.
   * - else if RCVD_SN >= SN(RX_DELIV) + Window_Size:
   *   - RCVD_HFN = HFN(RX_DELIV) – 1.
   * - else:
   *   - RCVD_HFN = HFN(RX_DELIV);
   * - RCVD_COUNT = [RCVD_HFN, RCVD_SN].
   */
  uint32_t rcvd_hfn, __attribute__((unused)) rcvd_count;
  if ((int64_t)rcvd_sn < (int64_t)SN(st.rx_deliv) - (int64_t)window_size) {
    rcvd_hfn = HFN(st.rx_deliv) + 1;
  } else if (rcvd_sn >= SN(st.rx_deliv) + window_size) {
    rcvd_hfn = HFN(st.rx_deliv) - 1;
  } else {
    rcvd_hfn = HFN(st.rx_deliv);
  }
  rcvd_count = COUNT(rcvd_hfn, rcvd_sn);

  /*
   * TS 38.323, section 5.8: Deciphering
   *
   * The data unit that is ciphered is the MAC-I and the
   * data part of the PDCP Data PDU except the
   * SDAP header and the SDAP Control PDU if included in the PDCP SDU.
   */
  if (ciphering_enabled == pdcp_ciphering_enabled::enabled) {
    // cipher_decrypt(
    //     &pdu->msg[cfg.hdr_len_bytes], pdu->N_bytes - cfg.hdr_len_bytes, rcvd_count, &pdu->msg[cfg.hdr_len_bytes]);
  }

  /*
   * Extract MAC-I:
   * Always extract from SRBs, only extract from DRBs if integrity is enabled
   */
  // sec_mac mac = {};
  if (is_srb() || (is_drb() && (integrity_enabled == pdcp_integrity_enabled::enabled))) {
    // extract_mac(pdu, mac);
  }

  // After checking the integrity, we can discard the header.
  // discard_data_header(buf);

  /*
   * Check valid rcvd_count:
   *
   * - if RCVD_COUNT < RX_DELIV; or
   * - if the PDCP Data PDU with COUNT = RCVD_COUNT has been received before:
   *   - discard the PDCP Data PDU;
   */
  if (rcvd_count < st.rx_deliv) {
    logger.log_debug("Out-of-order after time-out, duplicate or COUNT wrap-around");
    logger.log_debug("RCVD_COUNT %u, RCVD_COUNT %u", rcvd_count, st.rx_deliv);
    return; // Invalid count, drop.
  }

  // Check if PDU has been received
  if (reorder_queue.find(rcvd_count) != reorder_queue.end()) {
    logger.log_debug("Duplicate PDU, dropping");
    return; // PDU already present, drop.
  }

  // Store PDU in reception buffer
  reorder_queue[rcvd_count] = std::move(buf);

  // Update RX_NEXT
  if (rcvd_count >= st.rx_next) {
    st.rx_next = rcvd_count + 1;
  }

  // TODO if out-of-order configured, submit to upper layer

  if (rcvd_count == st.rx_deliv) {
    // Deliver to upper layers in ascending order of associated COUNT
    // deliver_all_consecutive_counts();
    upper_dn.on_new_sdu(std::move(buf)); // FIXME
  }

  // Handle reordering timers
  /*
  if (reordering_timer.is_running() and rx_deliv >= rx_reord) {
    reordering_timer.stop();
    logger.debug("Stopped t-Reordering - RX_DELIV=%d, RX_REORD=%ld", rx_deliv, rx_reord);
  }

  if (cfg.t_reordering != pdcp_t_reordering_t::infinity) {
    if (not reordering_timer.is_running() and rx_deliv < rx_next) {
      rx_reord = rx_next;
      reordering_timer.run();
      logger.debug("Started t-Reordering - RX_REORD=%ld, RX_DELIV=%ld, RX_NEXT=%ld", rx_reord, rx_deliv, rx_next);
    }
  }*/

  logger.log_debug("Rx PDCP state - RX_NEXT={}, RX_DELIV={}, RX_REORD={}", st.rx_next, st.rx_deliv, st.rx_reord);
}

bool pdcp_entity_rx::read_data_pdu_header(const byte_buffer& buf, uint32_t& sn) const
{
  byte_buffer_reader buf_reader = buf;
  if (buf_reader.empty()) {
    logger.log_warning("Unpacking header of empty PDCP PDU");
    return false;
  }

  // Check PDU is long enough to extract header
  if (buf.length() <= hdr_len_bytes) {
    logger.log_error("PDU too small to extract header");
    return false;
  }

  // Extract RCVD_SN
  switch (cfg.sn_size) {
    case pdcp_sn_size::size12bits:
      sn = (*buf_reader & 0x0fU) << 8U; // first 4 bits SN
      ++buf_reader;
      sn |= (*buf_reader & 0xffU); // last 8 bits SN
      ++buf_reader;
      break;
    case pdcp_sn_size::size18bits:
      sn = (*buf_reader & 0x03U) << 16U; // first 2 bits SN
      ++buf_reader;
      sn |= (*buf_reader & 0xffU) << 8U; // middle 8 bits SN
      ++buf_reader;
      sn |= (*buf_reader & 0xffU); // last 8 bits SN
      ++buf_reader;
      break;
    default:
      logger.log_error("Cannot extract RCVD_SN, invalid SN length configured: {}", cfg.sn_size);
      return false;
  }
  return true;
}
