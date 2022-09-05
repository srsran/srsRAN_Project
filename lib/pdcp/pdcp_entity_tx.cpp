/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_entity_tx.h"
#include "srsgnb/support/srsgnb_assert.h"

using namespace srsgnb;

/// \brief Receive an SDU from the upper layers, apply encription
/// and integrity protection and pass the resulting PDU
/// to the lower layers.
///
/// \param buf Buffer that hold the SDU from higher layers.
/// \ref TS 38.323 section 5.2.1: Transmit operation
void pdcp_entity_tx::handle_sdu(byte_buffer buf)
{
  bool max_hfn_reached = false; // FIXME actually check HFN
  if (max_hfn_reached) {
    upper_cn.on_max_hfn_reached();
  }

  // Start discard timer
  // TODO

  // Perform header compression
  // TODO

  // Write PDCP header info
  byte_buffer pdu_buf = {};
  write_data_pdu_header(pdu_buf, st.tx_next);
  pdu_buf.append(buf);

  // Apply ciphering and integrity protection
  apply_ciphering_and_integrity_protection(pdu_buf, st.tx_next);

  // Set meta-data for RLC (TODO)
  // sdu->md.pdcp_sn = tx_next;

  logger.log_info(pdu_buf.begin(),
                  pdu_buf.end(),
                  "TX PDU ({}B), COUNT={}, HFN={}, SN={}, integrity={}, encryption={}",
                  buf.length(),
                  st.tx_next,
                  HFN(st.tx_next),
                  SN(st.tx_next),
                  integrity_direction,
                  ciphering_direction);

  // Check if PDCP is associated with more than on RLC entity TODO
  // Write to lower layers
  lower_dn.on_new_pdu(std::move(pdu_buf));

  // Increment TX_NEXT
  st.tx_next++;
}

/*
 * Ciphering and Integrity Protection Helpers
 */
void pdcp_entity_tx::apply_ciphering_and_integrity_protection(byte_buffer&, uint32_t count)
{
  // TS 38.323, section 5.9: Integrity protection
  // The data unit that is integrity protected is the PDU header
  // and the data part of the PDU before ciphering.
  // uint8_t mac[4] = {};
  if (is_srb() || (is_drb() && (integrity_direction == pdcp_tx_direction::tx))) {
    // integrity_generate(sdu->msg, sdu->N_bytes, count, mac);
  }
  // Append MAC-I
  if (is_srb() || (is_drb() && (integrity_direction == pdcp_tx_direction::tx))) {
    // append_mac(sdu, mac);
  }

  // TS 38.323, section 5.8: Ciphering
  // The data unit that is ciphered is the MAC-I and the
  // data part of the PDCP Data PDU except the
  // SDAP header and the SDAP Control PDU if included in the PDCP SDU.
  if (ciphering_direction == pdcp_tx_direction::tx) {
    // cipher_encrypt(&sdu->msg[cfg.hdr_len_bytes], sdu->N_bytes - cfg.hdr_len_bytes, count, &sdu->msg[hdr_len_bytes]);
  }
}

void pdcp_entity_tx::integrity_generate(uint8_t* msg, uint32_t msg_len, uint32_t count, sec_mac& mac)
{
  // If control plane use RRC integrity key. If data use user plane key
  const sec_128_as_key& k_int = is_srb() ? sec_cfg.k_rrc_int : sec_cfg.k_up_int;
  switch (sec_cfg.integ_algo) {
    case integrity_algorithm::nia0:
      break;
    case integrity_algorithm::nia1:
      security_nia1(k_int, count, 0, 0, msg, msg_len, mac);
      break;
    case integrity_algorithm::nia2:
      // security_nia2(&k128, count, 0, cfg.tx_direction, msg, msg_len, mac);
      break;
    case integrity_algorithm::nia3:
      // security_nia3(&k128, count, 0, cfg.tx_direction, msg, msg_len, mac);
      break;
    default:
      break;
  }

  // logger.log_debug("Integrity gen input: COUNT {}, Bearer ID {}, Direction {}", count, 0, "Downlink");
  // logger.log_debug(k_int.begin(), k_int.end(), "Integrity gen key:");
  //  logger.log_debug(msg, msg_len, "Integrity gen input msg:");
  //  logger.log_debug(mac, 4, "MAC (generated)");
}

/*
 * PDU Helpers
 */
bool pdcp_entity_tx::write_data_pdu_header(byte_buffer& buf, uint32_t count)
{
  // Sanity check: 18bit SRB not allowed
  srsgnb_assert(!(is_srb() && cfg.sn_size == pdcp_sn_size::size18bits), "Invalid 18 bit SRB PDU");

  byte_buffer_writer hdr_writer = buf;

  // Set D/C if required
  if (is_drb()) {
    hdr_writer.append(0x80); // D/C bit field (1).
  } else {
    hdr_writer.append(0x00); // No D/C bit field.
  }

  // Set SN
  uint32_t sn = SN(count);
  // Add SN
  switch (cfg.sn_size) {
    case pdcp_sn_size::size12bits:
      hdr_writer.back() |= (sn & 0x00000f00U) >> 8U;
      hdr_writer.append((sn & 0x000000ffU));
      break;
    case pdcp_sn_size::size18bits:
      hdr_writer.back() |= (sn & 0x00030000U) >> 16U;
      hdr_writer.append((sn & 0x0000ff00U) >> 8U);
      hdr_writer.append((sn & 0x000000ffU));
      break;
    default:
      logger.log_error("Invalid SN length configuration: {} bits", cfg.sn_size);
  }
  return true;
}
