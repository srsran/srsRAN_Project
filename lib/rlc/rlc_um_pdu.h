/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSGNB_LIB_RLC_UM_PDU_H
#define SRSGNB_LIB_RLC_UM_PDU_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

constexpr size_t rlc_um_pdu_header_size_complete_sdu     = 1;
constexpr size_t rlc_um_pdu_header_size_6bit_sn_no_so    = 1;
constexpr size_t rlc_um_pdu_header_size_12bit_sn_no_so   = 2;
constexpr size_t rlc_um_pdu_header_size_6bit_sn_with_so  = 3;
constexpr size_t rlc_um_pdu_header_size_12bit_sn_with_so = 4;

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
inline bool rlc_um_read_data_pdu_header(const byte_buffer& pdu, const rlc_um_sn_size sn_size, rlc_um_pdu_header* header)
{
  byte_buffer_reader pdu_reader = pdu;
  if (pdu_reader.empty()) {
    srslog::fetch_basic_logger("RLC").warning("Unpacking header of empty RLC PDU");
    return false;
  }

  header->sn_size = sn_size;

  // Fixed part
  if (sn_size == rlc_um_sn_size::size6bits) {
    header->si = (rlc_si_field)((*pdu_reader >> 6U) & 0x03U); // 2 bits SI
    header->sn = *pdu_reader & 0x3fU;                         // 6 bits SN
    // sanity check
    if (header->si == rlc_si_field::full_sdu and header->sn != 0) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, reserved bits are set");
      return false;
    }
    ++pdu_reader;
  } else if (sn_size == rlc_um_sn_size::size12bits) {
    header->si = (rlc_si_field)((*pdu_reader >> 6U) & 0x03U); // 2 bits SI
    header->sn = (*pdu_reader & 0x0fU) << 8U;                 // 4 bits SN
    if (header->si == rlc_si_field::full_sdu and header->sn != 0) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, reserved bits are set");
      return false;
    }

    // sanity check
    if (header->si == rlc_si_field::first_segment) {
      // make sure two reserved bits are not set
      if (((*pdu_reader >> 4U) & 0x03U) != 0) {
        srslog::fetch_basic_logger("RLC").error("Malformed PDU, reserved bits are set");
        return false;
      }
    }

    if (header->si != rlc_si_field::full_sdu) {
      // continue unpacking remaining SN
      ++pdu_reader;
      header->sn |= (*pdu_reader & 0xffU); // 8 bits SN
    }

    ++pdu_reader;
  } else {
    srslog::fetch_basic_logger("RLC").error("Unsupported SN length\n");
    return false;
  }

  // Read optional part
  if (header->si == rlc_si_field::last_segment || header->si == rlc_si_field::neither_first_nor_last_segment) {
    // read SO
    header->so = (*pdu_reader & 0xffU) << 8U;
    ++pdu_reader;
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
  hdr_writer.append((to_number(header.si) & 0x03U) << 6U); // 2 bits SI

  if (header.si == rlc_si_field::full_sdu) {
    // that's all
  } else {
    if (header.sn_size == rlc_um_sn_size::size6bits) {
      // write SN
      hdr_writer.back() |= (header.sn & 0x3fU); // 6 bit SN
    } else {
      // 12bit SN
      hdr_writer.back() |= (header.sn >> 8U) & 0xfU; // high part of SN (4 bit)
      hdr_writer.append(header.sn & 0xffU);          // remaining 8 bit SN
    }
    if (header.so != 0) {
      // write SO
      hdr_writer.append((header.so) >> 8U); // first part of SO
      hdr_writer.append(header.so & 0xffU); // second part of SO
    }
  }
  pdu.chain_before(std::move(hdr_buf));
  return true;
}

} // namespace srsgnb

#endif // SRSGNB_LIB_RLC_UM_PDU_H
