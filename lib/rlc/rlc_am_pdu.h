/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/support/format/fmt_to_c_str.h"
#include "fmt/format.h"

namespace srsran {

const uint32_t INVALID_RLC_SN         = 0xffffffff; ///< Reserved number representing an invalid RLC sequence number
const uint32_t RETX_COUNT_NOT_STARTED = 0xffffffff; ///< Reserved number representing that RETX has not started

constexpr uint32_t rlc_am_nr_status_pdu_sizeof_header_ack_sn        = 3; ///< Header fixed part and ACK SN
constexpr uint32_t rlc_am_nr_status_pdu_sizeof_nack_sn_ext_12bit_sn = 2; ///< NACK SN and extension fields (12 bit SN)
constexpr uint32_t rlc_am_nr_status_pdu_sizeof_nack_sn_ext_18bit_sn = 3; ///< NACK SN and extension fields (18 bit SN)
constexpr uint32_t rlc_am_nr_status_pdu_sizeof_nack_so              = 4; ///< NACK segment offsets (start and end)
constexpr uint32_t rlc_am_nr_status_pdu_sizeof_nack_range           = 1; ///< NACK range (nof consecutively lost SDUs)

/// Expected number of NACKs in status PDU before needing to realloc
constexpr uint32_t RLC_AM_NR_TYP_NACKS = 512;

constexpr size_t rlc_am_pdu_header_so_size        = 2;
constexpr size_t rlc_am_pdu_header_min_size_12bit = 2;
constexpr size_t rlc_am_pdu_header_min_size_18bit = 3;
constexpr size_t rlc_am_pdu_header_min_size(rlc_am_sn_size sn_size)
{
  switch (sn_size) {
    case rlc_am_sn_size::size12bits:
      return rlc_am_pdu_header_min_size_12bit;
    case rlc_am_sn_size::size18bits:
      return rlc_am_pdu_header_min_size_18bit;
  }
  srsran_assertion_failure("Cannot determine RLC AM PDU header minimum size: unsupported sn_size={}.",
                           to_number(sn_size));
  return rlc_am_pdu_header_min_size_12bit;
}
constexpr size_t rlc_am_pdu_header_max_size(rlc_am_sn_size sn_size)
{
  switch (sn_size) {
    case rlc_am_sn_size::size12bits:
    case rlc_am_sn_size::size18bits:
      return rlc_am_pdu_header_min_size(sn_size) + rlc_am_pdu_header_so_size;
  }
  srsran_assertion_failure("Cannot determine RLC AM PDU header maximum size: unsupported sn_size={}.",
                           to_number(sn_size));
  return rlc_am_pdu_header_min_size_12bit + rlc_am_pdu_header_so_size;
}

/// AM PDU header
struct rlc_am_pdu_header {
  rlc_dc_field   dc;      ///< Data/Control (D/C) field
  uint8_t        p;       ///< Polling bit
  rlc_si_field   si;      ///< Segmentation info
  rlc_am_sn_size sn_size; ///< Sequence number size (12 or 18 bits)
  uint32_t       sn;      ///< Sequence number
  uint16_t       so;      ///< Sequence offset
  uint32_t       get_packed_size() const
  {
    if (si == rlc_si_field::last_segment || si == rlc_si_field::middle_segment) {
      return rlc_am_pdu_header_max_size(sn_size);
    }
    return rlc_am_pdu_header_min_size(sn_size);
    ;
  }
};

/// Status PDU NACK
struct rlc_am_status_nack {
  static const uint16_t so_end_of_sdu;

  uint32_t nack_sn;        ///< Sequence Number (SN) of first missing SDU
  bool     has_so;         ///< NACKs continuous sequence of bytes [so_start..so_end]
  uint16_t so_start;       ///< First missing byte in SDU with SN=nack_sn
  uint16_t so_end;         ///< Last missing byte in SDU with SN=nack_sn or SN=nack_sn+nack_range-1 if has_nack_range.
  bool     has_nack_range; ///< NACKs continuous sequence of SDUs
  uint8_t  nack_range;     ///< Number of SDUs being NACKed (including SN=nack_sn)

  rlc_am_status_nack()
  {
    has_so         = false;
    nack_sn        = 0;
    so_start       = 0;
    so_end         = so_end_of_sdu;
    has_nack_range = false;
    nack_range     = 0;
  }

  bool equals(const rlc_am_status_nack& other) const
  {
    return nack_sn == other.nack_sn && has_so == other.has_so && so_start == other.so_start && so_end == other.so_end &&
           has_nack_range == other.has_nack_range && nack_range == other.nack_range;
  }

