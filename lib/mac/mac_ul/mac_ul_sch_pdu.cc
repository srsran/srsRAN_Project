
#include "mac_ul_sch_pdu.h"
#include "srsgnb/srslog/srslog.h"
#include "ul_bsr.h"

using namespace srsgnb;

int mac_ul_sch_subpdu::unpack(span<const uint8_t> subpdu)
{
  if (subpdu.empty()) {
    srslog::fetch_basic_logger("MAC-NR").warning("Empty MAC subPDU");
    return -1;
  }
  const uint8_t* ptr = subpdu.data();

  // Skip R, read F bit and LCID
  F_bit    = (*ptr & 0x40U) > 0;
  lcid_val = *ptr & 0x3fU;
  ptr++;
  header_length = 1;

  uint32_t sdu_length = 0;
  if (lcid_val.is_valid_lcid()) {
    if (lcid_val.has_length_field()) {
      // Read first length byte
      sdu_length = (uint32_t)*ptr;
      ptr++;
      header_length++;

      if (F_bit) {
        // add second length byte
        sdu_length = sdu_length << 8 | ((uint32_t)*ptr & 0xff);
        ptr++;
        header_length++;
      }
    } else {
      sdu_length = lcid_val.sizeof_ce();
      if (lcid_val == lcid_ul_sch_t::PADDING) {
        // set subPDU length to rest of PDU
        // 1 Byte R/LCID MAC subheader
        header_length = 1;
        sdu_length    = subpdu.size() - header_length;
      }
    }
    sdu_view = {ptr, (size_t)sdu_length};
  } else {
    sdu_view = {};
    srslog::fetch_basic_logger("MAC-NR").warning("Invalid LCID ({}) in MAC PDU", lcid_val);
    return -1;
  }
  return payload().size();
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
      fmt::print(os, "SBSR: lcg={} bs={}", sbsr.lcg_id, sbsr.nof_bytes);
    } break;
    case lcid_ul_sch_t::LONG_BSR: {
      long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());
      fmt::print(os, "LBSR: bitmap={:#02x}", lbsr.bitmap);
      for (const auto& lcg : lbsr.list) {
        fmt::print(os, "lcg={} bs={}", lcg.lcg_id, lcg.nof_bytes);
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

int mac_ul_sch_pdu::unpack(span<const uint8_t> payload)
{
  size_t offset = 0;
  while (offset < payload.size()) {
    mac_ul_sch_subpdu subpdu{};
    if (subpdu.unpack(payload.subspan(offset, payload.size() - offset)) < 0) {
      return -1;
    }
    offset += subpdu.total_length();
    subpdus.push_back(subpdu);
  }

  return offset;
}

std::ostream& srsgnb::operator<<(std::ostream& os, const srsgnb::mac_ul_sch_pdu& pdu)
{
  fmt::print(os, "{}", fmt::join(pdu.begin(), pdu.end(), ", "));
  return os;
}
