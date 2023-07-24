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
#include "gtpu_pdu.h"
#include "gtpu_tunnel_logger.h"
#include "srsran/support/bit_encoding.h"

namespace srsran {

bool gtpu_read_ext_header(bit_decoder&                decoder,
                          gtpu_extension_header&      ext,
                          gtpu_extension_header_type& next_extension_header_type,
                          gtpu_tunnel_logger&         logger);

bool gtpu_write_ext_header(bit_encoder&                 encoder,
                           const gtpu_extension_header& ext,
                           gtpu_extension_header_type   next_extension_header_type,
                           gtpu_tunnel_logger&          logger);

void gtpu_unpack_ext_header_type(bit_decoder& decoder, gtpu_extension_header_type& type);

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 29.281 v10.1.0 Section 5
 ***************************************************************************/
bool gtpu_write_header(byte_buffer& pdu, const gtpu_header& header, gtpu_tunnel_logger& logger)
{
  // flags
  if (!gtpu_supported_flags_check(header, logger)) {
    logger.log_error("Unhandled GTP-U flags. {}", header.flags);
    return false;
  }

  // msg type
  if (!gtpu_supported_msg_type_check(header, logger)) {
    logger.log_error("Unhandled GTP-U message type. msg_type={:#x}", header.message_type);
    return false;
  }

  byte_buffer hdr_buf;
  bit_encoder encoder{hdr_buf};

  // Flags
  encoder.pack(header.flags.version, 3);
  encoder.pack(header.flags.protocol_type, 1);
  encoder.pack(0, 1);                               // Reserved
  encoder.pack(header.flags.ext_hdr ? 1 : 0, 1);    // E
  encoder.pack(header.flags.seq_number ? 1 : 0, 1); // S
  encoder.pack(header.flags.n_pdu ? 1 : 0, 1);      // PN

  // Message type
  encoder.pack(header.message_type, 8);

  // Length
  encoder.pack(header.length, 16);

  // TEID
  encoder.pack(header.teid.value(), 32);

  // Optional header fields
  if (header.flags.ext_hdr || header.flags.seq_number || header.flags.n_pdu) {
    // Sequence Number
    encoder.pack(header.seq_number, 16);

    // N-PDU
    encoder.pack(header.n_pdu, 8);

    // Next Extension Header Type
    encoder.pack(static_cast<uint8_t>(header.next_ext_hdr_type), 8);
  }

  // Write header extensions
  for (unsigned i = 0; i < header.ext_list.size(); ++i) {
    if (i == (header.ext_list.size() - 1)) {
      gtpu_write_ext_header(encoder, header.ext_list[i], gtpu_extension_header_type::no_more_extension_headers, logger);
    } else {
      gtpu_write_ext_header(encoder, header.ext_list[i], header.ext_list[i + 1].extension_header_type, logger);
    }
  }

  pdu.prepend(std::move(hdr_buf));
  return true;
}

bool gtpu_read_teid(uint32_t& teid, const byte_buffer& pdu, srslog::basic_logger& logger)
{
  if (pdu.length() < GTPU_BASE_HEADER_LEN) {
    logger.error(pdu.begin(), pdu.end(), "GTP-U PDU is too small. pdu_len={}", pdu.length());
    return false;
  }
  teid                          = {};
  byte_buffer_reader pdu_reader = pdu;
  pdu_reader += 4;
  for (int i = 3; i >= 0; --i) {
    teid |= (*pdu_reader << (i * 8U));
    ++pdu_reader;
  }
  return true;
}

bool gtpu_dissect_pdu(gtpu_dissected_pdu& dissected_pdu, byte_buffer raw_pdu, gtpu_tunnel_logger& logger)
{
  if (raw_pdu.length() < GTPU_BASE_HEADER_LEN) {
    logger.log_error(raw_pdu.begin(), raw_pdu.end(), "GTP-U PDU is too small. pdu_len={}", raw_pdu.length());
    return false;
  }

  dissected_pdu.buf = std::move(raw_pdu);
  bit_decoder decoder{dissected_pdu.buf};

  // Flags
  decoder.unpack(dissected_pdu.hdr.flags.version, 3);
  decoder.unpack(dissected_pdu.hdr.flags.protocol_type, 1);
  uint8_t spare = {};
  decoder.unpack(spare, 1);                              // Reserved
  decoder.unpack(dissected_pdu.hdr.flags.ext_hdr, 1);    // E
  decoder.unpack(dissected_pdu.hdr.flags.seq_number, 1); // S
  decoder.unpack(dissected_pdu.hdr.flags.n_pdu, 1);      // PN

  // Check supported flags
  if (!gtpu_supported_flags_check(dissected_pdu.hdr, logger)) {
    logger.log_error("Unhandled GTP-U Flags. {}", dissected_pdu.hdr.flags);
    return false;
  }

  // Message type
  decoder.unpack(dissected_pdu.hdr.message_type, 8);

  // Length
  decoder.unpack(dissected_pdu.hdr.length, 16);

  // TEID
  decoder.unpack(dissected_pdu.hdr.teid.value(), 32);

  // Optional header fields
  if (dissected_pdu.hdr.flags.ext_hdr || dissected_pdu.hdr.flags.seq_number || dissected_pdu.hdr.flags.n_pdu) {
    // Sanity check PDU length
    if (dissected_pdu.buf.length() < GTPU_EXTENDED_HEADER_LEN) {
      logger.log_error(dissected_pdu.buf.begin(),
                       dissected_pdu.buf.end(),
                       "Extended GTP-U PDU is too small. pdu_len={}",
                       dissected_pdu.buf.length());
      return false;
    }

    // Sequence Number
    decoder.unpack(dissected_pdu.hdr.seq_number, 16);

    // N-PDU
    decoder.unpack(dissected_pdu.hdr.n_pdu, 8);

    // Next Extension Header Type
    gtpu_unpack_ext_header_type(decoder, dissected_pdu.hdr.next_ext_hdr_type);

    if (not gtpu_extension_header_comprehension_check(dissected_pdu.hdr.next_ext_hdr_type, logger)) {
      return false;
    }
  }

  // Read Header Extensions
  if (dissected_pdu.hdr.flags.ext_hdr) {
    if (dissected_pdu.hdr.next_ext_hdr_type == gtpu_extension_header_type::no_more_extension_headers) {
      logger.log_error(dissected_pdu.buf.begin(),
                       dissected_pdu.buf.end(),
                       "E flag is set, but there are no further extensions. pdu_len={}",
                       dissected_pdu.buf.length());
      return false;
    }
    gtpu_extension_header_type next_extension_header_type = dissected_pdu.hdr.next_ext_hdr_type;
    do {
      gtpu_extension_header ext = {};
      ext.extension_header_type = next_extension_header_type;
      if (not gtpu_extension_header_comprehension_check(ext.extension_header_type, logger)) {
        return false;
      }
      if (!gtpu_read_ext_header(decoder, ext, next_extension_header_type, logger)) {
        return false;
      }
      if (dissected_pdu.hdr.ext_list.size() < dissected_pdu.hdr.ext_list.capacity()) {
        dissected_pdu.hdr.ext_list.push_back(ext);
      } else {
        logger.log_error("PDU exceeds the supported number of header extensions. capacity={}",
                         dissected_pdu.hdr.ext_list.capacity());
        return false;
      }

    } while (next_extension_header_type != gtpu_extension_header_type::no_more_extension_headers);
  }

  // Save header length
  dissected_pdu.hdr_len = decoder.nof_bytes();

  return true;
}

bool gtpu_read_ext_header(bit_decoder&                decoder,
                          gtpu_extension_header&      ext,
                          gtpu_extension_header_type& next_extension_header_type,
                          gtpu_tunnel_logger&         logger)
{
  // TODO check valid read extension types

  // Extract length indicator
  uint8_t length = 0;
  decoder.unpack(length, 8);

  // TODO check valid length for the extension type

  // The payload size is four bytes per the indicated length,
  // minus one byte for the length field and one for the next
  // extension header type. See section 5.2.1 of 29.281.
  uint16_t payload = length * 4 - 2;

  // TODO check max size

  // Extract view to container
  ext.container = decoder.unpack_aligned_bytes(payload);

  // Extract next extension header type
  gtpu_unpack_ext_header_type(decoder, next_extension_header_type);
  return true;
}

bool gtpu_write_ext_header(bit_encoder&                 encoder,
                           const gtpu_extension_header& ext,
                           gtpu_extension_header_type   next_extension_header_type,
                           gtpu_tunnel_logger&          logger)
{
  // TODO check valid write extension types

  uint8_t payload = 1 + ext.container.length() + 1;
  srsran_assert(payload % 4 == 0, "Invalid GTP-U extension size. payload={}", payload);

  uint8_t length = payload / 4;

  // Pack length
  encoder.pack(length, 8);

  // Pack container
  encoder.pack_bytes(ext.container);

  // Pack next header extension type
  encoder.pack(static_cast<uint8_t>(next_extension_header_type), 8);
  return true;
}

void gtpu_unpack_ext_header_type(bit_decoder& decoder, gtpu_extension_header_type& type)
{
  uint8_t tmp = 0;
  decoder.unpack(tmp, 8);
  type = static_cast<gtpu_extension_header_type>(tmp);
}

/// Supported feature helpers
bool gtpu_supported_flags_check(const gtpu_header& header, gtpu_tunnel_logger& logger)
{
  // flags
  if (header.flags.version != GTPU_FLAGS_VERSION_V1) {
    logger.log_error("Unhandled GTP-U version. {}", header.flags);
    return false;
  }
  if (header.flags.protocol_type != GTPU_FLAGS_GTP_PROTOCOL) {
    logger.log_error("Unhandled protocol type. {}", header.flags);
    return false;
  }
  if (header.flags.n_pdu) {
    logger.log_error("Unhandled packet number. {}", header.flags);
    return false;
  }
  return true;
}

bool gtpu_supported_msg_type_check(const gtpu_header& header, gtpu_tunnel_logger& logger)
{
  // msg_tpye
  if (header.message_type != GTPU_MSG_DATA_PDU && header.message_type != GTPU_MSG_ECHO_REQUEST &&
      header.message_type != GTPU_MSG_ECHO_RESPONSE && header.message_type != GTPU_MSG_ERROR_INDICATION &&
      header.message_type != GTPU_MSG_END_MARKER) {
    logger.log_error("Unhandled message type. msg_type={:#x}", header.message_type);
    return false;
  }
  return true;
}

bool gtpu_extension_header_comprehension_check(const gtpu_extension_header_type& type, gtpu_tunnel_logger& logger)
{
  switch (type) {
    case gtpu_extension_header_type::no_more_extension_headers:
      return true;
    case gtpu_extension_header_type::service_class_indicator:
    case gtpu_extension_header_type::udp_port:
    case gtpu_extension_header_type::ran_container:
    case gtpu_extension_header_type::long_pdcp_pdu_number_0:
    case gtpu_extension_header_type::long_pdcp_pdu_number_1:
    case gtpu_extension_header_type::xw_ran_container:
    case gtpu_extension_header_type::nr_ran_container:
      break;
    case gtpu_extension_header_type::pdu_session_container:
      return true; // TODO add actual support for PDU session container
    case gtpu_extension_header_type::pdcp_pdu_number:
      return true; // TODO add actual support for PDCP PDU number
    case gtpu_extension_header_type::reserved_0:
    case gtpu_extension_header_type::reserved_1:
    case gtpu_extension_header_type::reserved_2:
    case gtpu_extension_header_type::reserved_3:
      return false;
    default:
      break;
  }
  logger.log_debug("Extension header not comprehended. type={}", type);

  uint8_t comp = static_cast<uint8_t>(type) >> 6U;
  bool    comp_not_needed =
      !(comp == static_cast<uint8_t>(gtpu_comprehension::required_at_endpoint_not_intermediate_node) ||
        comp == static_cast<uint8_t>(gtpu_comprehension::required_at_endpoint_and_intermediate_node));
  if (comp_not_needed) {
    logger.log_debug("Extension header not comprehended. type={}", type);
  } else {
    logger.log_error("Extension header not comprehended. type={}", type);
  }
  return comp_not_needed;
}

byte_buffer gtpu_extract_t_pdu(gtpu_dissected_pdu&& dissected_pdu)
{
  dissected_pdu.buf.trim_head(dissected_pdu.hdr_len);
  return std::move(dissected_pdu.buf);
}

} // namespace srsran
