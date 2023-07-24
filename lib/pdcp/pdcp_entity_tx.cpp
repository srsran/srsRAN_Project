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

#include "pdcp_entity_tx.h"
#include "srsran/security/ciphering.h"
#include "srsran/security/integrity.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

/// \brief Receive an SDU from the upper layers, apply encryption
/// and integrity protection and pass the resulting PDU
/// to the lower layers.
///
/// \param sdu Buffer that hold the SDU from higher layers.
/// \ref TS 38.323 section 5.2.1: Transmit operation
void pdcp_entity_tx::handle_sdu(byte_buffer sdu)
{
  metrics_add_sdus(1, sdu.length());
  logger.log_debug(sdu.begin(), sdu.end(), "TX SDU. sdu_len={}", sdu.length());

  // The PDCP is not allowed to use the same COUNT value more than once for a given security key,
  // see TS 38.331, section 5.3.1.2. To avoid this, we notify the RRC once we exceed a "maximum"
  // COUNT. It is then the RRC's responsibility to refresh the keys. We continue transmitting until
  // we reached a maximum hard COUNT, after which we simply refuse to TX any further.
  if (st.tx_next >= cfg.max_count.hard) {
    if (!max_count_overflow) {
      logger.log_error("Reached maximum count, refusing to transmit further. count={}", st.tx_next);
      upper_cn.on_protocol_failure();
      max_count_overflow = true;
    }
    return;
  }
  if (st.tx_next >= cfg.max_count.notify) {
    if (!max_count_notified) {
      logger.log_warning("Approaching count wrap-around, notifying RRC. count={}", st.tx_next);
      upper_cn.on_max_count_reached();
      max_count_notified = true;
    }
  }

  // Perform header compression
  // TODO

  // Prepare header
  pdcp_data_pdu_header hdr = {};
  hdr.sn                   = SN(st.tx_next);

  // Pack header
  byte_buffer header_buf = {};
  write_data_pdu_header(header_buf, hdr);

  // Apply ciphering and integrity protection
  byte_buffer protected_buf = apply_ciphering_and_integrity_protection(std::move(header_buf), sdu, st.tx_next);

  // Start discard timer. If using RLC AM, we store
  // the PDU to use later in the data recovery procedure.
  if (cfg.discard_timer != pdcp_discard_timer::infinity && cfg.discard_timer != pdcp_discard_timer::not_configured) {
    unique_timer discard_timer = timers.create_timer();
    discard_timer.set(std::chrono::milliseconds(static_cast<unsigned>(cfg.discard_timer)),
                      discard_callback{this, st.tx_next});
    discard_timer.run();
    discard_info info;
    if (cfg.rlc_mode == pdcp_rlc_mode::um) {
      info = {st.tx_next, {}, std::move(discard_timer)};
    } else {
      info = {st.tx_next, std::move(sdu), std::move(discard_timer)};
    }
    discard_timers_map.insert(std::make_pair(st.tx_next, std::move(info)));
    logger.log_debug("Set discard timer. count={} timeout={}", st.tx_next, static_cast<uint32_t>(cfg.discard_timer));
  }

  // Write to lower layers
  write_data_pdu_to_lower_layers(st.tx_next, std::move(protected_buf));

  // Increment TX_NEXT
  st.tx_next++;
}

