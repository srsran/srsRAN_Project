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

#include "rlc_am_pdu.h"

using namespace srsran;

/****************************************************************************
 * Container implementation for pack/unpack functions
 ***************************************************************************/

const uint16_t rlc_am_status_nack::so_end_of_sdu = 0xffff;

rlc_am_status_pdu::rlc_am_status_pdu(rlc_am_sn_size sn_size_) :
  sn_size(sn_size_), mod_nr(cardinality(to_number(sn_size_)))
{
  nacks.reserve(RLC_AM_NR_TYP_NACKS);
}

void rlc_am_status_pdu::reset()
{
  cpt    = rlc_control_pdu_type::status_pdu;
  ack_sn = INVALID_RLC_SN;
  nacks.clear();
  packed_size = rlc_am_nr_status_pdu_sizeof_header_ack_sn;
}

bool rlc_am_status_pdu::is_continuous_sequence(const rlc_am_status_nack& left, const rlc_am_status_nack& right) const
{
  // SN must be continuous
  if (right.nack_sn != ((left.has_nack_range ? left.nack_sn + left.nack_range : (left.nack_sn + 1)) % mod_nr)) {
    return false;
  }

  // Segments on left side (if present) must reach the end of sdu
  if (left.has_so && left.so_end != rlc_am_status_nack::so_end_of_sdu) {
    return false;
  }

  // Segments on right side (if present) must start from the beginning
  if (right.has_so && right.so_start != 0) {
    return false;
  }

  return true;
}

void rlc_am_status_pdu::push_nack(const rlc_am_status_nack& nack)
{
  if (nacks.size() == 0) {
    nacks.push_back(nack);
    packed_size += nack_size(nack);
    return;
  }

  rlc_am_status_nack& prev = nacks.back();
  if (is_continuous_sequence(prev, nack) == false) {
    nacks.push_back(nack);
    packed_size += nack_size(nack);
    return;
  }

  // expand previous NACK
  // subtract size of previous NACK (add updated size later)
  packed_size -= nack_size(prev);

  // enable and update NACK range
  if (nack.has_nack_range == true) {
    if (prev.has_nack_range == true) {
      // [NACK range][NACK range]
      prev.nack_range += nack.nack_range;
    } else {
      // [NACK SDU][NACK range]
      prev.nack_range     = nack.nack_range + 1;
      prev.has_nack_range = true;
    }
  } else {
    if (prev.has_nack_range == true) {
      // [NACK range][NACK SDU]
      prev.nack_range++;
    } else {
      // [NACK SDU][NACK SDU]
      prev.nack_range     = 2;
      prev.has_nack_range = true;
    }
  }

  // enable and update segment offsets (if required)
  if (nack.has_so == true) {
    if (prev.has_so == false) {
      // [NACK SDU][NACK segm]
      prev.has_so   = true;
      prev.so_start = 0;
    }
    // [NACK SDU][NACK segm] or [NACK segm][NACK segm]
    prev.so_end = nack.so_end;
  } else {
    if (prev.has_so == true) {
      // [NACK segm][NACK SDU]
      prev.so_end = rlc_am_status_nack::so_end_of_sdu;
    }
    // [NACK segm][NACK SDU] or [NACK SDU][NACK SDU]
  }

  // add updated size
  packed_size += nack_size(prev);
}

bool rlc_am_status_pdu::trim(uint32_t max_packed_size)
{
  if (max_packed_size >= packed_size) {
    // no trimming required
    return true;
  }
  if (max_packed_size < rlc_am_nr_status_pdu_sizeof_header_ack_sn) {
    // too little space for smallest possible status PDU (only header + ACK).
    return false;
  }

  // remove NACKs (starting from the back) until it fits into given space
  // note: when removing a NACK for a segment, we have to remove all other NACKs with the same SN as well,
  // see TS 38.322 Sec. 5.3.4:
  //   "set the ACK_SN to the SN of the next not received RLC SDU
  //   which is not indicated as missing in the resulting STATUS PDU."
  while (nacks.size() > 0 && (max_packed_size < packed_size || nacks.back().nack_sn == ack_sn)) {
    packed_size -= nack_size(nacks.back());
    ack_sn = nacks.back().nack_sn;
    nacks.pop_back();
  }
  return true;
}

