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

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/bit_encoding.h"
#include <cstdint>
#include <optional>

namespace srsran {

/// \brief Extract PDCP SN from PDCP PDU (or RLC SDU) if applicable.
///
/// This function extracts the PDCP SN from a PDCP PDU (or RLC SDU) in case of a data PDU.
/// Control PDUs return no value as they do not contain a PDCP SN.
///
/// \param pdcp_pdu PDCP PDU (or RLC SDU) from which the PDCP SN shall be extracted.
/// \param pdcp_sn_len The length of the PDCP SN (12 bit or 18 bit) in the PDU.
/// \param is_srb Determines the bearer type: SRB (true) or DRB (false).
/// \return The PDCP SN of the PDU in case of a data PDU; or no value in case of control PDU.
inline std::optional<uint32_t>
get_pdcp_sn(byte_buffer_view pdcp_pdu, pdcp_sn_size pdcp_sn_len, bool is_srb, srslog::basic_logger& logger)
{
  if (pdcp_pdu.empty()) {
    logger.error("Cannot get PDCP SN from empty PDU");
    return {};
  }

  if (is_srb && pdcp_sn_len != pdcp_sn_size::size12bits) {
    logger.error(
        "Cannot get PDCP SN of SRB PDU: Invalid pdcp_sn_len={}. pdcp_pdu_len=", pdcp_sn_len, pdcp_pdu.length());
    return {};
  }

  bit_decoder decoder{pdcp_pdu};
  bool        read_ok;

  // D/C field (or R for SRBs)
  uint8_t dc_field = {};
  read_ok          = decoder.unpack(dc_field, 1);
  if (!read_ok) {
    logger.error("Failed to get PDCP SN: Cannot read D/C field. pdcp_pdu_len={}", pdcp_pdu.length());
    return {};
  }

  if (!is_srb && dc_field == 0) {
    logger.debug("Cannot get PDCP SN of DRB control PDU");
    return {};
  }

  if (is_srb && dc_field == 1) {
    logger.warning("Cannot get PDCP SN of SRB PDU: Reserved MSB set. pdcp_pdu_len={}", pdcp_pdu.length());
    return {};
  }

  uint8_t  reserved;
  uint32_t pdcp_sn;
  switch (pdcp_sn_len) {
    case pdcp_sn_size::size12bits:
      read_ok &= decoder.unpack(reserved, 3);
      read_ok &= decoder.unpack(pdcp_sn, 12);
      break;
    case srsran::pdcp_sn_size::size18bits:
      read_ok &= decoder.unpack(reserved, 5);
      read_ok &= decoder.unpack(pdcp_sn, 18);
      break;
    default:
      logger.error("Cannot get PDCP SN: Unsupported pdcp_sn_len={}", pdcp_sn_len);
      return {};
  }

  if (!read_ok) {
    logger.error("Failed to get PDCP SN: Cannot read PDCP header. pdcp_pdu_len={}", pdcp_pdu.length());
    return {};
  }

  if (reserved != 0) {
    if (is_srb) {
      logger.warning("Cannot get PDCP SN for SRB PDU with reserved bits set. pdcp_pdu_len={}", pdcp_pdu.length());
    } else {
      logger.warning("Cannot get PDCP SN for DRB data PDU with reserved bits set. pdcp_pdu_len={}", pdcp_pdu.length());
    }
    return {};
  }

  return pdcp_sn;
}

} // namespace srsran
