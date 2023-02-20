/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#pragma once

#include "gtpu_tunnel_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "fmt/format.h"
#include <cstdint>

namespace srsran {

/****************************************************************************
 * GTPU Header
 * Ref: 3GPP TS 29.281 v10.1.0 Section 5
 *
 *        | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |
 *
 * 1      |  Version  |PT | * | E | S |PN |
 * 2      |         Message Type          |
 * 3      |     Length (1st Octet)        |
 * 4      |     Length (2nd Octet)        |
 * 5      |      TEID (1st Octet)         |
 * 6      |      TEID (2nd Octet)         |
 * 7      |      TEID (3rd Octet)         |
 * 8      |      TEID (4th Octet)         |
 * 9      |    Seq Number (1st Octet)     |
 * 10     |    Seq Number (2st Octet)     |
 * 11     |            N-PDU              |
 * 12     |  Next Extension Header Type   |
 ***************************************************************************/
constexpr unsigned GTPU_BASE_HEADER_LEN          = 8;
constexpr unsigned GTPU_EXTENDED_HEADER_LEN      = 12;
constexpr unsigned GTPU_NON_MANDATORY_HEADER_LEN = 4;

constexpr unsigned GTPU_FLAGS_VERSION_MASK       = 0xe0;
constexpr unsigned GTPU_FLAGS_VERSION_V1         = 0x01;
constexpr unsigned GTPU_FLAGS_GTP_PRIME_PROTOCOL = 0x00;
constexpr unsigned GTPU_FLAGS_GTP_PROTOCOL       = 0x01;

constexpr unsigned GTPU_MSG_ECHO_REQUEST                             = 1;
constexpr unsigned GTPU_MSG_ECHO_RESPONSE                            = 2;
constexpr unsigned GTPU_MSG_ERROR_INDICATION                         = 26;
constexpr unsigned GTPU_MSG_SUPPORTED_EXTENSION_HEADERS_NOTIFICATION = 31;
constexpr unsigned GTPU_MSG_END_MARKER                               = 254;
constexpr unsigned GTPU_MSG_DATA_PDU                                 = 255;

// GTP-U extension header types. See TS 29.281 v16.2.0, figure 5.2.1-3
enum class gtpu_extension_header_type : uint8_t {
  no_more_extension_headers = 0b00000000,
  reserved_0                = 0b00000001,
  reserved_1                = 0b00000010,
  long_pdcp_pdu_number_0    = 0b00000011,
  service_class_indicator   = 0b00100000,
  udp_port                  = 0b01000000,
  ran_container             = 0b10000001,
  long_pdcp_pdu_number_1    = 0b10000010,
  xw_ran_container          = 0b10000011,
  nr_ran_container          = 0b10000100,
  pdu_session_container     = 0b10000101,
  pdcp_pdu_number           = 0b11000000,
  reserved_2                = 0b11000001,
  reserved_3                = 0b11000010
};
inline const char* to_string(gtpu_extension_header_type type)
{
  switch (type) {
    case gtpu_extension_header_type::no_more_extension_headers:
      return "no more extension headers";
    case gtpu_extension_header_type::service_class_indicator:
      return "service class indicator";
    case gtpu_extension_header_type::udp_port:
      return "UDP port";
    case gtpu_extension_header_type::ran_container:
      return "RAN container";
    case gtpu_extension_header_type::long_pdcp_pdu_number_0:
    case gtpu_extension_header_type::long_pdcp_pdu_number_1:
      return "long PDCP PDU number";
    case gtpu_extension_header_type::xw_ran_container:
      return "XW RAN container";
    case gtpu_extension_header_type::nr_ran_container:
      return "NR RAN container";
    case gtpu_extension_header_type::pdu_session_container:
      return "PDU session container";
    case gtpu_extension_header_type::pdcp_pdu_number:
      return "PDCP PDU number";
    case gtpu_extension_header_type::reserved_0:
    case gtpu_extension_header_type::reserved_1:
    case gtpu_extension_header_type::reserved_2:
    case gtpu_extension_header_type::reserved_3:
      return "reserved";
    default:
      return "invalid";
  }
};

// 00 Comprehension of this extension header is not required. An Intermediate Node shall forward it to any Receiver
// Endpoint
// 01 Comprehension of this extension header is not required. An Intermediate Node shall discard the
// Extension Header Content and not forward it to any Receiver Endpoint. Other extension headers shall be treated
// independently of this extension header.
// 10 Comprehension of this extension header is required by the Endpoint
// Receiver but not by an Intermediate Node. An Intermediate Node shall forward the whole field to the Endpoint
// Receiver.
// 11 Comprehension of this header type is required by recipient (either Endpoint Receiver or Intermediate Node)
enum class gtpu_comprehension : uint8_t {
  not_required_intermediate_node_forward     = 0b00000000,
  not_required_intermediate_node_discard     = 0b00000001,
  required_at_endpoint_not_intermediate_node = 0b00000010,
  required_at_endpoint_and_intermediate_node = 0b00000011
};

/// Base class for GTP-U extension headers
struct gtpu_extension_header {
  gtpu_extension_header_type extension_header_type = gtpu_extension_header_type::no_more_extension_headers;
  uint8_t                    length                = 0;
  std::vector<uint8_t>       container             = {};
};

/// GTP-U header, including extensions
struct gtpu_header {
  struct gtpu_flags {
    uint8_t version       = 1;
    uint8_t protocol_type = 0;
    bool    ext_hdr       = false;
    bool    seq_number    = false;
    bool    n_pdu         = false;
  } flags;
  uint8_t                            message_type      = 0;
  uint16_t                           length            = 0;
  uint32_t                           teid              = 0;
  uint16_t                           seq_number        = 0;
  uint8_t                            n_pdu             = 0;
  gtpu_extension_header_type         next_ext_hdr_type = gtpu_extension_header_type::no_more_extension_headers;
  std::vector<gtpu_extension_header> ext_list          = {};
};

bool gtpu_read_teid(uint32_t& teid, const byte_buffer& pdu, srslog::basic_logger& logger);
bool gtpu_read_and_strip_header(gtpu_header& header, byte_buffer& pdu, gtpu_tunnel_logger& logger);
bool gtpu_write_header(byte_buffer& pdu, const gtpu_header& header, gtpu_tunnel_logger& logger);

bool gtpu_supported_flags_check(const gtpu_header& header, gtpu_tunnel_logger& logger);
bool gtpu_supported_msg_type_check(const gtpu_header& header, gtpu_tunnel_logger& logger);
bool gtpu_extension_header_comprehension_check(const gtpu_extension_header_type& type, gtpu_tunnel_logger& logger);

} // namespace srsran

// Formatters
namespace fmt {

// GTP-U flags
template <>
struct formatter<srsran::gtpu_header::gtpu_flags> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::gtpu_header::gtpu_flags& flags, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "v={} pt={} e={} s={} pn={}",
                     flags.version,
                     flags.protocol_type,
                     flags.ext_hdr,
                     flags.seq_number,
                     flags.n_pdu);
  }
};

template <>
struct formatter<srsran::gtpu_header> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::gtpu_header& hdr, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{} len={} teid={:#x}", hdr.flags, hdr.length, hdr.teid);
  }
};

template <>
struct formatter<srsran::gtpu_extension_header_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::gtpu_extension_header_type& ext_type, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", to_string(ext_type));
  }
};
} // namespace fmt