void rlc_am_status_pdu::refresh_packed_size()
{
  packed_size = rlc_am_nr_status_pdu_sizeof_header_ack_sn;
  for (auto nack : nacks) {
    packed_size += nack_size(nack);
  }
}

uint32_t rlc_am_status_pdu::nack_size(const rlc_am_status_nack& nack) const
{
  uint32_t result = sn_size == rlc_am_sn_size::size12bits ? rlc_am_nr_status_pdu_sizeof_nack_sn_ext_12bit_sn
                                                          : rlc_am_nr_status_pdu_sizeof_nack_sn_ext_18bit_sn;
  if (nack.has_so) {
    result += rlc_am_nr_status_pdu_sizeof_nack_so;
  }
  if (nack.has_nack_range) {
    result += rlc_am_nr_status_pdu_sizeof_nack_range;
  }
  return result;
}

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.4
 ***************************************************************************/

// TODO - move implementation from header to this place

/****************************************************************************
 * Status PDU pack/unpack functions
 * Ref: 3GPP TS 38.322 v16.2.0 Section 6.2.2.5
 ***************************************************************************/

bool rlc_am_status_pdu::is_control_pdu(const byte_buffer_view& pdu)
{
  return ((*(pdu.begin()) >> 7U) & 0x01U) == to_number(rlc_dc_field::control);
}

