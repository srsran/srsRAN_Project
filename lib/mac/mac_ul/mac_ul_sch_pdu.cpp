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

#include "mac_ul_sch_pdu.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

error_type<std::string> mac_ul_sch_subpdu::unpack(const byte_buffer& subpdu)
{
  byte_buffer_reader reader = subpdu;
  return unpack(reader);
}

error_type<std::string> mac_ul_sch_subpdu::unpack(byte_buffer_reader& subpdu_reader)
{
  unsigned subpdu_len = subpdu_reader.length();
  if (subpdu_len == 0) {
    return std::string{"Empty subPDU."};
  }
  payload_view = {};

  // Skip R, read F bit and LCID
  F_bit    = (*subpdu_reader & 0x40U) > 0;
  lcid_val = *subpdu_reader & 0x3fU;
  ++subpdu_reader;
  header_length = 1;

  if (not lcid_val.is_valid_lcid()) {
    return fmt::format("Unrecognized lcid={}.", lcid_val);
  }

  uint32_t sdu_length = 0;
  if (lcid_val.has_length_field()) {
    // Variable-sized MAC CEs or SDUs

    if (subpdu_len < (F_bit ? 3 : 2)) {
      return fmt::format("Not enough bytes remaining in PDU to decode length prefix.");
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
      return fmt::format(
          "Not enough bytes remaining in PDU to decode SDU payload ({} < {}).", subpdu_len - header_length, sdu_length);
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
        return fmt::format("Not enough bytes remaining in PDU to decode CE payload ({} < {}).",
                           subpdu_len - header_length,
                           sdu_length);
      }
      payload_view = subpdu_reader.split_and_advance(sdu_length);
    }
  }
  return {};
}

void mac_ul_sch_pdu::clear()
{
  subpdus.clear();
}

error_type<std::string> mac_ul_sch_pdu::unpack(const byte_buffer& payload)
{
  byte_buffer_reader reader = payload;
  while (not reader.empty()) {
    if (subpdus.full()) {
      srslog::fetch_basic_logger("MAC", true)
          .warning("Maximum number of subPDUs per UL MAC PDU limit of {} was reached.", (unsigned)MAX_SUBPDUS_PER_PDU);
      return {};
    }

    mac_ul_sch_subpdu&      subpdu = subpdus.emplace_back();
    error_type<std::string> ret    = subpdu.unpack(reader);
    if (ret.is_error()) {
      // Discard all decoded subPDUs.
      clear();
      return ret;
    }
  }

  return {};
}
