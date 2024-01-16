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

#include "pdcp_entity_rx.h"
#include "../support/sdu_window_impl.h"
#include "srsran/instrumentation/traces/up_traces.h"
#include "srsran/security/ciphering.h"
#include "srsran/security/integrity.h"
#include "srsran/support/bit_encoding.h"

using namespace srsran;

pdcp_entity_rx::pdcp_entity_rx(uint32_t                        ue_index,
                               rb_id_t                         rb_id_,
                               pdcp_rx_config                  cfg_,
                               pdcp_rx_upper_data_notifier&    upper_dn_,
                               pdcp_rx_upper_control_notifier& upper_cn_,
                               timer_factory                   timers_) :
  pdcp_entity_tx_rx_base(rb_id_, cfg_.rb_type, cfg_.rlc_mode, cfg_.sn_size),
  logger("PDCP", {ue_index, rb_id_, "UL"}),
  cfg(cfg_),
  direction(cfg.direction == pdcp_security_direction::uplink ? security::security_direction::uplink
                                                             : security::security_direction::downlink),
  rx_window(create_rx_window(cfg.sn_size)),
  upper_dn(upper_dn_),
  upper_cn(upper_cn_),
  timers(timers_)
{
  // t-Reordering timer
  if (cfg.t_reordering != pdcp_t_reordering::ms0 && cfg.t_reordering != pdcp_t_reordering::infinity) {
    reordering_timer = timers.create_timer();
    if (static_cast<uint32_t>(cfg.t_reordering) > 0) {
      reordering_timer.set(std::chrono::milliseconds{static_cast<unsigned>(cfg.t_reordering)},
                           reordering_callback{this});
    }
  }
  if (cfg.rb_type == pdcp_rb_type::drb && cfg.t_reordering == pdcp_t_reordering::infinity) {
    logger.log_warning("t-Reordering of infinity on DRBs is not advised. It can cause data stalls.");
  }
  logger.log_info("PDCP configured. {}", cfg);
}

void pdcp_entity_rx::handle_pdu(byte_buffer_chain buf)
{
  trace_point rx_tp = up_tracer.now();
  metrics_add_pdus(1, buf.length());

  // Log PDU
  logger.log_debug(buf.begin(), buf.end(), "RX PDU. pdu_len={}", buf.length());
  // Sanity check
  if (buf.length() == 0) {
    metrics_add_dropped_pdus(1);
    logger.log_error("Dropping empty PDU.");
    return;
  }

  byte_buffer   pdu = buf.deep_copy();
  pdcp_dc_field dc  = pdcp_pdu_get_dc(*(pdu.begin()));
  if (is_srb() || dc == pdcp_dc_field::data) {
    handle_data_pdu(std::move(pdu));
  } else {
    handle_control_pdu(std::move(buf));
  }
  up_tracer << trace_event{"pdcp_rx_pdu", rx_tp};
}

void pdcp_entity_rx::reestablish(security::sec_128_as_config sec_cfg_)
{
  // - process the PDCP Data PDUs that are received from lower layers due to the re-establishment of the lower layers,
  //   as specified in the clause 5.2.2.1;

  // - for SRBs, discard all stored PDCP SDUs and PDCP PDUs;
  if (is_srb()) {
    discard_all_sdus();
  }

  // - for SRBs and UM DRBs, if t-Reordering is running:
  //   - stop and reset t-Reordering;
  //   - for UM DRBs, deliver all stored PDCP SDUs to the upper layers in ascending order of associated COUNT
  //     values after performing header decompression;
  if (is_srb() || is_um()) {
    if (reordering_timer.is_running()) {
      reordering_timer.stop();
    }
    if (is_um()) {
      deliver_all_sdus();
    }
  }

  // - for AM DRBs for Uu interface, perform header decompression using ROHC for all stored PDCP SDUs if drb-
  //   ContinueROHC is not configured in TS 38.331 [3];
  // - for AM DRBs for PC5 interface, perform header decompression using ROHC for all stored PDCP IP SDUs;
  // - for AM DRBs for Uu interface, perform header decompression using EHC for all stored PDCP SDUs if drb-
  //   ContinueEHC-DL is not configured in TS 38.331 [3];
  // - for UM DRBs and AM DRBs, reset the ROHC protocol for downlink and start with NC state in U-mode (as
  //   defined in RFC 3095 [8] and RFC 4815 [9]) if drb-ContinueROHC is not configured in TS 38.331 [3];
  // - for UM DRBs and AM DRBs, reset the EHC protocol for downlink if drb-ContinueEHC-DL is not configured in
  //   TS 38.331 [3];
  // TODO header compression not supported yet.

  // - for UM DRBs and SRBs, set RX_NEXT and RX_DELIV to the initial value;
  if (is_srb() || is_um()) {
    st = {};
  }

  // - apply the ciphering algorithm and key provided by upper layers during the PDCP entity re-establishment
  //   procedure;
  // - apply the integrity protection algorithm and key provided by upper layers during the PDCP entity re-
  //   establishment procedure.
  configure_security(sec_cfg_);
}

