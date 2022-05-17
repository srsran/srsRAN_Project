/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_MAC_UL_SCH_PDU_H
#define SRSGNB_MAC_UL_SCH_PDU_H

#include "lcid_ul_sch.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include "srsgnb/support/srsran_assert.h"

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

/// UL subPDU Formatter
std::ostream& operator<<(std::ostream& os, const srsgnb::mac_ul_sch_subpdu& subpdu);

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

/// UL PDU Formatter
std::ostream& operator<<(std::ostream& os, const srsgnb::mac_ul_sch_pdu& subpdu);

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

/// fmt::join doesn't work for operator<< user types. See https://github.com/fmtlib/fmt/issues/1283
template <typename Char>
struct formatter<srsgnb::mac_ul_sch_subpdu, Char> : detail::fallback_formatter<srsgnb::mac_ul_sch_subpdu, Char> {};

} // namespace fmt

#endif // SRSGNB_MAC_UL_SCH_PDU_H
