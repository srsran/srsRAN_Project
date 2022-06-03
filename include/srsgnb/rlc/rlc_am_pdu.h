/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSGNB_RLC_AM_PDU_H
#define SRSGNB_RLC_AM_PDU_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/rlc/rlc_types.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

struct rlc_am_pdu_header {
  rlc_dc_field   dc;      ///< Data/Control (D/C) field
  uint8_t        p;       ///< Polling bit
  rlc_si_field   si;      ///< Segmentation info
  rlc_am_sn_size sn_size; ///< Sequence number size (12 or 18 bits)
  uint32_t       sn;      ///< Sequence number
  uint16_t       so;      ///< Sequence offset
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.4
 ***************************************************************************/
inline bool rlc_am_read_data_pdu_header(const byte_buffer& pdu, const rlc_am_sn_size sn_size, rlc_am_pdu_header* header)
{
  byte_buffer_reader pdu_reader = pdu;
  if (pdu_reader.empty()) {
    srslog::fetch_basic_logger("RLC").warning("Unpacking header of empty RLC PDU");
    return false;
  }

  header->sn_size = sn_size;

  // Fixed part
  header->dc = (rlc_dc_field)((*pdu_reader >> 7U) & 0x01U); // 1 bit D/C field
  header->p  = (*pdu_reader >> 6U) & 0x01U;                 // 1 bit P flag
  header->si = (rlc_si_field)((*pdu_reader >> 4U) & 0x03U); // 2 bits SI

  if (sn_size == rlc_am_sn_size::size12bits) {
    header->sn = (*pdu_reader & 0x0FU) << 8U; // first 4 bits SN
    ++pdu_reader;

    header->sn |= (*pdu_reader & 0xFFU); // last 8 bits SN
    ++pdu_reader;
  } else if (sn_size == rlc_am_sn_size::size18bits) {
    // sanity check
    if ((*pdu_reader & 0x0CU) != 0) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, reserved bits are set");
      return false;
    }
    header->sn = (*pdu_reader & 0x03U) << 16U; // first 4 bits SN
    ++pdu_reader;
    header->sn |= (*pdu_reader & 0xFFU) << 8U; // bit 2-10 of SN
    ++pdu_reader;
    header->sn |= (*pdu_reader & 0xFFU); // last 8 bits SN
    ++pdu_reader;
  } else {
    srslog::fetch_basic_logger("RLC").error("Unsupported SN length");
    return false;
  }

  // Read optional part
  if (header->si == rlc_si_field::last_segment || header->si == rlc_si_field::neither_first_nor_last_segment) {
    // read SO
    header->so = (*pdu_reader & 0xFFU) << 8U;
    ++pdu_reader;
    header->so |= (*pdu_reader & 0xFFU);
    ++pdu_reader;
  }
  return true;
}

inline bool rlc_am_write_data_pdu_header(const rlc_am_pdu_header& header, byte_buffer& pdu)
{
  byte_buffer        hdr_buf;
  byte_buffer_writer hdr_writer = hdr_buf;

  // fixed header part
  hdr_writer.append((to_number(header.dc) & 0x01U) << 7U);   // 1 bit D/C field
  hdr_writer.back() |= (header.p & 0x01U) << 6U;             // 1 bit P flag
  hdr_writer.back() |= (to_number(header.si) & 0x03U) << 4U; // 2 bits SI

  if (header.sn_size == rlc_am_sn_size::size12bits) {
    // write first 4 bit of SN
    hdr_writer.back() |= (header.sn >> 8U) & 0x0fU; // 4 bit SN
    hdr_writer.append(header.sn & 0xffU);           // remaining 8 bit of SN
  } else {
    // 18bit SN
    hdr_writer.back() |= (header.sn >> 16U) & 0x3U; // 2 bit SN
    hdr_writer.append(header.sn >> 8U);             // bit 3 - 10 of SN
    hdr_writer.append(header.sn & 0xffU);           // remaining 8 bit of SN
  }

  if (header.so != 0) {
    // write SO
    hdr_writer.append(header.so >> 8U);   // first part of SO
    hdr_writer.append(header.so & 0xffU); // second part of SO
  }
  pdu.chain_before(std::move(hdr_buf));
  return true;
}
} // namespace srsgnb
#endif