bool rlc_am_status_pdu::unpack_12bit(const byte_buffer_view& pdu)
{
  byte_buffer_reader pdu_reader = pdu;
  if (pdu_reader.length() < rlc_am_nr_status_pdu_sizeof_header_ack_sn) {
    srslog::fetch_basic_logger("RLC").warning("Cannot unpack status PDU, input too short. rem_len={} hdr_ack_sn_len={}",
                                              pdu_reader.length(),
                                              rlc_am_nr_status_pdu_sizeof_header_ack_sn);
    return false;
  }

  // sanity check: is control PDU
  if (not is_control_pdu(pdu)) {
    srslog::fetch_basic_logger("RLC").warning("Cannot unpack status PDU, input is not a status PDU.");
    return false;
  }

  reset();

  // fixed part
  cpt = (rlc_control_pdu_type)((*pdu_reader >> 4) & 0x07); // 3 bits CPT

  // sanity check: is control PDU type (CPT) supported?
  if (cpt != rlc_control_pdu_type::status_pdu) {
    srslog::fetch_basic_logger("RLC").warning("Unsupported control PDU type, reserved bits are set. cpt={}", cpt);
    return false;
  }

  ack_sn = (*pdu_reader & 0x0f) << 8; // first 4 bits SN
  ++pdu_reader;

  ack_sn |= (*pdu_reader & 0xff); // last 8 bits SN
  ++pdu_reader;

  // read E1 flag
  uint8_t e1 = *pdu_reader & 0x80;

  // sanity check for reserved bits
  if ((*pdu_reader & 0x7f) != 0) {
    srslog::fetch_basic_logger("RLC").warning("Malformed status PDU, reserved bits are set in 3rd byte.");
    return false;
  }

  // all good, continue with next byte depending on E1
  ++pdu_reader;
  while (e1 != 0) {
    // check remaining buffer size
    if (pdu_reader.length() < rlc_am_nr_status_pdu_sizeof_nack_sn_ext_12bit_sn) {
      srslog::fetch_basic_logger("RLC").warning(
          "Cannot unpack NACK SN, input too short. rem_len={}, hdr_nack_sn_len={}",
          pdu_reader.length(),
          rlc_am_nr_status_pdu_sizeof_nack_sn_ext_12bit_sn);
      return false;
    }

    // E1 flag set, read a NACK_SN
    rlc_am_status_nack nack = {};
    nack.nack_sn            = (*pdu_reader & 0xff) << 4;
    ++pdu_reader;

    e1         = *pdu_reader & 0x08; // 1 = further NACKs follow
    uint8_t e2 = *pdu_reader & 0x04; // 1 = set of {so_start, so_end} follows
    uint8_t e3 = *pdu_reader & 0x02; // 1 = NACK range follows (i.e. NACK across multiple SNs)

    // sanity check for reserved bits
    if ((*pdu_reader & 0x01) != 0) {
      srslog::fetch_basic_logger("RLC").warning("Malformed NACK, reserved bits are set.");
      return false;
    }
    nack.nack_sn |= (*pdu_reader & 0xf0) >> 4;

    ++pdu_reader;
    if (e2 != 0) {
      // check remaining buffer size
      if (pdu_reader.length() < rlc_am_nr_status_pdu_sizeof_nack_so) {
        srslog::fetch_basic_logger("RLC").warning(
            "Cannot unpack NACK SO, input too short. rem_len={} hdr_nack_so_len={}",
            pdu_reader.length(),
            rlc_am_nr_status_pdu_sizeof_nack_so);
        return false;
      }
      nack.has_so   = true;
      nack.so_start = (*pdu_reader) << 8;
      ++pdu_reader;
      nack.so_start |= (*pdu_reader);
      ++pdu_reader;
      nack.so_end = (*pdu_reader) << 8;
      ++pdu_reader;
      nack.so_end |= (*pdu_reader);
      ++pdu_reader;
    }
    if (e3 != 0) {
      // check remaining buffer size
      if (pdu_reader.length() < rlc_am_nr_status_pdu_sizeof_nack_range) {
        srslog::fetch_basic_logger("RLC").warning(
            "Cannot unpack NACK range: input too short. rem_len={} hdr_nack_range_len={}",
            pdu_reader.length(),
            rlc_am_nr_status_pdu_sizeof_nack_range);
        return false;
      }
      nack.has_nack_range = true;
      nack.nack_range     = (*pdu_reader);
      ++pdu_reader;
    }
    push_nack(nack);
  }

  return true;
}

