/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pdcp_entity_rx.h"

using namespace srsgnb;

bool pdcp_entity_rx::read_data_pdu_header(const byte_buffer& buf, uint32_t& sn)
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
      sn = (*buf_reader & 0x0fU) << 16U; // first 4 bits SN
      ++buf_reader;
      sn |= (*buf_reader & 0xffU) << 8U; // last 8 bits SN
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
