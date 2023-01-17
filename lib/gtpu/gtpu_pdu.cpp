/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "gtpu_pdu.h"
#include "srsgnb/support/bit_encoding.h"

namespace srsgnb {

bool gtpu_read_ext_header(bit_decoder&                      decoder,
                          const gtpu_header_extension_type& header_type,
                          gtpu_extension_header&            ext,
                          gtpu_header_extension_type&       next_extension_header_type,
                          srslog::basic_logger&             logger);

bool gtpu_write_ext_header(bit_encoder&                 encoder,
                           const gtpu_extension_header& ext,
                           gtpu_header_extension_type   next_extension_header_type,
                           srslog::basic_logger&        logger);

void gtpu_unpack_ext_header_type(bit_decoder& decoder, gtpu_header_extension_type& type);

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 29.281 v10.1.0 Section 5
 ***************************************************************************/
bool gtpu_write_header(byte_buffer& pdu, const gtpu_header& header, srslog::basic_logger& logger)
{
  // flags
  if (!gtpu_supported_flags_check(header, logger)) {
    logger.error("gtpu_write_header - Unhandled GTP-U Flags. Flags: {}", header.flags);
    return false;
  }

  // msg type
  if (!gtpu_supported_msg_type_check(header, logger)) {
    logger.error("gtpu_write_header - Unhandled GTP-U Message Type. Message Type: {}", header.message_type);
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
  encoder.pack(header.teid, 32);

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
      gtpu_write_ext_header(encoder, header.ext_list[i], gtpu_header_extension_type::no_more_extension_headers, logger);
    } else {
      gtpu_write_ext_header(encoder, header.ext_list[i], header.ext_list[i + 1].extension_header_type, logger);
    }
  }

  pdu.chain_before(std::move(hdr_buf));
  return true;
}

bool gtpu_read_teid(uint32_t& teid, const byte_buffer& pdu, srslog::basic_logger& logger)
{
  if (pdu.length() < GTPU_BASE_HEADER_LEN) {
    logger.error(pdu.begin(), pdu.end(), "Error GTP-U PDU is too small. Length={}", pdu.length());
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

bool gtpu_read_and_strip_header(gtpu_header& header, byte_buffer& pdu, srslog::basic_logger& logger)
{
  if (pdu.length() < GTPU_BASE_HEADER_LEN) {
    logger.error(pdu.begin(), pdu.end(), "Error GTP-U PDU is too small. Length={}", pdu.length());
    return false;
  }

  bit_decoder decoder{pdu};

  // Flags
  decoder.unpack(header.flags.version, 3);
  decoder.unpack(header.flags.protocol_type, 1);
  uint8_t spare = {};
  decoder.unpack(spare, 1);                   // Reserved
  decoder.unpack(header.flags.ext_hdr, 1);    // E
  decoder.unpack(header.flags.seq_number, 1); // S
  decoder.unpack(header.flags.n_pdu, 1);      // PN

  // Check supported flags
  if (!gtpu_supported_flags_check(header, logger)) {
    logger.error("gtpu_read_header - Unhandled GTP-U Flags. Flags: {}", header.flags);
    return false;
  }

  // Message type
  decoder.unpack(header.message_type, 8);

  // Length
  decoder.unpack(header.length, 16);

  // TEID
  decoder.unpack(header.teid, 32);

  // Optional header fields
  if (header.flags.ext_hdr || header.flags.seq_number || header.flags.n_pdu) {
    // Sanity check PDU length
    if (pdu.length() < GTPU_EXTENDED_HEADER_LEN) {
      logger.error(pdu.begin(), pdu.end(), "Error extended GTP-U PDU is too small. Length={}", pdu.length());
      return false;
    }

    // Sequence Number
    decoder.unpack(header.seq_number, 16);

    // N-PDU
    decoder.unpack(header.n_pdu, 8);

    // Next Extension Header Type
    gtpu_unpack_ext_header_type(decoder, header.next_ext_hdr_type);
  }

  // Read Header Extensions
  if (header.flags.ext_hdr) {
    if (header.next_ext_hdr_type == gtpu_header_extension_type::no_more_extension_headers) {
      logger.error(pdu.begin(), pdu.end(), "Error E flag is set, but there are no extra extensions");
      return false;
    }
    gtpu_header_extension_type next_extension_header_type = gtpu_header_extension_type::no_more_extension_headers;
    do {
      gtpu_extension_header ext = {};
      if (!gtpu_read_ext_header(decoder, header.next_ext_hdr_type, ext, next_extension_header_type, logger)) {
        return false;
      }
      header.ext_list.push_back(ext);
    } while (next_extension_header_type != gtpu_header_extension_type::no_more_extension_headers);
  }
  // Trim header
  pdu.trim_head(decoder.nof_bytes());

  return true;
}

bool gtpu_read_ext_header(bit_decoder&                      decoder,
                          const gtpu_header_extension_type& header_type,
                          gtpu_extension_header&            ext,
                          gtpu_header_extension_type&       next_extension_header_type,
                          srslog::basic_logger&             logger)
{
  // TODO check valid read extension types

  // Extract length indicator
  decoder.unpack(ext.length, 8);

  // The payload size is four bytes per the indicated length,
  // minus one byte for the length field and one for the next
  // extension header type. See section 5.2.1 of 29.281.
  uint16_t payload = ext.length * 4 - 2;

  // TODO check max size

  // Extract container
  ext.container.resize(payload);
  for (unsigned i = 0; i < ext.container.size(); ++i) {
    decoder.unpack(ext.container[i], 8);
  }

  // Extract next extension header type
  gtpu_unpack_ext_header_type(decoder, next_extension_header_type);

  ext.extension_header_type = header_type;
  return true;
}

bool gtpu_write_ext_header(bit_encoder&                 encoder,
                           const gtpu_extension_header& ext,
                           gtpu_header_extension_type   next_extension_header_type,
                           srslog::basic_logger&        logger)
{
  // TODO check valid write extension types

  uint8_t payload = 1 + ext.container.size() + 1;
  srsgnb_assert(payload % 4 == 0, "Invalid GTP-U extension size");

  uint8_t length = payload / 4;

  // Pack length
  encoder.pack(length, 8);

  // Pack container
  for (unsigned i = 0; i < ext.container.size(); ++i) {
    encoder.pack(ext.container[i], 8);
  }

  // Pack next header extension type
  encoder.pack(static_cast<uint8_t>(next_extension_header_type), 8);
  return true;
}

void gtpu_unpack_ext_header_type(bit_decoder& decoder, gtpu_header_extension_type& type)
{
  uint8_t tmp = 0;
  decoder.unpack(tmp, 8);
  type = static_cast<gtpu_header_extension_type>(tmp);
}
} // namespace srsgnb
