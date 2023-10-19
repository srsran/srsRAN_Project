/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/rlc/rlc_config.h"
#include "srsran/srslog/srslog.h"
#include "fmt/format.h"

namespace srsran {

constexpr size_t rlc_um_pdu_header_size_complete_sdu = 1;

constexpr size_t rlc_um_pdu_header_size_6bit_sn_no_so  = 1;
constexpr size_t rlc_um_pdu_header_size_12bit_sn_no_so = 2;
constexpr size_t rlc_um_pdu_header_size_no_so(rlc_um_sn_size sn_size)
{
  switch (sn_size) {
    case rlc_um_sn_size::size6bits:
      return rlc_um_pdu_header_size_6bit_sn_no_so;
    case rlc_um_sn_size::size12bits:
      return rlc_um_pdu_header_size_12bit_sn_no_so;
  }
  srsran_assertion_failure("Cannot determine RLC UM PDU header size without SO: unsupported sn_size={}.",
                           to_number(sn_size));
  return rlc_um_pdu_header_size_6bit_sn_no_so;
}

constexpr size_t rlc_um_pdu_header_size_6bit_sn_with_so  = 3;
constexpr size_t rlc_um_pdu_header_size_12bit_sn_with_so = 4;
constexpr size_t rlc_um_pdu_header_size_with_so(rlc_um_sn_size sn_size)
{
  switch (sn_size) {
    case rlc_um_sn_size::size6bits:
      return rlc_um_pdu_header_size_6bit_sn_with_so;
    case rlc_um_sn_size::size12bits:
      return rlc_um_pdu_header_size_12bit_sn_with_so;
  }
  srsran_assertion_failure("Cannot determine RLC UM PDU header size with SO: unsupported sn_size={}.",
                           to_number(sn_size));
  return rlc_um_pdu_header_size_6bit_sn_no_so;
}

struct rlc_um_pdu_header {
  rlc_si_field   si;      ///< Segmentation info
  rlc_um_sn_size sn_size; ///< Sequence number size (6 or 12 bits)
  uint16_t       sn;      ///< Sequence number
  uint16_t       so;      ///< Segment offset
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 38.322 version 15.3.0 Section 6.2.2.3
 ***************************************************************************/
inline bool
rlc_um_read_data_pdu_header(const byte_buffer_view& pdu, const rlc_um_sn_size sn_size, rlc_um_pdu_header* header)
{
  byte_buffer_reader pdu_reader = pdu;
  if (pdu_reader.empty()) {
    srslog::fetch_basic_logger("RLC").warning(
        "UMD PDU too small. pdu_len={} hdr_len={}", pdu.length(), rlc_um_pdu_header_size_no_so(sn_size));
    return false;
  }

  header->sn_size = sn_size;

  // Fixed part
  if (sn_size == rlc_um_sn_size::size6bits) {
    header->si = (rlc_si_field)((*pdu_reader >> 6U) & 0x03U); // 2 bits SI
    header->sn = *pdu_reader & 0x3fU;                         // 6 bits SN
    // sanity check
    if (header->si == rlc_si_field::full_sdu and header->sn != 0) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, reserved bits are set.");
      return false;
    }
    ++pdu_reader;
  } else if (sn_size == rlc_um_sn_size::size12bits) {
    header->si = (rlc_si_field)((*pdu_reader >> 6U) & 0x03U); // 2 bits SI
    header->sn = (*pdu_reader & 0x0fU) << 8U;                 // 4 bits SN
    if (header->si == rlc_si_field::full_sdu and header->sn != 0) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, reserved bits are set.");
      return false;
    }

    // sanity check
    if (header->si == rlc_si_field::first_segment) {
      // make sure two reserved bits are not set
      if (((*pdu_reader >> 4U) & 0x03U) != 0) {
        srslog::fetch_basic_logger("RLC").error("Malformed PDU, reserved bits are set.");
        return false;
      }
    }

    if (header->si != rlc_si_field::full_sdu) {
      // continue unpacking remaining SN
      ++pdu_reader;
      if (pdu_reader.empty()) {
        srslog::fetch_basic_logger("RLC").error("Malformed PDU, missing lower byte of SN.");
        return false;
      }
      header->sn |= (*pdu_reader & 0xffU); // 8 bits SN
    }

    ++pdu_reader;
  } else {
    srslog::fetch_basic_logger("RLC").error("Unsupported sn_size={}.", to_number(sn_size));
    return false;
  }

  // Read optional part
  if (header->si == rlc_si_field::last_segment || header->si == rlc_si_field::middle_segment) {
    // read SO
    if (pdu_reader.empty()) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, missing upper byte of SO.");
      return false;
    }
    header->so = (*pdu_reader & 0xffU) << 8U;
    ++pdu_reader;
    if (pdu_reader.empty()) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, missing lower byte of SO.");
      return false;
    }
    header->so |= (*pdu_reader & 0xffU);
    ++pdu_reader;
  }

  return true;
}

inline size_t rlc_um_nr_packed_length(const rlc_um_pdu_header& header)
{
  size_t len = 0;
  if (header.si == rlc_si_field::full_sdu) {
    // that's all ..
    len++;
  } else {
    if (header.sn_size == rlc_um_sn_size::size6bits) {
      // Only 1B for SN
      len++;
    } else {
      // 2 B for 12bit SN
      len += 2;
    }
    if (header.so) {
      // Two bytes always for segment information
      len += 2;
    }
  }
  return len;
}

inline bool rlc_um_write_data_pdu_header(const rlc_um_pdu_header& header, byte_buffer& pdu)
{
  byte_buffer        hdr_buf;
  byte_buffer_writer hdr_writer = hdr_buf;
  // write SI field
  if (not hdr_writer.append((to_number(header.si) & 0x03U) << 6U)) { // 2 bits SI
    return false;
  }

  if (header.si == rlc_si_field::full_sdu) {
    // that's all
  } else {
    if (header.sn_size == rlc_um_sn_size::size6bits) {
      // write SN
      hdr_writer.back() |= (header.sn & 0x3fU); // 6 bit SN
    } else {
      // 12bit SN
      hdr_writer.back() |= (header.sn >> 8U) & 0xfU;  // high part of SN (4 bit)
      if (not hdr_writer.append(header.sn & 0xffU)) { // remaining 8 bit SN
        return false;
      }
    }
    if (header.so != 0) {
      // write SO
      if (not hdr_writer.append((header.so) >> 8U)) { // first part of SO
        return false;
      }
      if (not hdr_writer.append(header.so & 0xffU)) { // second part of SO
        return false;
      }
    }
  }
  pdu.prepend(std::move(hdr_buf));
  return true;
}

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::rlc_um_pdu_header> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_um_pdu_header& hdr, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (hdr.si == srsran::rlc_si_field::full_sdu) {
      // Header of full SDU only has SI; no SN and no SO.
      return format_to(ctx.out(), "si={}", hdr.si, hdr.sn, hdr.so);
    }
    return format_to(ctx.out(), "si={} sn={} so={}", hdr.si, hdr.sn, hdr.so);
  }
};
} // namespace fmt
