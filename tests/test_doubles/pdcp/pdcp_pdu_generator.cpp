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

///\file
///\brief pdcp_pdu_generator: Contains helper functions to generate dummy PDCP PDUs for testing purposes.

#pragma once

#include "pdcp_pdu_generator.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

byte_buffer srsran::test_helpers::create_pdcp_pdu(pdcp_sn_size pdcp_sn_len,
                                                  bool         is_srb,
                                                  uint32_t     pdcp_sn,
                                                  uint32_t     sdu_size,
                                                  uint8_t      first_byte)
{
  if (is_srb && pdcp_sn_len != pdcp_sn_size::size12bits) {
    report_error("Cannot create SRB PDU: Invalid pdcp_sn_len={}", pdcp_sn_len);
  }

  uint32_t pdcp_hdr_len = 0;
  switch (pdcp_sn_len) {
    case pdcp_sn_size::size12bits:
      pdcp_hdr_len = 2;
      break;
    case pdcp_sn_size::size18bits:
      pdcp_hdr_len = 3;
      break;
    default:
      report_error("Unsupported pdcp_sn_len={}", pdcp_sn_len);
      break;
  }
  report_error_if_not(sdu_size > pdcp_hdr_len,
                      "Cannot fit pdcp_hdr_len={} (pdcp_sn_len={}) + payload into sdu_size={}",
                      pdcp_hdr_len,
                      pdcp_sn_len,
                      sdu_size);

  byte_buffer sdu_buf;
  bit_encoder encoder{sdu_buf};
  bool        write_ok;

  // D/C field (or R for SRBs)
  write_ok = encoder.pack(is_srb ? 0 : 1, 1);

  switch (pdcp_sn_len) {
    case pdcp_sn_size::size12bits:
      // Reserved
      write_ok &= encoder.pack(0, 3);
      // PDCP SN
      write_ok &= encoder.pack(pdcp_sn, 12);
      break;
    case srsran::pdcp_sn_size::size18bits:
      // Reserved
      write_ok &= encoder.pack(0, 5);
      // PDCP SN
      write_ok &= encoder.pack(pdcp_sn, 18);
      break;
    default:
      report_error("Cannot create SDU: Unsupported pdcp_sn_len={}", pdcp_sn_len);
      return {};
  }

  if (!write_ok) {
    report_error("Cannot create SDU: Failed to write PDCP header. pdcp_sn_len={} pdcp_sn={} sdu_size={}",
                 pdcp_sn_len,
                 pdcp_sn,
                 sdu_size);
    return {};
  }

  // Fill remaining size with payload bytes
  uint32_t payload_len = sdu_size - sdu_buf.length();
  for (uint32_t k = 0; k < payload_len; ++k) {
    report_error_if_not(sdu_buf.append(first_byte + k), "Failed to allocate byte buffer");
  }
  return sdu_buf;
}
