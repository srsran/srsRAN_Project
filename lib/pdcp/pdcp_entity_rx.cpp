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
#include "srsgnb/security/ciphering.h"
#include "srsgnb/security/integrity.h"
#include "srsgnb/support/bit_encoding.h"

using namespace srsgnb;

pdcp_entity_rx::pdcp_entity_rx(uint32_t                        ue_index,
                               lcid_t                          lcid_,
                               pdcp_config::pdcp_rx_config     cfg_,
                               pdcp_rx_upper_data_notifier&    upper_dn_,
                               pdcp_rx_upper_control_notifier& upper_cn_,
                               timer_manager&                  timers_) :
  pdcp_entity_tx_rx_base(lcid_, cfg_.rb_type, cfg_.sn_size),
  logger("PDCP", ue_index, lcid_),
  cfg(cfg_),
  upper_dn(upper_dn_),
  upper_cn(upper_cn_),
  timers(timers_)
{
  // Security direction
  direction = cfg.direction == pdcp_security_direction::uplink ? security::security_direction::uplink
                                                               : security::security_direction::downlink;

  // t-Reordering timer
  if (cfg.t_reordering != pdcp_t_reordering::infinity) {
    reordering_timer = timers.create_unique_timer();
    if (static_cast<uint32_t>(cfg.t_reordering) > 0) {
      reordering_timer.set(static_cast<uint32_t>(cfg.t_reordering), reordering_callback{this});
    }
  } else if (cfg.rlc_mode == pdcp_rlc_mode::um) {
    logger.log_error("{} possible PDCP-NR misconfiguration: using infinite re-ordering timer with RLC UM bearer.");
  }
  logger.log_info("Configured PDCP RX entity. Configuration: {}", cfg);
}

void pdcp_entity_rx::handle_pdu(byte_buffer_slice_chain pdu)
{
  metrics_add_pdus(1, pdu.length());

  // Log PDU
  logger.log_info(pdu.begin(),
                  pdu.end(),
                  "RX PDU ({} B), integrity={}, ciphering={}",
                  pdu.length(),
                  integrity_enabled,
                  ciphering_enabled);
  // Sanity check
  if (pdu.length() == 0) {
    metrics_add_dropped_pdus(1);
    logger.log_error("PDCP PDU is empty.");
    return;
  }

  if (is_srb() || pdcp_pdu_get_dc(*(pdu.begin())) == pdcp_dc_field::data) {
    handle_data_pdu(std::move(pdu));
  } else {
    handle_control_pdu(std::move(pdu));
  }
}