  bool operator==(const rlc_am_status_nack& other) const { return equals(other); }
  bool operator!=(const rlc_am_status_nack& other) const { return not equals(other); }
};

/// AM Status PDU
class rlc_am_status_pdu
{
private:
  /// Stored SN size required to compute the packed size
  rlc_am_sn_size sn_size = rlc_am_sn_size::size12bits;
  /// Stored modulus to determine continuous sequences across SN overflows
  uint32_t mod_nr = cardinality(to_number(rlc_am_sn_size::size12bits));
  /// Internal NACK container; keep in sync with packed_size
  std::vector<rlc_am_status_nack> nacks = {};
  /// Stores the current packed size; sync on each change of nacks
  uint32_t packed_size = rlc_am_nr_status_pdu_sizeof_header_ack_sn;

  void     refresh_packed_size();
  uint32_t nack_size(const rlc_am_status_nack& nack) const;
  size_t   pack_12bit(span<uint8_t> buf) const;
  size_t   pack_18bit(span<uint8_t> buf) const;
  bool     unpack_12bit(const byte_buffer_view& pdu);
  bool     unpack_18bit(const byte_buffer_view& pdu);

public:
  /// Control PDU type
  rlc_control_pdu_type cpt = rlc_control_pdu_type::status_pdu;
  /// SN of the next not received RLC Data PDU
  uint32_t ack_sn = INVALID_RLC_SN;

  rlc_am_status_pdu(rlc_am_sn_size sn_size_);
  /// Explicit copy ctor to avoid implicit copies.
  explicit rlc_am_status_pdu(const rlc_am_status_pdu& other) = default;
  /// Default move ctor.
  rlc_am_status_pdu(rlc_am_status_pdu&& other) = default;
  /// Copy assignment is disabled. Use std::move, or explicit copy instead.
  rlc_am_status_pdu& operator=(const rlc_am_status_pdu&) = delete;
  /// Move assignment of byte_buffer.
  rlc_am_status_pdu& operator=(rlc_am_status_pdu&& other) = default;

  void reset();
  /// \brief Checks whether the two NACKs can be merged into one NACK.
  ///
  /// This first checks whether both NACKs form as continuous sequence and then also validates
  /// that the resulting range does not exceed the maximum length of a NACK range (which is 255).
  ///
  /// \param left The NACK on the left-hand side.
  /// \param right The NACK on the right-hand side.
  /// \return True if both NACKs can be merged into one. False if not.
  bool can_be_merged(const rlc_am_status_nack& left, const rlc_am_status_nack& right) const;
  void push_nack(const rlc_am_status_nack& nack);
  const std::vector<rlc_am_status_nack>& get_nacks() const { return nacks; }
  uint32_t                               get_packed_size() const { return packed_size; }
  bool                                   trim(uint32_t max_packed_size);

  /// \brief Write the RLC AM status PDU to a PDU buffer and meets the length of the generate PDU accordingly.
  /// \param[out] buf Buffer where to encode the status PDU. The encoded PDU size cannot exceed the size of the buffer.
  /// \return Number of bytes taken by the written status PDU.
  size_t pack(span<uint8_t> buf) const;

  /// \brief Read a RLC AM status PDU from a PDU buffer view
  /// \param pdu A reference to a byte_buffer_view
  /// \return true if PDU was read successfully, false otherwise
  [[nodiscard]] bool unpack(const byte_buffer_view& pdu);

