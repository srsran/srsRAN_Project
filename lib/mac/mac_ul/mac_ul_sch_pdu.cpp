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

using namespace srsran;

bool mac_ul_sch_subpdu::unpack(const byte_buffer& subpdu)
{
  byte_buffer_reader reader = subpdu;
  return unpack(reader);
}

bool mac_ul_sch_subpdu::unpack(byte_buffer_reader& subpdu_reader)
{
  unsigned subpdu_len = subpdu_reader.length();
  if (subpdu_len == 0) {
    srslog::fetch_basic_logger("MAC", true).warning("Invalid UL MAC PDU. Cause: Empty subPDU.");
    return false;
  }
  payload_view = {};

  // Skip R, read F bit and LCID
  F_bit    = (*subpdu_reader & 0x40U) > 0;
  lcid_val = *subpdu_reader & 0x3fU;
  ++subpdu_reader;
  header_length = 1;

  if (not lcid_val.is_valid_lcid()) {
    srslog::fetch_basic_logger("MAC").warning("Invalid UL MAC PDU. Cause: Unrecognized lcid={}.", lcid_val);
    return false;
  }

  uint32_t sdu_length = 0;
  if (lcid_val.has_length_field()) {
    // Variable-sized MAC CEs or SDUs

    if (subpdu_len < (F_bit ? 3 : 2)) {
      srslog::fetch_basic_logger("MAC").warning(
          "Invalid UL MAC PDU. Cause: Not enough bytes remaining in PDU to decode length prefix.");
      return false;
    }

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

    if (subpdu_len < header_length + sdu_length) {
      srslog::fetch_basic_logger("MAC").warning(
          "Invalid UL MAC PDU. Cause: Not enough bytes remaining in PDU to decode SDU payload ({} < {}).",
          subpdu_len - header_length,
          sdu_length);
      return false;
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
      sdu_length = lcid_val.sizeof_ce();

      if (subpdu_len < header_length + sdu_length) {
        srslog::fetch_basic_logger("MAC").warning(
            "Invalid UL MAC PDU. Cause: Not enough bytes remaining in PDU to decode CE payload ({} < {}).",
            subpdu_len - header_length,
            sdu_length);
        return false;
      }
      payload_view = subpdu_reader.split_and_advance(sdu_length);
    }
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
    if (subpdus.full()) {
      srslog::fetch_basic_logger("MAC", true)
          .warning("Maximum number of subPDUs per UL MAC PDU limit of {} was reached.", (unsigned)MAX_SUBPDUS_PER_PDU);
      return true;
    }
    mac_ul_sch_subpdu& subpdu = subpdus.emplace_back();
    if (not subpdu.unpack(reader)) {
      // Discard all decoded subPDUs.
      clear();
      return false;
    }
  }

  return true;
}