void pdcp_entity_rx::handle_data_pdu(byte_buffer_slice_chain pdu)
{
  // Sanity check
  if (pdu.length() <= hdr_len_bytes) {
    metrics_add_dropped_pdus(1);
    logger.log_error("PDCP PDU is too small. PDU length={}, header length={}", pdu.length(), hdr_len_bytes);
    return;
  }
  logger.log_debug("Rx PDCP state - RX_NEXT={}, RX_DELIV={}, RX_REORD={}", st.rx_next, st.rx_deliv, st.rx_reord);

  // Unpack header
  pdcp_data_pdu_header hdr = {};
  if (not read_data_pdu_header(hdr, pdu)) {
    metrics_add_dropped_pdus(1);
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
  if ((int64_t)hdr.sn < (int64_t)SN(st.rx_deliv) - (int64_t)window_size) {
    rcvd_hfn = HFN(st.rx_deliv) + 1;
  } else if (hdr.sn >= SN(st.rx_deliv) + window_size) {
    rcvd_hfn = HFN(st.rx_deliv) - 1;
  } else {
    rcvd_hfn = HFN(st.rx_deliv);
  }
  rcvd_count = COUNT(rcvd_hfn, hdr.sn);

  // The PDCP is not allowed to use the same COUNT value more than once for a given security key,
  // see TS 38.331, section 5.3.1.2. To avoid this, we notify the RRC once we exceed a "maximum"
  // notification COUNT. It is then the RRC's responsibility to refresh the keys. We continue receiving until
  // we reach a hard maximum RCVD_COUNT, after which we refuse to receive any further.
  if (rcvd_count > cfg.max_count.notify) {
    if (!max_count_notified) {
      logger.log_warning("Approaching COUNT wrap-around, notifying RRC. COUNT={}", rcvd_count);
      upper_cn.on_max_count_reached();
      max_count_notified = true;
    }
  }
  if (rcvd_count >= cfg.max_count.hard && !max_count_overflow) {
    if (!max_count_overflow) {
      logger.log_error("Reached maximum COUNT, refusing to receive further. COUNT={}", rcvd_count);
      upper_cn.on_protocol_failure();
      max_count_overflow = true;
    }
    return;
  }

  /*
   * TS 38.323, section 5.8: Deciphering
   *
   * The data unit that is ciphered is the MAC-I and the
   * data part of the PDCP Data PDU except the
   * SDAP header and the SDAP Control PDU if included in the PDCP SDU.
   */
  byte_buffer sdu;
  if (ciphering_enabled == pdcp_ciphering_enabled::enabled) {
    sdu = cipher_decrypt(pdu.begin() + hdr_len_bytes, pdu.end(), rcvd_count);
    std::array<uint8_t, pdcp_data_pdu_header_size_max> header_buf;
    std::copy(pdu.begin(), pdu.begin() + hdr_len_bytes, header_buf.begin());
    sdu.prepend(span<uint8_t>{header_buf.data(), hdr_len_bytes});
  } else {
    // TODO: Optimize - avoid bytewise copy
    sdu = {pdu.begin(), pdu.end()};
  }

  /*
   * Extract MAC-I:
   * Always extract from SRBs, only extract from DRBs if integrity is enabled
   */
  security::sec_mac mac = {};
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
      logger.log_warning(sdu.begin(), sdu.end(), "Integrity failed. Dropping PDU");
      metrics_add_integrity_failed_pdus(1);
      upper_cn.on_integrity_failure();
      return; // Invalid packet, drop.
    }
    metrics_add_integrity_verified_pdus(1);
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
  // /!\ Caution: reorder_queue is used to build status report:
  //     For out-of-order:
  //     - store empty buffers there
  //     - clean upon each rx'ed PDU
  //     - don't forward empty buffer to upper layers

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
    st.rx_reord = st.rx_next;
    if (cfg.t_reordering == pdcp_t_reordering::ms0) {
      handle_t_reordering_expire();
    } else if (not reordering_timer.is_running() and st.rx_deliv < st.rx_next) {
      reordering_timer.run();
      logger.log_debug(
          "Started t-Reordering - RX_REORD={}, RX_DELIV={}, RX_NEXT={}", st.rx_reord, st.rx_deliv, st.rx_next);
    }
  }

  logger.log_debug("Rx PDCP state - RX_NEXT={}, RX_DELIV={}, RX_REORD={}", st.rx_next, st.rx_deliv, st.rx_reord);
}

void pdcp_entity_rx::handle_control_pdu(byte_buffer_slice_chain pdu)
{
  // Read and verify PDU header (first byte)
  uint8_t hdr_byte = *pdu.begin();

  // Assert control PDU
  pdcp_dc_field dc = pdcp_pdu_get_dc(hdr_byte);
  srsgnb_assert(dc == pdcp_dc_field::control,
                "Cannot handle status report due to invalid D/C field: Expected {}, Got {}",
                pdcp_dc_field::control,
                dc);

  // Switch control PDU type (CPT)
  pdcp_control_pdu_header control_hdr = {};
  control_hdr.cpt                     = pdcp_control_pdu_get_cpt(hdr_byte);
  switch (control_hdr.cpt) {
    case pdcp_control_pdu_type::status_report:
      status_handler->on_status_report(std::move(pdu));
      break;
    default:
      logger.log_error(pdu.begin(), pdu.end(), "Received unsupported control PDU: {}", control_hdr);
  }
}