void pdcp_entity_tx::reestablish(security::sec_128_as_config sec_cfg_)
{
  // - for UM DRBs and AM DRBs, reset the ROHC protocol for uplink and start with an IR state in U-mode (as
  //   defined in RFC 3095 [8] and RFC 4815 [9]) if drb-ContinueROHC is not configured in TS 38.331 [3];
  // - for UM DRBs and AM DRBs, reset the EHC protocol for uplink if drb-ContinueEHC-UL is not configured in
  //   TS 38.331 [3];
  //   Header compression not supported yet (TODO).

  // - for UM DRBs and SRBs, set TX_NEXT to the initial value;
  // - for SRBs, discard all stored PDCP SDUs and PDCP PDUs;
  if (is_srb() || is_um()) {
    st = {};
    discard_all_pdus(); // While not explicitly stated in the spec,
                        // there is no point in storing PDCP UM PDUs.
                        // They cannot be RETXed and RLC already discarded them.
                        // Also, this avoids having multiple discard timers
                        // associated with the with the same COUNT.
  }

  // - apply the ciphering algorithm and key provided by upper layers during the PDCP entity re-establishment
  //   procedure;
  // - apply the integrity protection algorithm and key provided by upper layers during the PDCP entity re-
  //   establishment procedure;
  enable_security(sec_cfg_);

  // - for UM DRBs, for each PDCP SDU already associated with a PDCP SN but for which a corresponding PDU has
  //   not previously been submitted to lower layers, and;
  // - for AM DRBs for Uu interface whose PDCP entities were suspended, from the first PDCP SDU for which the
  //   successful delivery of the corresponding PDCP Data PDU has not been confirmed by lower layers, for each
  //   PDCP SDU already associated with a PDCP SN:
  //   - consider the PDCP SDUs as received from upper layer;
  //   - perform transmission of the PDCP SDUs in ascending order of the COUNT value associated to the PDCP
  //     SDU prior to the PDCP re-establishment without restarting the discardTimer, as specified in clause 5.2.1;
  //
  //  For UM DRBs, when SDUs are associated with a PDCP SN they are immediately pushed to the lower-layer.
  //  As such, there is nothing to do here.
  //  For AM DRBs, PDCP entity suspension is not supported yet (TODO).

  // - for AM DRBs whose PDCP entities were not suspended, from the first PDCP SDU for which the successful
  //   delivery of the corresponding PDCP Data PDU has not been confirmed by lower layers, perform retransmission
  //   or transmission of all the PDCP SDUs already associated with PDCP SNs in ascending order of the COUNT
  //   values associated to the PDCP SDU prior to the PDCP entity re-establishment as specified below:
  //   - perform header compression of the PDCP SDU using ROHC as specified in the clause 5.7.4 and/or using
  //     EHC as specified in the clause 5.12.4;
  //   - perform integrity protection and ciphering of the PDCP SDU using the COUNT value associated with this
  //     PDCP SDU as specified in the clause 5.9 and 5.8;
  //   - submit the resulting PDCP Data PDU to lower layer, as specified in clause 5.2.1.
  if (is_am()) {
    retransmit_all_pdus();
  }
}

void pdcp_entity_tx::write_data_pdu_to_lower_layers(uint32_t count, byte_buffer buf)
{
  logger.log_info(
      buf.begin(), buf.end(), "TX PDU. type=data pdu_len={} sn={} count={}", buf.length(), SN(count), count);
  metrics_add_pdus(1, buf.length());
  pdcp_tx_pdu tx_pdu = {};
  tx_pdu.buf         = std::move(buf);
  if (is_drb()) {
    tx_pdu.pdcp_sn = SN(count); // Set only for data PDUs on DRBs.
  }
  lower_dn.on_new_pdu(std::move(tx_pdu));
}

void pdcp_entity_tx::write_control_pdu_to_lower_layers(byte_buffer buf)
{
  logger.log_info(buf.begin(), buf.end(), "TX PDU. type=ctrl pdu_len={}", buf.length());
  metrics_add_pdus(1, buf.length());
  pdcp_tx_pdu tx_pdu = {};
  tx_pdu.buf         = std::move(buf);
  // tx_pdu.pdcp_sn is not set for control PDUs
  lower_dn.on_new_pdu(std::move(tx_pdu));
}