bool rlc_am_status_pdu::unpack_18bit(const byte_buffer_view& pdu)
{
  byte_buffer_reader pdu_reader = pdu;
  if (pdu_reader.length() < rlc_am_nr_status_pdu_sizeof_header_ack_sn) {
    srslog::fetch_basic_logger("RLC").warning("Cannot unpack status PDU, input too short. rem_len={} hdr_ack_sn_len={}",
                                              pdu_reader.length(),
                                              rlc_am_nr_status_pdu_sizeof_header_ack_sn);
    return false;
  }

  // sanity check: is control PDU
  if (not is_control_pdu(pdu)) {
    srslog::fetch_basic_logger("RLC").warning("Cannot unpack status PDU, input is not a status PDU.");
    return false;
  }

  reset();

  // fixed part
  cpt = (rlc_control_pdu_type)((*pdu_reader >> 4) & 0x07); // 3 bits CPT

  // sanity check: is control PDU type (CPT) supported?
  if (cpt != rlc_control_pdu_type::status_pdu) {
    srslog::fetch_basic_logger("RLC").warning("Unsupported control PDU type, reserved bits are set. cpt={}", cpt);
    return false;
  }

  ack_sn = (*pdu_reader & 0x0f) << 14; // upper 4 bits of SN
  ++pdu_reader;

  ack_sn |= (*pdu_reader & 0xff) << 6; // center 8 bits of SN
  ++pdu_reader;

  ack_sn |= (*pdu_reader & 0xfc) >> 2; // lower 6 bits of SN

  // read E1 flag
  uint8_t e1 = *pdu_reader & 0x02;

  // sanity check for reserved bits
  if ((*pdu_reader & 0x01) != 0) {
    srslog::fetch_basic_logger("RLC").warning("Malformed status PDU, reserved bits are set in 3rd byte.");
    return false;
  }

  // all good, continue with next byte depending on E1
  ++pdu_reader;
  while (e1 != 0) {
    // check remaining buffer size
    if (pdu_reader.length() < rlc_am_nr_status_pdu_sizeof_nack_sn_ext_18bit_sn) {
      srslog::fetch_basic_logger("RLC").warning(
          "Cannot unpack NACK SN, input too short. rem_len={}, hdr_nack_sn_len={}",
          pdu_reader.length(),
          rlc_am_nr_status_pdu_sizeof_nack_sn_ext_18bit_sn);
      return false;
    }

    // E1 flag set, read a NACK_SN
    rlc_am_status_nack nack = {};

    nack.nack_sn = (*pdu_reader & 0xff) << 10; // upper 8 bits of SN
    ++pdu_reader;
    nack.nack_sn |= (*pdu_reader & 0xff) << 2; // center 8 bits of SN
    ++pdu_reader;
    nack.nack_sn |= (*pdu_reader & 0xc0) >> 6; // lower 2 bits of SN

    e1         = *pdu_reader & 0x20; // 1 = further NACKs follow
    uint8_t e2 = *pdu_reader & 0x10; // 1 = set of {so_start, so_end} follows
    uint8_t e3 = *pdu_reader & 0x08; // 1 = NACK range follows (i.e. NACK across multiple SNs)

    // sanity check for reserved bits
    if ((*pdu_reader & 0x07) != 0) {
      srslog::fetch_basic_logger("RLC").warning("Malformed NACK, reserved bits are set.");
      return false;
    }

    ++pdu_reader;
    if (e2 != 0) {
      // check remaining buffer size
      if (pdu_reader.length() < rlc_am_nr_status_pdu_sizeof_nack_so) {
        srslog::fetch_basic_logger("RLC").warning(
            "Cannot unpack NACK SO, input too short. rem_len={} hdr_nack_so_len={}",
            pdu_reader.length(),
            rlc_am_nr_status_pdu_sizeof_nack_so);
        return false;
      }
      nack.has_so   = true;
      nack.so_start = (*pdu_reader) << 8;
      ++pdu_reader;
      nack.so_start |= (*pdu_reader);
      ++pdu_reader;
      nack.so_end = (*pdu_reader) << 8;
      ++pdu_reader;
      nack.so_end |= (*pdu_reader);
      ++pdu_reader;
    }
    if (e3 != 0) {
      // check remaining buffer size
      if (pdu_reader.length() < rlc_am_nr_status_pdu_sizeof_nack_range) {
        srslog::fetch_basic_logger("RLC").warning(
            "Cannot unpack NACK range: input too short. rem_len={} hdr_nack_range_len={}",
            pdu_reader.length(),
            rlc_am_nr_status_pdu_sizeof_nack_range);
        return false;
      }
      nack.has_nack_range = true;
      nack.nack_range     = (*pdu_reader);
      ++pdu_reader;
    }
    push_nack(nack);
  }

  return true;
}

bool rlc_am_status_pdu::unpack(const byte_buffer_view& pdu)
{
  switch (sn_size) {
    case rlc_am_sn_size::size12bits:
      return unpack_12bit(pdu);
      break;
    case rlc_am_sn_size::size18bits:
      return unpack_18bit(pdu);
      break;
  }

  return false;
}