// Deliver all consecutively associated COUNTs.
// Update RX_NEXT after submitting to higher layers
void pdcp_entity_rx::deliver_all_consecutive_counts()
{
  for (std::map<uint32_t, byte_buffer>::iterator it = reorder_queue.begin();
       it != reorder_queue.end() && it->first == st.rx_deliv;
       reorder_queue.erase(it++)) {
    logger.log_debug("Delivering SDU with RCVD_COUNT {}", it->first);

    // Pass PDCP SDU to the upper layers
    metrics_add_sdus(1, it->second.length());
    upper_dn.on_new_sdu(std::move(it->second));

    // Update RX_DELIV
    st.rx_deliv = st.rx_deliv + 1;
  }
}

byte_buffer pdcp_entity_rx::compile_status_report()
{
  byte_buffer buf = {};
  bit_encoder enc(buf);

  // Pack PDU header
  enc.pack(to_number(pdcp_dc_field::control), 1);
  enc.pack(to_number(pdcp_control_pdu_type::status_report), 3);
  enc.pack(0b0000, 4);

  // Pack RX_DELIV into FMC field
  enc.pack(st.rx_deliv, 32);

  // Set bitmap boundaries, ensure to not exceed max control PDU size (9000 Bytes)
  constexpr uint32_t max_bits     = (pdcp_control_pdu_max_size - 5) * 8;
  uint32_t           bitmap_begin = st.rx_deliv + 1; // Bitmap starts from FMC+1
  uint32_t           bitmap_end   = st.rx_next;
  if (bitmap_begin < bitmap_end && bitmap_end - bitmap_begin > max_bits) {
    bitmap_end = bitmap_begin + max_bits;
  }

  // Pack bitmap
  for (uint32_t i = bitmap_begin; i < bitmap_end; i++) {
    // Bit == 0: PDCP SDU with COUNT = (FMC + bit position) modulo 2^32 is missing.
    // Bit == 1: PDCP SDU with COUNT = (FMC + bit position) modulo 2^32 is correctly received.
    unsigned bit = reorder_queue.find(i) != reorder_queue.end() ? 0 : 1;
    enc.pack(bit, 1);
  }

  return buf;
}

/*
 * Security helpers
 */
