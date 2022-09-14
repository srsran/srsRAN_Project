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
  /*
  // Log PDU
  logger.log_info(buf.begin(),
                  buf.end(),
                  "RX PDU ({} B), integrity={}, encryption={}",
                  buf->N_bytes,
                  integrity_enabled,
                  encryption_enabled);

  if (rx_overflow) {
    logger.warning("Rx PDCP COUNTs have overflowed. Discarding SDU.");
    return;
  }

  // Sanity check
  if (pdu->N_bytes <= cfg.hdr_len_bytes) {
    return;
  }
  logger.debug("Rx PDCP state - RX_NEXT=%u, RX_DELIV=%u, RX_REORD=%u", rx_next, rx_deliv, rx_reord);

  // Extract RCVD_SN from header
  uint32_t rcvd_sn = read_data_header(pdu);
  */
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
  /*
 uint32_t rcvd_hfn, rcvd_count;
 if ((int64_t)rcvd_sn < (int64_t)SN(rx_deliv) - (int64_t)window_size) {
   rcvd_hfn = HFN(rx_deliv) + 1;
 } else if (rcvd_sn >= SN(rx_deliv) + window_size) {
   rcvd_hfn = HFN(rx_deliv) - 1;
 } else {
   rcvd_hfn = HFN(rx_deliv);
 }
 rcvd_count = COUNT(rcvd_hfn, rcvd_sn);
 */
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
