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

#pragma once

#include "lcid_ul_sch.h"
#include "ul_bsr.h"
#include "ul_phr.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/rnti.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class mac_ul_sch_subpdu
{
public:
  /// \brief Unpacks a subPDU from a byte buffer reader.
  error_type<std::string> unpack(byte_buffer_reader& subpdu_reader);

  /// \brief Unpacks a subPDU from a byte buffer containing a subPDU.
  error_type<std::string> unpack(const byte_buffer& subpdu);

  /// Get LCID of the unpacked subPDU.
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
  /// Implementation-defined limit for the number of subPDUs per PDU.
  static constexpr size_t MAX_SUBPDUS_PER_PDU = 4096;

  using subpdu_list = static_vector<mac_ul_sch_subpdu, MAX_SUBPDUS_PER_PDU>;

public:
  using iterator       = subpdu_list::iterator;
  using const_iterator = subpdu_list::const_iterator;

  mac_ul_sch_pdu() = default;

  void clear();

  error_type<std::string> unpack(const byte_buffer& payload);

  mac_ul_sch_subpdu&       subpdu(size_t i) { return subpdus[i]; }
  const mac_ul_sch_subpdu& subpdu(size_t i) const { return subpdus[i]; }
  size_t                   nof_subpdus() const { return subpdus.size(); }

  iterator       begin() { return subpdus.begin(); }
  iterator       end() { return subpdus.end(); }
  const_iterator begin() const { return subpdus.begin(); }
  const_iterator end() const { return subpdus.end(); }

private:
  subpdu_list subpdus;
};

/// Decode C-RNTI MAC CE
inline rnti_t decode_crnti_ce(byte_buffer_view payload)
{
  if (payload.length() < 2) {
    return rnti_t::INVALID_RNTI;
  }

  // Conversion between Little Endian to RNTI value. See TS 38.321, 6.1.3.2 - C-RNTI MAC CE.
  return to_rnti(le16toh((uint16_t)payload[0] << 8U | payload[1]));
}

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::mac_ul_sch_subpdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::mac_ul_sch_subpdu& subpdu, FormatContext& ctx) -> decltype(ctx.out())
  {
    using namespace srsran;
    if (subpdu.lcid().is_sdu()) {
      format_to(ctx.out(), "lcid={}: len={}", subpdu.lcid(), subpdu.sdu_length());
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
        format_to(ctx.out(), "C-RNTI: {:#x}", to_value(decode_crnti_ce(subpdu.payload())));
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
        expected<long_bsr_report> lbsr = decode_lbsr(bsr_format::LONG_BSR, subpdu.payload());
        if (lbsr.has_value()) {
          format_to(ctx.out(), "LBSR: ");
          for (const auto& lcg : lbsr.value().list) {
            format_to(ctx.out(), "lcg={} bs={} ", lcg.lcg_id, lcg.buffer_size);
          }
        } else {
          format_to(ctx.out(), "LBSR: invalid");
        }
        break;
      }
      case lcid_ul_sch_t::SE_PHR: {
        const phr_report phr = decode_se_phr(subpdu.payload());
        format_to(ctx.out(), "SE_PHR: ph={}dB p_cmax={}dBm", phr.get_se_phr().ph, phr.get_se_phr().p_cmax);
        break;
      }
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
struct formatter<srsran::mac_ul_sch_pdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(const srsran::mac_ul_sch_pdu& pdu, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", fmt::join(pdu.begin(), pdu.end(), ", "));
  }
};

} // namespace fmt
