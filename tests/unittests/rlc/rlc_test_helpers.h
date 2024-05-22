/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/error_handling.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Fixture base class for all RLC {Tx,Rx} {AM,UM,TM} tests which contains commonly uses helper methods
class rlc_trx_test
{
public:
  /// \brief Creates a byte_buffer serving as SDU for RLC, i.e. a PDCP PDU.
  ///
  /// The produced SDU contains a PDCP data PDU header followed by an incremental sequence of bytes starting with the
  /// value given by first_byte, i.e. if first_byte = 0xfc, the SDU will be <PDCP-HDR> 0xfc 0xfe 0xff 0x00 0x01 ...
  /// The minimum sdu_size is 3 for 12-bit PDCP SNs and 4 for 18-bit PDCP SNs (i.e. PDCP-HDR + 1 or more bytes).
  ///
  /// \param pdcp_sn_len Size of the PDCP sequence number
  /// \param pdcp_sn PDCP sequence number
  /// \param sdu_size Size of the SDU (including PDCP header)
  /// \param first_byte Value of the first payload byte after PDCP header
  /// \return the produced SDU as a byte_buffer
  byte_buffer create_sdu(pdcp_sn_size pdcp_sn_len, uint32_t pdcp_sn, uint32_t sdu_size, uint8_t first_byte = 0) const
  {
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

    // D/C field
    write_ok = encoder.pack(1, 1);

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
};