bool pdcp_entity_rx::integrity_verify(byte_buffer_view buf, uint32_t count, const security::sec_mac& mac)
{
  // If control plane use RRC integrity key. If data use user plane key
  const security::sec_128_as_key& k_int = is_srb() ? sec_cfg.k_128_rrc_int : sec_cfg.k_128_up_int;

  security::sec_mac mac_exp  = {};
  bool              is_valid = true;
  switch (sec_cfg.integ_algo) {
    case security::integrity_algorithm::nia0:
      break;
    case security::integrity_algorithm::nia1:
      security_nia1(mac_exp, k_int, count, lcid - 1, direction, buf.begin(), buf.end());
      break;
    case security::integrity_algorithm::nia2:
      security_nia2(mac_exp, k_int, count, lcid - 1, direction, buf.begin(), buf.end());
      break;
    case security::integrity_algorithm::nia3:
      security_nia3(mac_exp, k_int, count, lcid - 1, direction, buf.begin(), buf.end());
      break;
    default:
      break;
  }

  if (sec_cfg.integ_algo != security::integrity_algorithm::nia0) {
    for (uint8_t i = 0; i < 4; i++) {
      if (mac[i] != mac_exp[i]) {
        is_valid = false;
        break;
      }
    }
    srslog::basic_levels level = is_valid ? srslog::basic_levels::debug : srslog::basic_levels::warning;
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

byte_buffer pdcp_entity_rx::cipher_decrypt(byte_buffer_slice_chain::const_iterator msg_begin,
                                           byte_buffer_slice_chain::const_iterator msg_end,
                                           uint32_t                                count)
{
  // If control plane use RRC integrity key. If data use user plane key
  const security::sec_128_as_key& k_enc = is_srb() ? sec_cfg.k_128_rrc_enc : sec_cfg.k_128_up_enc;

  logger.log_debug("Cipher decrypt input: COUNT: {}, Bearer ID: {}, Direction: {}", count, lcid, direction);
  logger.log_debug((uint8_t*)k_enc.data(), k_enc.size(), "Cipher decrypt key:");
  logger.log_debug(msg_begin, msg_end, "Cipher decrypt input msg");

  byte_buffer ct;

  switch (sec_cfg.cipher_algo) {
    case security::ciphering_algorithm::nea0:
      ct.append(msg_begin, msg_end);
      break;
    case security::ciphering_algorithm::nea1:
      ct = security_nea1(k_enc, count, lcid - 1, direction, msg_begin, msg_end);
      break;
    case security::ciphering_algorithm::nea2:
      ct = security_nea2(k_enc, count, lcid - 1, direction, msg_begin, msg_end);
      break;
    case security::ciphering_algorithm::nea3:
      ct = security_nea3(k_enc, count, lcid - 1, direction, msg_begin, msg_end);
      break;
    default:
      break;
  }
  logger.log_debug(ct.begin(), ct.end(), "Cipher decrypt output msg");
  return ct;
}

/*
 * Timers
 */
void pdcp_entity_rx::handle_t_reordering_expire()
{
  metrics_add_t_reordering_timeouts(1);
  // Deliver all PDCP SDU(s) with associated COUNT value(s) < RX_REORD
  for (std::map<uint32_t, byte_buffer>::iterator it = reorder_queue.begin();
       it != reorder_queue.end() && it->first < st.rx_reord;
       reorder_queue.erase(it++)) {
    // Deliver PDCP SDU to the upper layers
    upper_dn.on_new_sdu(std::move(it->second));
  }

  // Update RX_DELIV to the first PDCP SDU not delivered to the upper layers
  st.rx_deliv = st.rx_reord;

  // Deliver all PDCP SDU(s) consecutively associated COUNT value(s) starting from RX_REORD
  deliver_all_consecutive_counts();

  if (st.rx_deliv < st.rx_next) {
    if (cfg.t_reordering == pdcp_t_reordering::ms0) {
      logger.log_error(
          "RX_DELIV={} < RX_NEXT={}, but t-Reordering is 0ms. RX_REORD={}", st.rx_deliv, st.rx_next, st.rx_reord);
      return;
    }
    logger.log_debug("Updating RX_REORD to {}. Old RX_REORD={}, RX_DELIV={}", st.rx_next, st.rx_reord, st.rx_deliv);
    st.rx_reord = st.rx_next;
    reordering_timer.run();
  }
}

// Reordering Timer Callback (t-reordering)
void pdcp_entity_rx::reordering_callback::operator()(uint32_t /*timer_id*/)
{
  parent->logger.log_info("Reordering timer expired. RX_REORD={}, re-order queue size={}",
                          parent->st.rx_reord,
                          parent->reorder_queue.size());
  parent->handle_t_reordering_expire();
}

/*
 * Header helpers
 */
bool pdcp_entity_rx::read_data_pdu_header(pdcp_data_pdu_header& hdr, const byte_buffer_slice_chain& buf) const
{
  // Check PDU is long enough to extract header
  if (buf.length() <= hdr_len_bytes) {
    logger.log_error("PDU too small to extract header");
    return false;
  }

  byte_buffer_slice_chain::const_iterator buf_it = buf.begin();

  // Extract RCVD_SN
  switch (cfg.sn_size) {
    case pdcp_sn_size::size12bits:
      hdr.sn = (*buf_it & 0x0fU) << 8U; // first 4 bits SN
      ++buf_it;
      hdr.sn |= (*buf_it & 0xffU); // last 8 bits SN
      ++buf_it;
      break;
    case pdcp_sn_size::size18bits:
      hdr.sn = (*buf_it & 0x03U) << 16U; // first 2 bits SN
      ++buf_it;
      hdr.sn |= (*buf_it & 0xffU) << 8U; // middle 8 bits SN
      ++buf_it;
      hdr.sn |= (*buf_it & 0xffU); // last 8 bits SN
      ++buf_it;
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

void pdcp_entity_rx::extract_mac(byte_buffer& buf, security::sec_mac& mac) const
{
  if (buf.length() <= security::sec_mac_len) {
    logger.log_error("PDU too small to extract MAC-I");
    return;
  }
  for (unsigned i = 0; i < security::sec_mac_len; i++) {
    mac[i] = buf[buf.length() - security::sec_mac_len + i];
  }
  buf.trim_tail(security::sec_mac_len);
}