void pdcp_entity_tx::handle_status_report(byte_buffer_chain status)
{
  byte_buffer buf = {status.begin(), status.end()};
  bit_decoder dec(buf);

  // Unpack and check PDU header
  uint32_t dc = 0;
  dec.unpack(dc, 1);
  if (dc != to_number(pdcp_dc_field::control)) {
    logger.log_warning(
        buf.begin(), buf.end(), "Invalid D/C field in status report. dc={}", to_number(pdcp_dc_field::control), dc);
    return;
  }
  uint32_t cpt = 0;
  dec.unpack(cpt, 3);
  if (cpt != to_number(pdcp_control_pdu_type::status_report)) {
    logger.log_warning(buf.begin(),
                       buf.end(),
                       "Invalid CPT field in status report. cpt={}",
                       to_number(pdcp_control_pdu_type::status_report),
                       cpt);
    return;
  }
  uint32_t reserved = 0;
  dec.unpack(reserved, 4);
  if (reserved != 0) {
    logger.log_warning(
        buf.begin(), buf.end(), "Ignoring status report because reserved bits are set. reserved={:#x}", reserved);
    return;
  }

  // Unpack FMC field
  uint32_t fmc = 0;
  dec.unpack(fmc, 32);
  logger.log_info("Status report. fmc={}", fmc);

  // Discard any SDU with COUNT < FMC
  for (auto it = discard_timers_map.begin(); it != discard_timers_map.end();) {
    if (it->first < fmc) {
      logger.log_debug("SDU discarded. count={}", it->first);
      lower_dn.on_discard_pdu(SN(it->first));
      it = discard_timers_map.erase(it);
    } else {
      ++it;
    }
  }

  // Evaluate bitmap: discard any SDU with the bit in the bitmap set to 1
  unsigned bit = 0;
  while (dec.unpack(bit, 1)) {
    fmc++;
    // Bit == 0: PDCP SDU with COUNT = (FMC + bit position) modulo 2^32 is missing.
    // Bit == 1: PDCP SDU with COUNT = (FMC + bit position) modulo 2^32 is correctly received.
    if (bit == 1) {
      logger.log_debug("SDU discarded. count={}", fmc);
      lower_dn.on_discard_pdu(SN(fmc));
      discard_timers_map.erase(fmc);
    }
  }
}

/*
 * Ciphering and Integrity Protection Helpers
 */
byte_buffer
pdcp_entity_tx::apply_ciphering_and_integrity_protection(byte_buffer hdr, const byte_buffer& sdu, uint32_t count)
{
  // TS 38.323, section 5.9: Integrity protection
  // The data unit that is integrity protected is the PDU header
  // and the data part of the PDU before ciphering.
  security::sec_mac mac = {};
  if (integrity_enabled == security::integrity_enabled::on) {
    byte_buffer buf = {};
    buf.append(hdr);
    buf.append(sdu);
    integrity_generate(mac, buf, count);
  }

  // TS 38.323, section 5.8: Ciphering
  // The data unit that is ciphered is the MAC-I and the
  // data part of the PDCP Data PDU except the
  // SDAP header and the SDAP Control PDU if included in the PDCP SDU.
  byte_buffer ct;
  if (ciphering_enabled == security::ciphering_enabled::on) {
    byte_buffer buf = {};
    buf.append(sdu);
    // Append MAC-I
    if (is_srb() || (is_drb() && (integrity_enabled == security::integrity_enabled::on))) {
      buf.append(mac);
    }
    ct = cipher_encrypt(buf, count);
  } else {
    ct.append(sdu);
    // Append MAC-I
    if (is_srb() || (is_drb() && (integrity_enabled == security::integrity_enabled::on))) {
      ct.append(mac);
    }
  }

  // Construct the protected buffer
  byte_buffer protected_buf;
  protected_buf.append(hdr);
  protected_buf.append(ct);

  return protected_buf;
}

void pdcp_entity_tx::integrity_generate(security::sec_mac& mac, byte_buffer_view buf, uint32_t count)
{
  switch (sec_cfg.integ_algo) {
    case security::integrity_algorithm::nia0:
      break;
    case security::integrity_algorithm::nia1:
      security_nia1(mac, sec_cfg.k_128_int, count, bearer_id, direction, buf.begin(), buf.end());
      break;
    case security::integrity_algorithm::nia2:
      security_nia2(mac, sec_cfg.k_128_int, count, bearer_id, direction, buf.begin(), buf.end());
      break;
    case security::integrity_algorithm::nia3:
      security_nia3(mac, sec_cfg.k_128_int, count, bearer_id, direction, buf.begin(), buf.end());
      break;
    default:
      break;
  }

  logger.log_debug("Integrity gen. count={} bearer_id={} dir={}", count, bearer_id, direction);
  logger.log_debug((uint8_t*)sec_cfg.k_128_int.data(), sec_cfg.k_128_int.size(), "Integrity gen key.");
  logger.log_debug(buf.begin(), buf.end(), "Integrity gen input message.");
  logger.log_debug((uint8_t*)mac.data(), mac.size(), "MAC generated.");
}

