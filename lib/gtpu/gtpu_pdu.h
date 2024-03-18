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

#include "gtpu_tunnel_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/static_vector.h"
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

constexpr unsigned GTPU_MAX_NUM_HEADER_EXTENSIONS = 10;

constexpr unsigned GTPU_MAX_NUM_PRIVATE_EXTENSIONS     = 1;
constexpr unsigned GTPU_PRIVATE_EXTENSION_VALUE_LENGTH = 1;

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

// GTP-U information element types types. See TS 29.281 Sec. 8.1
enum class gtpu_information_element_type : uint8_t {
  recovery                          = 14,
  tunnel_endpoint_identifier_data_i = 16,
  gsn_address                       = 133,
  extension_header_type_list        = 141,
  private_extension                 = 255
};
inline const char* to_string(gtpu_information_element_type type)
{
  switch (type) {
    case gtpu_information_element_type::recovery:
      return "Recovery";
    case gtpu_information_element_type::tunnel_endpoint_identifier_data_i:
      return "Tunnel Endpoint Identifier Data I";
    case gtpu_information_element_type::gsn_address:
      return "GSN Address";
    case gtpu_information_element_type::extension_header_type_list:
      return "Extension Header Type List";
    case gtpu_information_element_type::private_extension:
      return "Private Extension";
    default:
      return "Reserved";
  }
};

/// Base class for GTP-U extension headers
struct gtpu_extension_header {
  gtpu_extension_header_type extension_header_type = gtpu_extension_header_type::no_more_extension_headers;
  byte_buffer_view           container             = {};
};

/// GTP-U information element for "Recovery". See TS 29.281 Sec. 8.2
///
/// The value of the restart counter shall be set to 0 by the sending entity and ignored by the receiving entity. This
/// information element is used in GTP user plane due to backwards compatibility reasons.
///
/// IE format: TV
struct gtpu_ie_recovery {
  /// Restart counter (always 0)
  uint8_t restart_counter = 0;
};

/// GTPU information element for "Tunnel Endpoint Identifier Data I". See TS 29.281 Sec. 8.3
///
/// The Tunnel Endpoint Identifier Data I information element contains the Tunnel Endpoint Identifier used by a GTP
/// entity for the user plane.
///
/// IE format: TV
struct gtpu_ie_teid_i {
  /// Tunnel Endpoint Identifier Data I
  uint32_t teid_i = 0;
};

struct gtpu_ie_gtpu_peer_address {
  using ipv4_addr_t = std::array<uint8_t, 4>;
  using ipv6_addr_t = std::array<uint8_t, 16>;

  /// IPv4 or IPv6 Address
  variant<ipv4_addr_t, ipv6_addr_t> gtpu_peer_address;
};

/// GTP-U information element for "Private Extension". See TS 29.281 Sec. 8.6
/// IE format: TLV
struct gtpu_ie_private_extension {
  /// The Extension Identifier is a value defined in the Private Enterprise number list
  uint16_t extension_identifier;
  /// Custom extension
  static_vector<uint8_t, GTPU_PRIVATE_EXTENSION_VALUE_LENGTH> extension_value;
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
  /// This field indicates the type of GTP-U message.
  uint8_t message_type = 0;
  /// This field indicates the length in octets of the payload, i.e. the rest of the packet following the mandatory
  /// part of the GTP header (that is the first 8 octets). The Sequence Number, the N-PDU Number or any Extension
  /// headers shall be considered to be part of the payload, i.e. included in the length count.
  uint16_t length = 0;
  /// Tunnel Endpoint Identifier.
  gtpu_teid_t teid = {};
  /// Sequence Number.
  uint16_t seq_number = 0;
  /// N-PDU Number.
  uint8_t n_pdu = 0;
  /// Next Extension Header Type: This field defines the type of Extension Header that follows this field in the
  /// GTP-PDU.
  gtpu_extension_header_type next_ext_hdr_type = gtpu_extension_header_type::no_more_extension_headers;
  /// Collection of included GTP-U header extensions
  static_vector<gtpu_extension_header, GTPU_MAX_NUM_HEADER_EXTENSIONS> ext_list;
};

/// Intermediate representation of a received GTP-U PDU to access the unpacked header and to the raw content of the
/// extension headers. This container holds a buffer with the original PDU to which the views of the extension header
/// content are mapped. After processing the extension headers, the T-PDU can be extracted via \c gtpu_extract_t_pdu
/// which advances the PDU buffer to the start of the T-PDU according to \c hdr_len.
struct gtpu_dissected_pdu {
  /// Storage of the original GTP-U PDU to which the views of the extension header content is mapped
  byte_buffer buf;
  /// Access to the dissected content of the GTP-U PDU
  gtpu_header hdr;
  /// Total header length (including all extension headers); marks the start of the T-PDU.
  size_t hdr_len = 0;
};

bool gtpu_read_teid(uint32_t& teid, const byte_buffer& pdu, srslog::basic_logger& logger);

/// Creates a dissected representation of a raw GTP-U PDU with access to the unpacked header and to the raw content of
/// the extension headers for further processing of the extensions.
/// \param[out] dissected_pdu Reference to an object into which the dissected GTP-U PDU shall be filled/moved.
/// \param[in] raw_pdu The raw GTP-U PDU that shall be dissected.
/// \param[in] logger Access to the logger.
/// \return True if the dissection was successful, False otherwise.
bool gtpu_dissect_pdu(gtpu_dissected_pdu& dissected_pdu, byte_buffer raw_pdu, gtpu_tunnel_logger& logger);

