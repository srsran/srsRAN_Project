/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "gtpu.h"
#include "srsgnb/support/bit_encoding.h"

namespace srsgnb {

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 29.281 v10.1.0 Section 5
 ***************************************************************************/
bool gtpu_write_header(const gtpu_header& header, byte_buffer& pdu, srslog::basic_logger& logger)
{
  // flags
  if (!gtpu_supported_flags_check(header, logger)) {
    logger.error("gtpu_write_header - Unhandled GTP-U Flags. Flags: 0x%x", header.flags);
    return false;
  }

  // msg type
  if (!gtpu_supported_msg_type_check(header, logger)) {
    logger.error("gtpu_write_header - Unhandled GTP-U Message Type. Message Type: 0x%x", header.message_type);
    return false;
  }

  // If E, S or PN are set, the header is longer
  /*
  if (header.flags & (GTPU_FLAGS_EXTENDED_HDR | GTPU_FLAGS_SEQUENCE | GTPU_FLAGS_PACKET_NUM)) {
    if (pdu.get_headroom() < GTPU_EXTENDED_HEADER_LEN) {
      logger.error("gtpu_write_header - No room in PDU for header");
      return false;
    }
    pdu->msg -= GTPU_EXTENDED_HEADER_LEN;
    pdu->N_bytes += GTPU_EXTENDED_HEADER_LEN;
    header->length += GTPU_EXTENDED_HEADER_LEN - GTPU_BASE_HEADER_LEN;
    if (header->next_ext_hdr_type > 0) {
      pdu->msg -= header->ext_buffer.size();
      pdu->N_bytes += header->ext_buffer.size();
      header->length += header->ext_buffer.size();
    }
  } else {
    if (pdu->get_headroom() < GTPU_BASE_HEADER_LEN) {
      logger.error("gtpu_write_header - No room in PDU for header");
      return false;
    }
    pdu->msg -= GTPU_BASE_HEADER_LEN;
    pdu->N_bytes += GTPU_BASE_HEADER_LEN;
  }

  // write mandatory fields
  uint8_t* ptr = pdu->msg;
  *ptr         = header->flags;
  ptr++;
  *ptr = header->message_type;
  ptr++;
  uint16_to_uint8(header->length, ptr);
  ptr += 2;
  uint32_to_uint8(header->teid, ptr);
  ptr += 4;
  // write optional fields, if E, S or PN are set.
  if (header->flags & (GTPU_FLAGS_EXTENDED_HDR | GTPU_FLAGS_SEQUENCE | GTPU_FLAGS_PACKET_NUM)) {
    // S
    if (header->flags & GTPU_FLAGS_SEQUENCE) {
      uint16_to_uint8(header->seq_number, ptr);
    } else {
      uint16_to_uint8(0, ptr);
    }
    ptr += 2;
    // PN
    if (header->flags & GTPU_FLAGS_PACKET_NUM) {
      *ptr = header->n_pdu;
    } else {
      header->n_pdu = 0;
      *ptr          = 0;
    }
    ptr++;
    // E
    if (header->flags & GTPU_FLAGS_EXTENDED_HDR) {
      *ptr = header->next_ext_hdr_type;
      ptr++;
      for (size_t i = 0; i < header->ext_buffer.size(); ++i) {
        *ptr = header->ext_buffer[i];
        ptr++;
      }
    } else {
      *ptr = 0;
      ptr++;
    }
  }
  */
  return true;
}

bool gtpu_read_header(gtpu_header& header, const byte_buffer& pdu, srslog::basic_logger& logger)
{
  bit_decoder decoder{pdu};

  // Version
  uint8_t version = {};
  if (not decoder.unpack(version, 3)) {
    logger.error(pdu.begin(), pdu.end(), "Error reading GTP-U version. Flags: {}", header.flags);
    return false;
  }
  header.flags.version = version;

  // PT
  uint8_t pt = {};
  if (not decoder.unpack(pt, 1)) {
    logger.error(pdu.begin(), pdu.end(), "Error reading GTP-U protocol type. Flags: {}", header.flags);
    return false;
  }
  header.flags.protocol_type = pt;

  // Spare
  uint8_t spare = {};
  if (not decoder.unpack(spare, 1)) {
    logger.error(pdu.begin(), pdu.end(), "Error reading GTP-U spare bit. Flags: {}", header.flags);
    return false;
  }

  // E
  uint8_t ext_flag = {};
  if (not decoder.unpack(ext_flag, 1)) {
    logger.error(pdu.begin(), pdu.end(), "Error reading GTP-U extension flag. Flags: {}", header.flags);
    return false;
  }
  header.flags.ext_hdr = ext_flag == 1;

  // S
  uint8_t sn_flag = {};
  if (not decoder.unpack(sn_flag, 1)) {
    logger.error(pdu.begin(), pdu.end(), "Error reading GTP-U SN flag. Flags: {}", header.flags);
    return false;
  }
  header.flags.seq_number = sn_flag == 1;

  // PN
  uint8_t pn_flag = {};
  if (not decoder.unpack(pn_flag, 1)) {
    logger.error(pdu.begin(), pdu.end(), "Error reading GTP-U N-PDU flag. Flags: {}", header.flags);
    return false;
  }
  header.flags.n_pdu = pn_flag == 1;

  // flags
  if (!gtpu_supported_flags_check(header, logger)) {
    logger.error("gtpu_read_header - Unhandled GTP-U Flags. Flags: {}", header.flags);
    return false;
  }

  // message_type
  if (!gtpu_supported_msg_type_check(header, logger)) {
    logger.error("gtpu_read_header - Unhandled GTP-U Message Type. Flags: {}", header.flags);
    return false;
  }
  /*
  // If E, S or PN are set, header is longer
  if (header->flags & (GTPU_FLAGS_EXTENDED_HDR | GTPU_FLAGS_SEQUENCE | GTPU_FLAGS_PACKET_NUM)) {
    pdu->msg += GTPU_EXTENDED_HEADER_LEN;
    pdu->N_bytes -= GTPU_EXTENDED_HEADER_LEN;

    uint8_to_uint16(ptr, &header->seq_number);
    ptr += 2;

    header->n_pdu = *ptr;
    ptr++;

    header->next_ext_hdr_type = *ptr;
    ptr++;

    if (not gtpu_read_ext_header(pdu, &ptr, header, logger)) {
      return false;
    }
  } else {
    pdu->msg += GTPU_BASE_HEADER_LEN;
    pdu->N_bytes -= GTPU_BASE_HEADER_LEN;
  }
  */
  return true;
}

bool gtpu_read_ext_header(const byte_buffer& pdu, uint8_t** ptr, gtpu_header& header, srslog::basic_logger& logger)
{
  /*
  if ((header->flags & GTPU_FLAGS_EXTENDED_HDR) == 0 or header->next_ext_hdr_type == 0) {
    return true;
  }

  // TODO: Iterate over next headers until no more extension headers
  switch (header->next_ext_hdr_type) {
    case GTPU_EXT_HEADER_PDCP_PDU_NUMBER:
      pdu->msg += HEADER_PDCP_PDU_NUMBER_SIZE;
      pdu->N_bytes -= HEADER_PDCP_PDU_NUMBER_SIZE;
      header->ext_buffer.resize(HEADER_PDCP_PDU_NUMBER_SIZE);
      for (size_t i = 0; i < HEADER_PDCP_PDU_NUMBER_SIZE; ++i) {
        header->ext_buffer[i] = **ptr;
        (*ptr)++;
      }
      break;
    case GTPU_EXT_HEADER_PDU_SESSION_CONTAINER:
      pdu->msg += GTPU_EXT_HEADER_PDU_SESSION_CONTAINER_LEN;
      pdu->N_bytes -= GTPU_EXT_HEADER_PDU_SESSION_CONTAINER_LEN;
      // TODO: Save Header Extension
      break;
    default:
      logger.error("gtpu_read_header - Unhandled GTP-U Extension Header Type: 0x%x", header->next_ext_hdr_type);
      return false;
  }
  */
  return true;
}
} // namespace srsgnb