  /// \brief Checks if a PDU buffer view contains a control PDU
  /// \param pdu A reference to a byte_buffer_view
  /// \return true if the byte buffer contains a control PDU, false otherwise
  static bool is_control_pdu(const byte_buffer_view& pdu);
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.4
 ***************************************************************************/
[[nodiscard]] inline bool
rlc_am_read_data_pdu_header(const byte_buffer_view& pdu, const rlc_am_sn_size sn_size, rlc_am_pdu_header* header)
{
  byte_buffer_reader pdu_reader = pdu;
  if (pdu_reader.length() <= rlc_am_pdu_header_min_size(sn_size)) {
    srslog::fetch_basic_logger("RLC").warning(
        "AMD PDU too small. pdu_len={} hdr_len={}", pdu.length(), rlc_am_pdu_header_min_size(sn_size));
    return false;
  }

  header->sn_size = sn_size;

  // Fixed part
  header->dc = (rlc_dc_field)((*pdu_reader >> 7U) & 0x01U); // 1 bit D/C field
  header->p  = (*pdu_reader >> 6U) & 0x01U;                 // 1 bit P flag
  header->si = (rlc_si_field)((*pdu_reader >> 4U) & 0x03U); // 2 bits SI

  if (sn_size == rlc_am_sn_size::size12bits) {
    header->sn = (*pdu_reader & 0x0fU) << 8U; // first 4 bits SN
    ++pdu_reader;
    if (pdu_reader.empty()) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, missing lower byte of SN.");
      return false;
    }
    header->sn |= (*pdu_reader & 0xffU); // last 8 bits SN
    ++pdu_reader;
  } else if (sn_size == rlc_am_sn_size::size18bits) {
    // sanity check
    if ((*pdu_reader & 0x0cU) != 0) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, reserved bits are set.");
      return false;
    }
    header->sn = (*pdu_reader & 0x03U) << 16U; // first 4 bits SN
    ++pdu_reader;
    if (pdu_reader.empty()) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, missing center byte of SN.");
      return false;
    }
    header->sn |= (*pdu_reader & 0xffU) << 8U; // bit 2-10 of SN
    ++pdu_reader;
    if (pdu_reader.empty()) {
      srslog::fetch_basic_logger("RLC").error("Malformed PDU, missing lower byte of SN.");
      return false;
    }
    header->sn |= (*pdu_reader & 0xffU); // last 8 bits SN
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

inline size_t rlc_am_write_data_pdu_header(span<uint8_t> buf, const rlc_am_pdu_header& header)
{
  span<uint8_t>::iterator buf_it = buf.begin();

  // fixed header part
  *buf_it = (to_number(header.dc) & 0x01U) << 7U; // 1 bit D/C field

  *buf_it |= (header.p & 0x01U) << 6U;             // 1 bit P flag
  *buf_it |= (to_number(header.si) & 0x03U) << 4U; // 2 bits SI

  if (header.sn_size == rlc_am_sn_size::size12bits) {
    // 12-bit SN
    *buf_it |= (header.sn >> 8U) & 0x0fU; // upper 4 bits of SN
    ++buf_it;
    *buf_it = header.sn & 0xffU; // remaining 8 bits of SN
  } else {
    // 18-bit SN
    *buf_it |= (header.sn >> 16U) & 0x3U; // upper 2 bits of SN
    ++buf_it;
    *buf_it = header.sn >> 8U; // center 8 bits of SN
    ++buf_it;
    *buf_it = header.sn & 0xffU; // lower 8 bits of SN
  }
  ++buf_it;

  if (header.so != 0) {
    // write SO
    *buf_it = header.so >> 8U; // upper part of SO
    ++buf_it;
    *buf_it = header.so & 0xffU; // lower part of SO
    ++buf_it;
  }
  return std::distance(buf.begin(), buf_it);
}

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::rlc_am_pdu_header> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_am_pdu_header& hdr, FormatContext& ctx) const
  {
    if (hdr.si == srsran::rlc_si_field::full_sdu || hdr.si == srsran::rlc_si_field::first_segment) {
      // Header of full SDU or first SDU segment has no SO.
      return format_to(ctx.out(), "dc={} p={} si={} sn={}", hdr.dc, hdr.p, hdr.si, hdr.sn);
    }
    return format_to(ctx.out(), "dc={} p={} si={} sn={} so={}", hdr.dc, hdr.p, hdr.si, hdr.sn, hdr.so);
  }
};

template <>
struct formatter<srsran::rlc_am_status_nack> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_am_status_nack& nack, FormatContext& ctx) const
  {
    if (nack.has_nack_range) {
      if (nack.has_so) {
        return format_to(ctx.out(), "[{} {}:{} r{}]", nack.nack_sn, nack.so_start, nack.so_end, nack.nack_range);
      }
      return format_to(ctx.out(), "[{} r{}]", nack.nack_sn, nack.nack_range);
    }
    if (nack.has_so) {
      return format_to(ctx.out(), "[{} {}:{}]", nack.nack_sn, nack.so_start, nack.so_end);
    }
    return format_to(ctx.out(), "[{}]", nack.nack_sn);
  }
};

template <>
struct formatter<srsran::rlc_am_status_pdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_am_status_pdu& status, FormatContext& ctx) const
  {
    memory_buffer buffer;
    format_to(std::back_inserter(buffer), "ack_sn={} n_nack={}", status.ack_sn, status.get_nacks().size());
    if (!status.get_nacks().empty()) {
      format_to(std::back_inserter(buffer), " nack=");
      for (auto nack : status.get_nacks()) {
        format_to(std::back_inserter(buffer), "{}", nack);
      }
    }

    return format_to(ctx.out(), "{}", srsran::to_c_str(buffer));
  }
};

} // namespace fmt
