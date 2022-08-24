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

bool pdcp_entity_tx::write_data_pdu_header(byte_buffer& buf, uint32_t count)
{
  // Sanity check: 18bit SRB not allowed
  srsgnb_assert(!(is_srb() && cfg.sn_size == pdcp_sn_size::size18bits), "Invalid 18 bit SRB PDU");

  byte_buffer_writer hdr_writer = buf;

  // Set D/C if required
  if (is_drb()) {
    hdr_writer.append(0x80); // 1 bit D/C field
  }
  uint32_t sn = SN(count);
  // Add SN
  switch (cfg.sn_size) {
    case pdcp_sn_size::size12bits:
      srsran::uint16_to_uint8(SN(count), sdu->msg);
      if (is_drb()) {
        sdu->msg[0] |= 0x80; // On Data PDUs for DRBs we must set the D flag.
      }
      break;
    case pdcp_sn_size::size18bits:
      srsran::uint24_to_uint8(SN(count), sdu->msg);
      sdu->msg[0] |= 0x80; // Data PDU and SN LEN 18 implies DRB, D flag must be present
      break;
    default:
      logger.error("Invalid SN length configuration: %d bits", cfg.sn_len);
  }
}
