
#include "mac_ul_sch_pdu.h"
#include "srsgnb/srslog/srslog.h"
#include "ul_bsr.h"

using namespace srsgnb;

bool mac_ul_sch_subpdu::unpack(byte_buffer_view& subpdu)
{
  sdu_view = {};
  if (subpdu.empty()) {
    srslog::fetch_basic_logger("MAC-NR").warning("Empty MAC subPDU");
    return false;
  }

  // Skip R, read F bit and LCID
  F_bit    = (*subpdu & 0x40U) > 0;
  lcid_val = *subpdu & 0x3fU;
  ++subpdu;
  header_length = 1;

  uint32_t sdu_length = 0;
  if (lcid_val.is_valid_lcid()) {
    if (lcid_val.has_length_field()) {
      // Read first length byte
      sdu_length = (uint32_t)*subpdu;
      ++subpdu;
      header_length++;

      if (F_bit) {
        // add second length byte
        sdu_length = sdu_length << 8 | ((uint32_t)*subpdu & 0xff);
        ++subpdu;
        header_length++;
      }
      std::tie(sdu_view, subpdu) = subpdu.split((size_t)sdu_length);
    } else {
      if (lcid_val == lcid_ul_sch_t::PADDING) {
        // set subPDU length to rest of PDU
        // 1 Byte R/LCID MAC subheader
        sdu_view = subpdu;
        subpdu   = {};
      } else {
        sdu_length                 = lcid_val.sizeof_ce();
        std::tie(sdu_view, subpdu) = subpdu.split((size_t)sdu_length);
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
  switch ((lcid_t)subpdu.lcid()) {
    case lcid_ul_sch_t::CCCH_SIZE_48:
      fmt::print(os, "CCCH48: len={}", subpdu.sdu_length());
      break;
    case lcid_ul_sch_t::CCCH_SIZE_64:
      fmt::print(os, "CCCH64: len={}", subpdu.sdu_length());
      break;
    case lcid_ul_sch_t::CRNTI:
      fmt::print(os, "C-RNTI: {:#04x}", decode_crnti_ce(subpdu.payload()));
      break;
    case lcid_ul_sch_t::SHORT_TRUNC_BSR:
      fmt::print(os, "SHORT_TRUNC_BSR: len={}", subpdu.total_length());
      break;
    case lcid_ul_sch_t::LONG_TRUNC_BSR:
      fmt::print(os, "LONG_TRUNC_BSR: len={}", subpdu.total_length());
      break;
    case lcid_ul_sch_t::SHORT_BSR: {
      lcg_bsr_report sbsr = decode_sbsr(subpdu.payload());
      fmt::print(os, "SBSR: lcg={} bs={}", sbsr.lcg_id, sbsr.buffer_size);
    } break;
    case lcid_ul_sch_t::LONG_BSR: {
      long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());
      fmt::print(os, "LBSR: bitmap={:#02x}", lbsr.bitmap);
      for (const auto& lcg : lbsr.list) {
        fmt::print(os, "lcg={} bs={}", lcg.lcg_id, lcg.buffer_size);
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
  byte_buffer_view view = payload;
  while (not view.empty()) {
    mac_ul_sch_subpdu subpdu{};
    if (not subpdu.unpack(view)) {
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