bool rlc_am_status_pdu::pack_12bit(byte_buffer& pdu) const
{
  byte_buffer        hdr_buf;
  byte_buffer_writer hdr_writer = hdr_buf;

  // fixed header part
  hdr_writer.append(0U); // 1 bit D/C field and 3 bit CPT are all zero

  // write first 4 bit of ACK_SN
  hdr_writer.back() |= (ack_sn >> 8U) & 0x0fU; // 4 bits ACK_SN
  hdr_writer.append(ack_sn & 0xffU);           // remaining 8 bits of SN

  // write E1 flag in octet 3
  if (nacks.size() > 0) {
    hdr_writer.append(0x80U);
  } else {
    hdr_writer.append(0x00U);
  }

  if (nacks.size() > 0) {
    for (uint32_t i = 0; i < nacks.size(); i++) {
      hdr_writer.append((nacks[i].nack_sn >> 4U) & 0xffU); // upper 8 bits of SN
      hdr_writer.append((nacks[i].nack_sn & 0x0fU) << 4U); // lower 4 bits of SN

      if (i < (uint32_t)(nacks.size() - 1)) {
        hdr_writer.back() |= 0x08U; // Set E1
      }
      if (nacks[i].has_so) {
        hdr_writer.back() |= 0x04U; // Set E2
      }
      if (nacks[i].has_nack_range) {
        hdr_writer.back() |= 0x02U; // Set E3
      }

      if (nacks[i].has_so) {
        hdr_writer.append(nacks[i].so_start >> 8U);
        hdr_writer.append(nacks[i].so_start);
        hdr_writer.append(nacks[i].so_end >> 8U);
        hdr_writer.append(nacks[i].so_end);
      }
      if (nacks[i].has_nack_range) {
        hdr_writer.append(nacks[i].nack_range);
      }
    }
  }
  pdu.prepend(std::move(hdr_buf));
  return true;
}

bool rlc_am_status_pdu::pack_18bit(byte_buffer& pdu) const
{
  byte_buffer        hdr_buf;
  byte_buffer_writer hdr_writer = hdr_buf;

  // fixed header part
  hdr_writer.append(0U); // 1 bit D/C field and 3 bit CPT are all zero

  hdr_writer.back() |= (ack_sn >> 14U) & 0x0fU; // upper 4 bits of SN
  hdr_writer.append((ack_sn >> 6U) & 0xffU);    // center 8 bits of SN
  hdr_writer.append((ack_sn << 2U) & 0xfcU);    // lower 6 bits of SN

  // set E1 flag if necessary
  if (nacks.size() > 0) {
    hdr_writer.back() |= 0x02;
  }

  if (nacks.size() > 0) {
    for (uint32_t i = 0; i < nacks.size(); i++) {
      hdr_writer.append((nacks[i].nack_sn >> 10) & 0xffU); // upper 8 bits of SN
      hdr_writer.append((nacks[i].nack_sn >> 2) & 0xffU);  // center 8 bits of SN
      hdr_writer.append((nacks[i].nack_sn << 6) & 0xc0U);  // lower 2 bits of SN

      if (i < (uint32_t)(nacks.size() - 1)) {
        hdr_writer.back() |= 0x20U; // Set E1
      }
      if (nacks[i].has_so) {
        hdr_writer.back() |= 0x10U; // Set E2
      }
      if (nacks[i].has_nack_range) {
        hdr_writer.back() |= 0x08U; // Set E3
      }

      if (nacks[i].has_so) {
        hdr_writer.append(nacks[i].so_start >> 8);
        hdr_writer.append(nacks[i].so_start);
        hdr_writer.append(nacks[i].so_end >> 8);
        hdr_writer.append(nacks[i].so_end);
      }
      if (nacks[i].has_nack_range) {
        hdr_writer.append(nacks[i].nack_range);
      }
    }
  }

  pdu.prepend(std::move(hdr_buf));
  return true;
}

bool rlc_am_status_pdu::pack(byte_buffer& pdu) const
{
  switch (sn_size) {
    case rlc_am_sn_size::size12bits:
      return pack_12bit(pdu);
      break;
    case rlc_am_sn_size::size18bits:
      return pack_18bit(pdu);
      break;
  }

  return false;
}