void pdcp_entity_rx::handle_data_pdu(byte_buffer pdu)
{
  // Sanity check
  if (pdu.length() <= hdr_len_bytes) {
    metrics_add_dropped_pdus(1);
    logger.log_error(pdu.begin(), pdu.end(), "RX PDU too small. pdu_len={} hdr_len={}", pdu.length(), hdr_len_bytes);
    return;
  }
  // Log state
  log_state(srslog::basic_levels::debug);

  // Unpack header
  pdcp_data_pdu_header hdr = {};
  if (not read_data_pdu_header(hdr, pdu)) {
    metrics_add_dropped_pdus(1);
    logger.log_error(
        pdu.begin(), pdu.end(), "Failed to extract SN. pdu_len={} hdr_len={}", pdu.length(), hdr_len_bytes);
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
  uint32_t rcvd_hfn;
  uint32_t rcvd_count;
  if ((int64_t)hdr.sn < (int64_t)SN(st.rx_deliv) - (int64_t)window_size) {
    rcvd_hfn = HFN(st.rx_deliv) + 1;
  } else if (hdr.sn >= SN(st.rx_deliv) + window_size) {
    rcvd_hfn = HFN(st.rx_deliv) - 1;
  } else {
    rcvd_hfn = HFN(st.rx_deliv);
  }
  rcvd_count = COUNT(rcvd_hfn, hdr.sn);

  logger.log_info(
      pdu.begin(), pdu.end(), "RX PDU. type=data pdu_len={} sn={} count={}", pdu.length(), hdr.sn, rcvd_count);

  // The PDCP is not allowed to use the same COUNT value more than once for a given security key,
  // see TS 38.331, section 5.3.1.2. To avoid this, we notify the RRC once we exceed a "maximum"
  // notification COUNT. It is then the RRC's responsibility to refresh the keys. We continue receiving until
  // we reach a hard maximum RCVD_COUNT, after which we refuse to receive any further.
  if (rcvd_count > cfg.custom.max_count.notify) {
    if (!max_count_notified) {
      logger.log_warning("Approaching max_count, notifying RRC. count={}", rcvd_count);
      upper_cn.on_max_count_reached();
      max_count_notified = true;
    }
  }
  if (rcvd_count >= cfg.custom.max_count.hard) {
    if (!max_count_overflow) {
      logger.log_error("Reached max_count, refusing further RX. count={}", rcvd_count);
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
  byte_buffer_view sdu_plus_mac = byte_buffer_view{pdu.begin() + hdr_len_bytes, pdu.end()};
  if (ciphering_enabled == security::ciphering_enabled::on &&
      sec_cfg.cipher_algo != security::ciphering_algorithm::nea0) {
    cipher_decrypt(sdu_plus_mac, rcvd_count);
  }

  /*
   * Extract MAC-I:
   * Always extract from SRBs, only extract from DRBs if integrity is enabled
   */
  security::sec_mac mac = {};
  if (is_srb() || (is_drb() && (integrity_enabled == security::integrity_enabled::on))) {
    extract_mac(pdu, mac);
  }

  /*
   * TS 38.323, section 5.9: Integrity verification
   *
   * The data unit that is integrity protected is the PDU header
   * and the data part of the PDU before ciphering.
   */
  if (integrity_enabled == security::integrity_enabled::on) {
    bool is_valid = integrity_verify(pdu, rcvd_count, mac);
    if (!is_valid) {
      logger.log_warning(pdu.begin(), pdu.end(), "Integrity failed, dropping PDU.");
      metrics_add_integrity_failed_pdus(1);
      // TODO: Re-enable once the RRC supports notifications from the PDCP
      // upper_cn.on_integrity_failure();
      return; // Invalid packet, drop.
    }
    metrics_add_integrity_verified_pdus(1);
    logger.log_debug(pdu.begin(), pdu.end(), "Integrity passed.");
  }
  // After checking the integrity, we can discard the header.
  discard_data_header(pdu);

  /*
   * Check valid rcvd_count:
   *
   * - if RCVD_COUNT < RX_DELIV; or
   * - if the PDCP Data PDU with COUNT = RCVD_COUNT has been received before:
   *   - discard the PDCP Data PDU;
   */
  if (rcvd_count < st.rx_deliv) {
    logger.log_debug("Out-of-order after timeout, duplicate or count wrap-around. count={} {}", rcvd_count, st);
    return; // Invalid count, drop.
  }

  // Check if PDU has been received
  if (rx_window->has_sn(rcvd_count)) {
    const pdcp_rx_sdu_info& sdu_info = (*rx_window)[rcvd_count];
    if (sdu_info.count == rcvd_count) {
      logger.log_debug("Duplicate PDU dropped. count={}", rcvd_count);
      return; // PDU already present, drop.
    } else {
      logger.log_error("Removing old PDU with count={} for new PDU with count={}", sdu_info.count, rcvd_count);
      rx_window->remove_sn(rcvd_count);
    }
  }

  // Store PDU in Rx window
  pdcp_rx_sdu_info& sdu_info = rx_window->add_sn(rcvd_count);
  sdu_info.sdu               = std::move(pdu);
  sdu_info.count             = rcvd_count;

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
    logger.log_debug("Stopped t-Reordering.", st);
  }

  if (cfg.t_reordering != pdcp_t_reordering::infinity) {
    if (cfg.t_reordering == pdcp_t_reordering::ms0) {
      st.rx_reord = st.rx_next;
      handle_t_reordering_expire();
    } else if (not reordering_timer.is_running() and st.rx_deliv < st.rx_next) {
      st.rx_reord = st.rx_next;
      reordering_timer.run();
      logger.log_debug("Started t-Reordering.");
    }
  }

  // Log state
  log_state(srslog::basic_levels::debug);
}

void pdcp_entity_rx::handle_control_pdu(byte_buffer_chain pdu)
{
  // Read and verify PDU header (first byte)
  uint8_t hdr_byte = *pdu.begin();

  // Assert control PDU
  pdcp_dc_field dc = pdcp_pdu_get_dc(hdr_byte);
  srsran_assert(dc == pdcp_dc_field::control, "Invalid D/C field in control PDU. dc={}", dc);

  // Switch control PDU type (CPT)
  pdcp_control_pdu_header control_hdr = {};
  control_hdr.cpt                     = pdcp_control_pdu_get_cpt(hdr_byte);
  switch (control_hdr.cpt) {
    case pdcp_control_pdu_type::status_report:
      status_handler->on_status_report(std::move(pdu));
      break;
    default:
      logger.log_error(pdu.begin(), pdu.end(), "Unsupported control PDU type. {}", control_hdr);
  }
}

// Deliver all consecutively associated COUNTs.
// Update RX_DELIV after submitting to higher layers
void pdcp_entity_rx::deliver_all_consecutive_counts()
{
  while (st.rx_deliv != st.rx_next && rx_window->has_sn(st.rx_deliv)) {
    pdcp_rx_sdu_info& sdu_info = (*rx_window)[st.rx_deliv];
    logger.log_info("RX SDU. count={}", st.rx_deliv);

    // Pass PDCP SDU to the upper layers
    metrics_add_sdus(1, sdu_info.sdu.length());
    upper_dn.on_new_sdu(std::move(sdu_info.sdu));
    rx_window->remove_sn(st.rx_deliv);

    // Update RX_DELIV
    st.rx_deliv = st.rx_deliv + 1;
  }
}

// Deliver all RX'ed SDUs, regardless of order. Used during re-establishment.
// No need to update RX_DELIV, as the re-establishment procedure will be responsible
// for updating the state.
void pdcp_entity_rx::deliver_all_sdus()
{
  for (uint32_t count = st.rx_deliv; count < st.rx_next; count++) {
    if (rx_window->has_sn(count)) {
      pdcp_rx_sdu_info& sdu_info = (*rx_window)[count];
      logger.log_info("RX SDU. count={}", count);

      // Pass PDCP SDU to the upper layers
      metrics_add_sdus(1, sdu_info.sdu.length());
      upper_dn.on_new_sdu(std::move(sdu_info.sdu));
      rx_window->remove_sn(count);
    }
  }
}

// Discard all SDUs.
void pdcp_entity_rx::discard_all_sdus()
{
  while (st.rx_deliv != st.rx_next) {
    if (rx_window->has_sn(st.rx_deliv)) {
      rx_window->remove_sn(st.rx_deliv);
      logger.log_debug("Discarded RX SDU. count={}", st.rx_next);
    }

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
    unsigned bit = rx_window->has_sn(i) ? 1 : 0;
    enc.pack(bit, 1);
  }

  return buf;
}

std::unique_ptr<sdu_window<pdcp_rx_sdu_info>> pdcp_entity_rx::create_rx_window(pdcp_sn_size sn_size_)
{
  std::unique_ptr<sdu_window<pdcp_rx_sdu_info>> rx_window_;
  switch (sn_size_) {
    case pdcp_sn_size::size12bits:
      rx_window_ = std::make_unique<sdu_window_impl<pdcp_rx_sdu_info,
                                                    pdcp_window_size(pdcp_sn_size_to_uint(pdcp_sn_size::size12bits)),
                                                    pdcp_bearer_logger>>(logger);
      break;
    case pdcp_sn_size::size18bits:
      rx_window_ = std::make_unique<sdu_window_impl<pdcp_rx_sdu_info,
                                                    pdcp_window_size(pdcp_sn_size_to_uint(pdcp_sn_size::size18bits)),
                                                    pdcp_bearer_logger>>(logger);
      break;
    default:
      srsran_assertion_failure("Cannot create rx_window for unsupported sn_size={}.", pdcp_sn_size_to_uint(sn_size_));
  }
  return rx_window_;
}

/*
 * Security helpers
 */
bool pdcp_entity_rx::integrity_verify(byte_buffer_view buf, uint32_t count, const security::sec_mac& mac)
{
  srsran_assert(sec_cfg.k_128_int.has_value(), "Cannot verify integrity: Integrity key is not configured.");
  srsran_assert(sec_cfg.integ_algo.has_value(), "Cannot verify integrity: Integrity algorithm is not configured.");
  security::sec_mac mac_exp  = {};
  bool              is_valid = true;
  switch (sec_cfg.integ_algo.value()) {
    case security::integrity_algorithm::nia0:
      break;
    case security::integrity_algorithm::nia1:
      security_nia1(mac_exp, sec_cfg.k_128_int.value(), count, bearer_id, direction, buf.begin(), buf.end());
      break;
    case security::integrity_algorithm::nia2:
      security_nia2(mac_exp, sec_cfg.k_128_int.value(), count, bearer_id, direction, buf.begin(), buf.end());
      break;
    case security::integrity_algorithm::nia3:
      security_nia3(mac_exp, sec_cfg.k_128_int.value(), count, bearer_id, direction, buf.begin(), buf.end());
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
               "Integrity check. is_valid={} count={} bearer_id={} dir={}",
               is_valid,
               count,
               bearer_id,
               direction);
    logger.log(
        level, (uint8_t*)sec_cfg.k_128_int.value().data(), sec_cfg.k_128_int.value().size(), "Integrity check key.");
    logger.log(level, (uint8_t*)mac_exp.data(), mac_exp.size(), "MAC expected.");
    logger.log(level, (uint8_t*)mac.data(), mac.size(), "MAC found.");
    logger.log(level, buf.begin(), buf.end(), "Integrity check input message. len={}", buf.length());
  }

  return is_valid;
}

byte_buffer pdcp_entity_rx::cipher_decrypt(byte_buffer_view& msg, uint32_t count)
{
  logger.log_debug("Cipher decrypt. count={} bearer_id={} dir={}", count, bearer_id, direction);
  logger.log_debug((uint8_t*)sec_cfg.k_128_enc.data(), sec_cfg.k_128_enc.size(), "Cipher decrypt key.");
  logger.log_debug(msg.begin(), msg.end(), "Cipher decrypt input msg.");

  byte_buffer ct;

  switch (sec_cfg.cipher_algo) {
    case security::ciphering_algorithm::nea1:
      security_nea1(sec_cfg.k_128_enc, count, bearer_id, direction, msg);
      break;
    case security::ciphering_algorithm::nea2:
      security_nea2(sec_cfg.k_128_enc, count, bearer_id, direction, msg);
      break;
    case security::ciphering_algorithm::nea3:
      security_nea3(sec_cfg.k_128_enc, count, bearer_id, direction, msg);
      break;
    default:
      break;
  }
  logger.log_debug(msg.begin(), msg.end(), "Cipher decrypt output msg.");
  return ct;
}

/*
 * Timers
 */
void pdcp_entity_rx::handle_t_reordering_expire()
{
  metrics_add_t_reordering_timeouts(1);
  // Deliver all PDCP SDU(s) with associated COUNT value(s) < RX_REORD
  while (st.rx_deliv != st.rx_reord) {
    if (rx_window->has_sn(st.rx_deliv)) {
      pdcp_rx_sdu_info& sdu_info = (*rx_window)[st.rx_deliv];
      logger.log_info("RX SDU. count={}", st.rx_deliv);

      // Pass PDCP SDU to the upper layers
      metrics_add_sdus(1, sdu_info.sdu.length());
      upper_dn.on_new_sdu(std::move(sdu_info.sdu));
      rx_window->remove_sn(st.rx_deliv);
    }

    // Update RX_DELIV
    st.rx_deliv = st.rx_deliv + 1;
  }

  // Deliver all PDCP SDU(s) consecutively associated COUNT value(s) starting from RX_REORD
  deliver_all_consecutive_counts();

  // Log state
  log_state(srslog::basic_levels::debug);

  if (st.rx_deliv < st.rx_next) {
    if (cfg.t_reordering == pdcp_t_reordering::ms0) {
      logger.log_error("Reordering timer expired after 0ms and rx_deliv < rx_next. {}", st);
      return;
    }
    logger.log_debug("Updating rx_reord to rx_next. {}", st);
    st.rx_reord = st.rx_next;
    reordering_timer.run();
  }
}

// Reordering Timer Callback (t-reordering)
void pdcp_entity_rx::reordering_callback::operator()(timer_id_t /*timer_id*/)
{
  parent->logger.log_info("Reordering timer expired. {}", parent->st);
  parent->handle_t_reordering_expire();
}

/*
 * Header helpers
 */
bool pdcp_entity_rx::read_data_pdu_header(pdcp_data_pdu_header& hdr, const byte_buffer& buf) const
{
  // Check PDU is long enough to extract header
  if (buf.length() <= hdr_len_bytes) {
    logger.log_error("PDU too small to extract header. pdu_len={} hdr_len={}", buf.length(), hdr_len_bytes);
    return false;
  }

  byte_buffer::const_iterator buf_it = buf.begin();

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
      logger.log_error("Invalid SN size config. sn_size={}", cfg.sn_size);
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
    logger.log_error("PDU too small to extract MAC-I. pdu_len={} mac_len={}", buf.length(), security::sec_mac_len);
    return;
  }
  for (unsigned i = 0; i < security::sec_mac_len; i++) {
    mac[i] = buf[buf.length() - security::sec_mac_len + i];
  }
  buf.trim_tail(security::sec_mac_len);
}
