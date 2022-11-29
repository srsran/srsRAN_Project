/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lcid_ul_sch.h"
#include "ul_bsr.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/srsgnb_assert.h"

namespace srsgnb {

class mac_ul_sch_subpdu
{
public:
  /// Returns buffer view with begin() pointing at the first byte after the decoded subPDU.
  bool unpack(byte_buffer_reader& subpdu_reader);
  bool unpack(const byte_buffer& subpdu);

  lcid_ul_sch_t    lcid() const { return lcid_val; }
  uint32_t         total_length() const { return header_length + payload().length(); }
  byte_buffer_view payload() const { return payload_view; }
  uint32_t         sdu_length() const { return payload_view.length(); }

private:
  lcid_ul_sch_t    lcid_val;
  int              header_length = 0;
  bool             F_bit         = false;
  byte_buffer_view payload_view;
};

class mac_ul_sch_pdu
{
  static constexpr size_t MAX_PDU_LIST = 16;

public:
  using iterator       = static_vector<mac_ul_sch_subpdu, MAX_PDU_LIST>::iterator;
  using const_iterator = static_vector<mac_ul_sch_subpdu, MAX_PDU_LIST>::const_iterator;

  mac_ul_sch_pdu() = default;

  void clear();

  bool unpack(const byte_buffer& payload);

  mac_ul_sch_subpdu&       subpdu(size_t i) { return subpdus[i]; }
  const mac_ul_sch_subpdu& subpdu(size_t i) const { return subpdus[i]; }
  size_t                   nof_subpdus() const { return subpdus.size(); }

  iterator       begin() { return subpdus.begin(); }
  iterator       end() { return subpdus.end(); }
  const_iterator begin() const { return subpdus.begin(); }
  const_iterator end() const { return subpdus.end(); }

private:
  static_vector<mac_ul_sch_subpdu, MAX_PDU_LIST> subpdus;
};

/// Decode C-RNTI MAC CE
inline rnti_t decode_crnti_ce(byte_buffer_view payload)
{
  if (payload.length() < 2) {
    return INVALID_RNTI;
  }

  // Conversion between Little Endian to RNTI value. See TS 38.321, 6.1.3.2 - C-RNTI MAC CE.
  return to_rnti(le16toh((uint16_t)payload[0] << 8U | payload[1]));
}

} // namespace srsgnb

namespace fmt {

template <>
struct formatter<srsgnb::mac_ul_sch_subpdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::mac_ul_sch_subpdu& subpdu, FormatContext& ctx) -> decltype(ctx.out())
  {
    using namespace srsgnb;
    if (subpdu.lcid().is_sdu()) {
      format_to(ctx.out(), "LCID={}: len={}", subpdu.lcid(), subpdu.sdu_length());
      return ctx.out();
    }
    switch (subpdu.lcid().value()) {
      case lcid_ul_sch_t::CCCH_SIZE_64:
        format_to(ctx.out(), "CCCH64: len={}", subpdu.sdu_length());
        break;
      case lcid_ul_sch_t::CCCH_SIZE_48:
        format_to(ctx.out(), "CCCH48: len={}", subpdu.sdu_length());
        break;
      case lcid_ul_sch_t::CRNTI:
        format_to(ctx.out(), "C-RNTI: {:#04x}", decode_crnti_ce(subpdu.payload()));
        break;
      case lcid_ul_sch_t::SHORT_TRUNC_BSR: {
        lcg_bsr_report sbsr = decode_sbsr(subpdu.payload());
        format_to(ctx.out(), "SHORT_TRUNC_BSR: len={} bs={}", subpdu.total_length(), sbsr.buffer_size);
        break;
      }
      case lcid_ul_sch_t::LONG_TRUNC_BSR:
        format_to(ctx.out(), "LONG_TRUNC_BSR: len={}", subpdu.total_length());
        break;
      case lcid_ul_sch_t::SHORT_BSR: {
        lcg_bsr_report sbsr = decode_sbsr(subpdu.payload());
        format_to(ctx.out(), "SBSR: lcg={} bs={}", sbsr.lcg_id, sbsr.buffer_size);
        break;
      }
      case lcid_ul_sch_t::LONG_BSR: {
        long_bsr_report lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());
        format_to(ctx.out(), "LBSR: bitmap={:#02x} ", lbsr.bitmap);
        for (const auto& lcg : lbsr.list) {
          format_to(ctx.out(), "lcg={} bs={} ", lcg.lcg_id, lcg.buffer_size);
        }
        break;
      }
      case lcid_ul_sch_t::SE_PHR:
        format_to(ctx.out(), "SE_PHR: total_len={}", subpdu.total_length());
        break;
      case lcid_ul_sch_t::PADDING:
        format_to(ctx.out(), "PAD: len={}", subpdu.sdu_length());
        break;
      default:
        format_to(ctx.out(), "Unrecognized CE={} total_len={}", subpdu.lcid(), subpdu.total_length());
        break;
    }
    return ctx.out();
  }
};

template <>
struct formatter<srsgnb::mac_ul_sch_pdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(const srsgnb::mac_ul_sch_pdu& pdu, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", fmt::join(pdu.begin(), pdu.end(), ", "));
  }
};

} // namespace fmt