/// Prepends the GTP-U header in front of a GTP-U PDU.
/// Ref: TS 29.281 Sec.
/// \param[out] pdu Buffer of the GTP-U PDU to which the information element shall be prepended.
/// \param[in] header The GTP-U header.
/// \param[in] logger Access to the logger.
/// \return True if write was successful, False otherwise.
bool gtpu_write_header(byte_buffer& pdu, const gtpu_header& header, gtpu_tunnel_logger& logger);

/// Append the "recovery" information element to a GTP-U PDU.
/// Ref: TS 29.281 Sec. 8.2.
/// \param[out] pdu Buffer of the GTP-U PDU to which the information element shall be appended.
/// \param[in] ie_recovery The information element "recovery".
/// \param[in] logger Access to the logger.
/// \return True if write was successful, False otherwise.
bool gtpu_write_ie_recovery(byte_buffer& pdu, gtpu_ie_recovery& ie_recovery, gtpu_tunnel_logger& logger);

/// Append the "private extension" information element to a GTP-U PDU.
/// Ref: TS 29.281 Sec. 8.6.
/// \param[out] pdu Buffer of the GTP-U PDU to which the information element shall be appended.
/// \param[in] ie_recovery The information element "private extension".
/// \param[in] logger Access to the logger.
/// \return True if write was successful, False otherwise.
bool gtpu_write_ie_private_extension(byte_buffer&               pdu,
                                     gtpu_ie_private_extension& ie_priv_ext,
                                     gtpu_tunnel_logger&        logger);

bool gtpu_supported_flags_check(const gtpu_header& header, gtpu_tunnel_logger& logger);
bool gtpu_supported_msg_type_check(const gtpu_header& header, gtpu_tunnel_logger& logger);
bool gtpu_extension_header_comprehension_check(const gtpu_extension_header_type& type, gtpu_tunnel_logger& logger);

/// Extracts the GTP-U message (i.e. the information elements or the G-PDU) of a dissected GTP-U PDU by advancing its
/// internal PDU buffer by the header length and returning that object. Any other content of the dissected PDU (e.g.
/// header extensions) will be invalidated.
///
/// \param dissected_pdu The dissected GTP-U PDU from which the message shall be extracted.
/// \return The T-PDU of the dissected GTP-U PDU.
byte_buffer gtpu_extract_msg(gtpu_dissected_pdu&& dissected_pdu);

/// GTP-U Tunnel Management Message: Error Indication. See TS 29.281 Sec. 7.3.1
struct gtpu_msg_error_indication {
  /// Tunnel Endpoint Identifier Data I
  gtpu_ie_teid_i teid_i;
  /// GTP-U Peer Adddress
  gtpu_ie_gtpu_peer_address gtpu_peer_address;
};

/// Reads and unpacks a GTP-U error indication message.
bool gtpu_read_msg_error_indication(gtpu_msg_error_indication& error_indication,
                                    const byte_buffer&         pdu,
                                    srslog::basic_logger&      logger);

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

template <>
struct formatter<srsran::gtpu_ie_teid_i> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::gtpu_ie_teid_i& ie, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "teid_i={:#x}", ie.teid_i);
  }
};

template <>
struct formatter<srsran::gtpu_ie_gtpu_peer_address> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::gtpu_ie_gtpu_peer_address& ie, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (srsran::variant_holds_alternative<srsran::gtpu_ie_gtpu_peer_address::ipv4_addr_t>(ie.gtpu_peer_address)) {
      auto& addr = srsran::variant_get<srsran::gtpu_ie_gtpu_peer_address::ipv4_addr_t>(ie.gtpu_peer_address);
      return format_to(ctx.out(), "peer_addr={}.{}.{}.{}", addr[0], addr[1], addr[2], addr[3]);
    }
    if (srsran::variant_holds_alternative<srsran::gtpu_ie_gtpu_peer_address::ipv6_addr_t>(ie.gtpu_peer_address)) {
      auto& addr = srsran::variant_get<srsran::gtpu_ie_gtpu_peer_address::ipv6_addr_t>(ie.gtpu_peer_address);
      return format_to(ctx.out(),
                       "peer_addr={:x}{:x}:{:x}{:x}:{:x}{:x}:{:x}{:x}:{:x}{:x}:{:x}{:x}:{:x}{:x}:{:x}{:x}",
                       addr[0],
                       addr[1],
                       addr[2],
                       addr[3],
                       addr[4],
                       addr[5],
                       addr[6],
                       addr[7],
                       addr[8],
                       addr[9],
                       addr[10],
                       addr[11],
                       addr[12],
                       addr[13],
                       addr[14],
                       addr[15]);
    }
    return format_to(ctx.out(), "peer_addr={{na}}", ie);
  }
};

template <>
struct formatter<srsran::gtpu_msg_error_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::gtpu_msg_error_indication& err_ind, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{} {}", err_ind.teid_i, err_ind.gtpu_peer_address);
  }
};
} // namespace fmt
