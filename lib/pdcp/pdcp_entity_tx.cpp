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
void pdcp_entity_tx::handle_sdu(byte_buffer buf)
{
  bool max_hfn_reached = false; // FIXME actually check HFN
  if (max_hfn_reached) {
    upper_cn.on_max_hfn_reached();
  }
  lower_dn.on_new_pdu(std::move(buf));
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
