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

void pdcp_entity_rx::handle_pdu(byte_buffer pdu)
{
  // Log PDU
  logger.log_info(pdu.begin(),
                  pdu.end(),
                  "RX PDU ({} B), integrity={}, ciphering={}",
                  pdu.length(),
                  integrity_enabled,
                  ciphering_enabled);

  // TODO Config max HFN and notify RRC
  // if (rx_overflow) {
  //  logger.warning("Rx PDCP COUNTs have overflowed. Discarding SDU.");
  //  return;
  // }

  // Sanity check
  if (pdu.length() <= hdr_len_bytes) {
    logger.log_error("PDCP PDU is too small. PDU length={}, header length={}", pdu.length(), hdr_len_bytes);
    return;
  }
  logger.log_debug("Rx PDCP state - RX_NEXT={}, RX_DELIV={}, RX_REORD={}", st.rx_next, st.rx_deliv, st.rx_reord);

  // Extract RCVD_SN from header
  uint32_t rcvd_sn = {};
  if (not read_data_pdu_header(pdu, rcvd_sn)) {
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
  uint32_t rcvd_hfn, rcvd_count;
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
  byte_buffer sdu;
  if (ciphering_enabled == pdcp_ciphering_enabled::enabled) {
    sdu = cipher_decrypt(byte_buffer_view{pdu, hdr_len_bytes, pdu.length() - hdr_len_bytes}, rcvd_count);
    pdu.trim_tail(pdu.length() - hdr_len_bytes);
    sdu.chain_before(std::move(pdu));
  } else {
    sdu = std::move(pdu);
  }

  /*
   * Extract MAC-I:
   * Always extract from SRBs, only extract from DRBs if integrity is enabled
   */
  sec_mac mac = {};
  if (is_srb() || (is_drb() && (integrity_enabled == pdcp_integrity_enabled::enabled))) {
    extract_mac(sdu, mac);
  }

  /*
   * TS 38.323, section 5.9: Integrity verification
   *
   * The data unit that is integrity protected is the PDU header
   * and the data part of the PDU before ciphering.
   */
  if (integrity_enabled == pdcp_integrity_enabled::enabled) {
    bool is_valid = integrity_verify(sdu, rcvd_count, mac);
    if (!is_valid) {
      logger.log_error(sdu.begin(), sdu.end(), "Integrity failed. Dropping PDU");
      // upper_cn->notify_pdcp_integrity_error(lcid); // FIXME
      return; // Invalid packet, drop.
    }
    logger.log_debug(sdu.begin(), sdu.end(), "Integrity verification successful");
  }
  // After checking the integrity, we can discard the header.
  discard_data_header(sdu);

  /*
   * Check valid rcvd_count:
   *
   * - if RCVD_COUNT < RX_DELIV; or
   * - if the PDCP Data PDU with COUNT = RCVD_COUNT has been received before:
   *   - discard the PDCP Data PDU;
   */
  if (rcvd_count < st.rx_deliv) {
    logger.log_debug("Out-of-order after time-out, duplicate or COUNT wrap-around");
    logger.log_debug("RCVD_COUNT {}, RCVD_COUNT {}", rcvd_count, st.rx_deliv);
    return; // Invalid count, drop.
  }

  // Check if PDU has been received
  if (reorder_queue.find(rcvd_count) != reorder_queue.end()) {
    logger.log_debug("Duplicate PDU, dropping");
    return; // PDU already present, drop.
  }

  // Store PDU in reception buffer
  reorder_queue[rcvd_count] = std::move(sdu);

  // Update RX_NEXT
  if (rcvd_count >= st.rx_next) {
    st.rx_next = rcvd_count + 1;
  }

  // TODO if out-of-order configured, submit to upper layer

  if (rcvd_count == st.rx_deliv) {
    // Deliver to upper layers in ascending order of associated COUNT
    deliver_all_consecutive_counts();
  }

  // Handle reordering timers
  if (reordering_timer.is_running() and st.rx_deliv >= st.rx_reord) {
    reordering_timer.stop();
    logger.log_debug("Stopped t-Reordering - RX_DELIV={}, RX_REORD={}", st.rx_deliv, st.rx_reord);
  }

  if (cfg.t_reordering != pdcp_t_reordering::infinity) {
    if (not reordering_timer.is_running() and st.rx_deliv < st.rx_next) {
      st.rx_reord = st.rx_next;
      reordering_timer.run();
      logger.log_debug(
          "Started t-Reordering - RX_REORD={}, RX_DELIV={}, RX_NEXT={}", st.rx_reord, st.rx_deliv, st.rx_next);
    }
  }

  logger.log_debug("Rx PDCP state - RX_NEXT={}, RX_DELIV={}, RX_REORD={}", st.rx_next, st.rx_deliv, st.rx_reord);
}

// Deliver all consecutively associated COUNTs.
// Update RX_NEXT after submitting to higher layers
void pdcp_entity_rx::deliver_all_consecutive_counts()
{
  for (std::map<uint32_t, byte_buffer>::iterator it = reorder_queue.begin();
       it != reorder_queue.end() && it->first == st.rx_deliv;
       reorder_queue.erase(it++)) {
    logger.log_debug("Delivering SDU with RCVD_COUNT {}", it->first);

    // Pass PDCP SDU to the next layers
    upper_dn.on_new_sdu(std::move(it->second));

    // Update RX_DELIV
    st.rx_deliv = st.rx_deliv + 1;
  }
}
/*
 * Security helpers
 */
bool pdcp_entity_rx::integrity_verify(byte_buffer_view buf, uint32_t count, const sec_mac& mac)
{
  // If control plane use RRC integrity key. If data use user plane key
  const sec_128_as_key& k_int = is_srb() ? sec_cfg.k_128_rrc_int : sec_cfg.k_128_up_int;

  sec_mac mac_exp  = {};
  bool    is_valid = true;
  switch (sec_cfg.integ_algo) {
    case integrity_algorithm::nia0:
      break;
    case integrity_algorithm::nia1:
      security_nia1(mac_exp, k_int, count, lcid - 1, direction, buf);
      break;
    case integrity_algorithm::nia2:
      security_nia2(mac_exp, k_int, count, lcid - 1, direction, buf);
      break;
    case integrity_algorithm::nia3:
      security_nia3(mac_exp, k_int, count, lcid - 1, direction, buf);
      break;
    default:
      break;
  }

  if (sec_cfg.integ_algo != integrity_algorithm::nia0) {
    for (uint8_t i = 0; i < 4; i++) {
      if (mac[i] != mac_exp[i]) {
        is_valid = false;
        break;
      }
    }
    srslog::basic_levels level = is_valid ? srslog::basic_levels::debug : srslog::basic_levels::error;
    logger.log(level,
               buf.begin(),
               buf.end(),
               "Integrity check input - COUNT: {}, Bearer ID: {}, Direction: {}",
               count,
               lcid,
               direction);
    logger.log(level, (uint8_t*)k_int.data(), 16, "Integrity check key:");
    logger.log(level, (uint8_t*)mac_exp.data(), 4, "MAC {} (expected):", is_valid ? "match" : "mismatch");
    logger.log(level, (uint8_t*)mac.data(), 4, "MAC {} (found):", is_valid ? "match" : "mismatch");
    logger.log(level, buf.begin(), buf.end(), "Integrity check input msg (Bytes={})", buf.length());
  }

  return is_valid;
}

byte_buffer pdcp_entity_rx::cipher_decrypt(byte_buffer_view msg, uint32_t count)
{
  // If control plane use RRC integrity key. If data use user plane key
  const sec_128_as_key& k_enc = is_srb() ? sec_cfg.k_128_rrc_enc : sec_cfg.k_128_up_enc;

  logger.log_debug("Cipher decrypt input: COUNT: {}, Bearer ID: {}, Direction: {}", count, lcid, direction);
  logger.log_debug((uint8_t*)k_enc.data(), k_enc.size(), "Cipher decrypt key:");
  logger.log_debug(msg.begin(), msg.end(), "Cipher decrypt input msg");

  byte_buffer ct;

  switch (sec_cfg.cipher_algo) {
    case ciphering_algorithm::nea0:
      break;
    case ciphering_algorithm::nea1:
      ct = security_nea1(k_enc, count, lcid - 1, direction, msg);
      break;
    case ciphering_algorithm::nea2:
      ct = security_nea2(k_enc, count, lcid - 1, direction, msg);
      break;
    case ciphering_algorithm::nea3:
      ct = security_nea3(k_enc, count, lcid - 1, direction, msg);
      break;
    default:
      break;
  }
  logger.log_debug(ct.begin(), ct.end(), "Cipher decrypt output msg");
  return ct;
}

/*
 * Header helpers
 */
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

void pdcp_entity_rx::discard_data_header(byte_buffer& buf) const
{
  buf.trim_head(hdr_len_bytes);
}

void pdcp_entity_rx::extract_mac(byte_buffer& buf, sec_mac& mac) const
{
  if (buf.length() <= sec_mac_len) {
    logger.log_error("PDU too small to extract MAC-I");
    return;
  }
  for (unsigned i = 0; i < sec_mac_len; i++) {
    mac[i] = buf[buf.length() - sec_mac_len + i];
  }
  buf.trim_tail(sec_mac_len);
}
