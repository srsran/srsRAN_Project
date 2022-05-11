/**
 *
 * \section COPYRIGHT
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
#include "ul_bsr.h"

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

std::ostream& srsgnb::operator<<(std::ostream& os, const srsgnb::mac_ul_sch_subpdu& subpdu)
{
  using namespace srsgnb;
  switch (subpdu.lcid().value()) {
    case lcid_ul_sch_t::CCCH_SIZE_48:
      fmt::print(os, "CCCH48: len={}", subpdu.sdu_length());
      break;
    case lcid_ul_sch_t::CCCH_SIZE_64:
      fmt::print(os, "CCCH64: len={}", subpdu.sdu_length());
      break;
    case lcid_ul_sch_t::CRNTI:
      fmt::print(os, "C-RNTI: {:#04x}", decode_crnti_ce(subpdu.payload()));
      break;
    case lcid_ul_sch_t::SHORT_TRUNC_BSR: {
      lcg_bsr_report sbsr = decode_sbsr(subpdu.payload());
      fmt::print(os, "SHORT_TRUNC_BSR: len={} bs={}", subpdu.total_length(), sbsr.buffer_size);
      break;
    }
    case lcid_ul_sch_t::LONG_TRUNC_BSR:
      fmt::print(os, "LONG_TRUNC_BSR: len={}", subpdu.total_length());
      break;
    case lcid_ul_sch_t::SHORT_BSR: {
      lcg_bsr_report sbsr = decode_sbsr(subpdu.payload());
      fmt::print(os, "SBSR: lcg={} bs={}", sbsr.lcg_id, sbsr.buffer_size);
    } break;
    case lcid_ul_sch_t::LONG_BSR: {
      long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());
      fmt::print(os, "LBSR: bitmap={:#02x} ", lbsr.bitmap);
      for (const auto& lcg : lbsr.list) {
        fmt::print(os, "lcg={} bs={} ", lcg.lcg_id, lcg.buffer_size);
      }
    } break;
    case lcid_ul_sch_t::SE_PHR:
      //      fmt::print(os, " SE_PHR: ph={} pc={}", get_phr(), get_pcmax());
      fmt::print(os, "SE_PHR: total_len={}", subpdu.total_length());
      break;
    case lcid_ul_sch_t::PADDING:
      fmt::print(os, "PAD: len={}", subpdu.sdu_length());
      break;
    default:
      fmt::print(os, "CE={} total_len={}", subpdu.lcid(), subpdu.total_length());
      break;
  }
  return os;
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

std::ostream& srsgnb::operator<<(std::ostream& os, const srsgnb::mac_ul_sch_pdu& pdu)
{
  fmt::print(os, "{}", fmt::join(pdu.begin(), pdu.end(), ", "));
  return os;
}
