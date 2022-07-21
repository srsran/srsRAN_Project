/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_ul_sch_pdu.h"
#include "srsgnb/srslog/srslog.h"

using namespace srsgnb;

bool mac_ul_sch_subpdu::unpack(const byte_buffer& subpdu)
{
  byte_buffer_reader reader = subpdu;
  return unpack(reader);
}

bool mac_ul_sch_subpdu::unpack(byte_buffer_reader& subpdu_reader)
{
  payload_view = {};
  if (subpdu_reader.empty()) {
    srslog::fetch_basic_logger("MAC-NR").warning("Empty MAC subPDU");
    return false;
  }

  // Skip R, read F bit and LCID
  F_bit    = (*subpdu_reader & 0x40U) > 0;
  lcid_val = *subpdu_reader & 0x3fU;
  ++subpdu_reader;
  header_length = 1;

  uint32_t sdu_length = 0;
  if (lcid_val.is_valid_lcid()) {
    // Variable-sized MAC CEs
    if (lcid_val.has_length_field()) {
      // Read first length byte
      sdu_length = (uint32_t)*subpdu_reader;
      ++subpdu_reader;
      header_length++;

      if (F_bit) {
        // add second length byte
        sdu_length = sdu_length << 8U | ((uint32_t)*subpdu_reader & 0xffU);
        ++subpdu_reader;
        header_length++;
      }
      payload_view = subpdu_reader.split_and_advance(sdu_length);
    } else {
      // Fixed-sized MAC CEs
      if (lcid_val == lcid_ul_sch_t::PADDING) {
        // set subPDU length to rest of PDU
        // 1 Byte R/LCID MAC subheader
        payload_view  = subpdu_reader.view();
        subpdu_reader = {};
      } else {
        sdu_length   = lcid_val.sizeof_ce();
        payload_view = subpdu_reader.split_and_advance(sdu_length);
      }
    }
  } else {
    srslog::fetch_basic_logger("MAC-NR").warning("Invalid LCID ({}) in MAC PDU", lcid_val);
  }
  return true;
}

void mac_ul_sch_pdu::clear()
{
  subpdus.clear();
}

bool mac_ul_sch_pdu::unpack(const byte_buffer& payload)
{
  byte_buffer_reader reader = payload;
  while (not reader.empty()) {
    mac_ul_sch_subpdu subpdu{};
    if (not subpdu.unpack(reader)) {
      return false;
    }
    subpdus.push_back(subpdu);
  }

  return true;
}