byte_buffer pdcp_entity_tx::cipher_encrypt(byte_buffer_view msg, uint32_t count)
{
  logger.log_debug("Cipher encrypt. count={} bearer_id={} dir={}", count, bearer_id, direction);
  logger.log_debug((uint8_t*)sec_cfg.k_128_enc.data(), sec_cfg.k_128_enc.size(), "Cipher encrypt key.");
  logger.log_debug(msg.begin(), msg.end(), "Cipher encrypt input msg.");

  byte_buffer ct;

  switch (sec_cfg.cipher_algo) {
    case security::ciphering_algorithm::nea0:
      ct.append(msg);
      break;
    case security::ciphering_algorithm::nea1:
      ct = security_nea1(sec_cfg.k_128_enc, count, bearer_id, direction, msg.begin(), msg.end());
      break;
    case security::ciphering_algorithm::nea2:
      ct = security_nea2(sec_cfg.k_128_enc, count, bearer_id, direction, msg.begin(), msg.end());
      break;
    case security::ciphering_algorithm::nea3:
      ct = security_nea3(sec_cfg.k_128_enc, count, bearer_id, direction, msg.begin(), msg.end());
      break;
    default:
      break;
  }
  logger.log_debug(ct.begin(), ct.end(), "Cipher encrypt output msg.");
  return ct;
}

/*
 * Status report and data recovery
 */
void pdcp_entity_tx::send_status_report()
{
  if (cfg.status_report_required) {
    logger.log_info("Status report triggered.");
    byte_buffer status_report = status_provider->compile_status_report();
    write_control_pdu_to_lower_layers(std::move(status_report));
  } else {
    logger.log_warning("Status report triggered but not configured.");
  }
}

void pdcp_entity_tx::data_recovery()
{
  srsran_assert(is_drb() && cfg.rlc_mode == pdcp_rlc_mode::am, "Invalid bearer type for data recovery.");
  logger.log_info("Data recovery requested.");

  /*
   * TS 38.323 Sec. 5.4.1:
   * [...] the receiving PDCP entity shall trigger a PDCP status report when:
   * [...] -upper layer requests a PDCP data recovery; [...]
   */
  if (cfg.status_report_required) {
    send_status_report();
  }
  retransmit_all_pdus();
}

void pdcp_entity_tx::discard_all_pdus()
{
  discard_timers_map.clear();
}

void pdcp_entity_tx::retransmit_all_pdus()
{
  for (const std::pair<const uint32_t, discard_info>& info : discard_timers_map) {
    // Prepare header
    pdcp_data_pdu_header hdr = {};
    hdr.sn                   = SN(info.second.count);

    // Pack header
    byte_buffer header_buf = {};
    write_data_pdu_header(header_buf, hdr);

    // Perform header compression if required
    // (TODO)

    // Perform integrity protection and ciphering
    byte_buffer protected_buf =
        apply_ciphering_and_integrity_protection(std::move(header_buf), info.second.sdu, info.second.count);
    write_data_pdu_to_lower_layers(info.first, std::move(protected_buf));
  }
}

/*
 * PDU Helpers
 */
void pdcp_entity_tx::write_data_pdu_header(byte_buffer& buf, const pdcp_data_pdu_header& hdr) const
{
  // Sanity check: 18-bit SN not allowed for SRBs
  srsran_assert(
      !(is_srb() && cfg.sn_size == pdcp_sn_size::size18bits), "Invalid SN size for SRB. sn_size={}", cfg.sn_size);

  byte_buffer_writer hdr_writer = buf;

  // Set D/C if required
  if (is_drb()) {
    hdr_writer.append(0x80); // D/C bit field (1).
  } else {
    hdr_writer.append(0x00); // No D/C bit field.
  }

  // Add SN
  switch (cfg.sn_size) {
    case pdcp_sn_size::size12bits:
      hdr_writer.back() |= (hdr.sn & 0x00000f00U) >> 8U;
      hdr_writer.append((hdr.sn & 0x000000ffU));
      break;
    case pdcp_sn_size::size18bits:
      hdr_writer.back() |= (hdr.sn & 0x00030000U) >> 16U;
      hdr_writer.append((hdr.sn & 0x0000ff00U) >> 8U);
      hdr_writer.append((hdr.sn & 0x000000ffU));
      break;
    default:
      logger.log_error("Invalid sn_size={}", cfg.sn_size);
  }
}

/*
 * Timers
 */

void pdcp_entity_tx::stop_discard_timer(uint32_t highest_sn)
{
  if (!(cfg.discard_timer != pdcp_discard_timer::infinity && cfg.discard_timer != pdcp_discard_timer::not_configured)) {
    logger.log_debug("Cannot stop discard timers. Not configured or infinite. highest_sn={}", highest_sn);
    return;
  }
  if (discard_timers_map.empty()) {
    logger.log_debug("Cannot stop discard timers. No timers active. highest_sn={}", highest_sn);
    return;
  }

  // TX_NEXT_DELIV is the COUNT value of the first PDCP PDU for which the delivery is not confirmed.
  uint32_t tx_next_deliv = discard_timers_map.begin()->first;

  /*
   * Calculate HIGHEST_COUNT. This is adapted from TS 38.331 Sec. 5.2.2 "Receive operation" of the Rx side.
   *
   * - if HIGHEST_SN < SN(TX_NEXT_DELIV) – Window_Size:
   *   - HIGHEST_HFN = HFN(TX_NEXT_DELIV) + 1.
   * - else if HIGHEST_SN >= SN(TX_NEXT_DELIV) + Window_Size:
   *   - HIGHEST_HFN = HFN(TX_NEXT_DELIV) – 1.
   * - else:
   *   - HIGHEST_HFN = HFN(TX_NEXT_DELIV);
   * - HIGHEST_COUNT = [HIGHEST_HFN, HIGHEST_SN].
   */
  uint32_t highest_hfn, highest_count;
  if ((int64_t)highest_sn < (int64_t)SN(tx_next_deliv) - (int64_t)window_size) {
    highest_hfn = HFN(tx_next_deliv) + 1;
  } else if (highest_sn >= SN(tx_next_deliv) + window_size) {
    highest_hfn = HFN(tx_next_deliv) - 1;
  } else {
    highest_hfn = HFN(tx_next_deliv);
  }
  highest_count = COUNT(highest_hfn, highest_sn);
  logger.log_debug("Stopping discard timers. highest_count={} highest_sn={} tx_next_deliv={}",
                   highest_count,
                   highest_sn,
                   tx_next_deliv);

  // Remove timers from map
  for (uint32_t count = tx_next_deliv; count <= highest_count; count++) {
    discard_timers_map.erase(count);
    logger.log_debug("Stopped discard timer. count={}", count);
  }
}

// Discard Timer Callback (discardTimer)
void pdcp_entity_tx::discard_callback::operator()(timer_id_t timer_id)
{
  parent->logger.log_debug("Discard timer expired. count={}", discard_count);

  // Notify the RLC of the discard. It's the RLC to actually discard, if no segment was transmitted yet.
  parent->lower_dn.on_discard_pdu(pdcp_compute_sn(discard_count, parent->sn_size));

  // Add discard to metrics
  parent->metrics_add_discard_timouts(1);

  // Remove timer from map
  // NOTE: this will delete the callback. It *must* be the last instruction.
  parent->discard_timers_map.erase(discard_count);
}
