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
bool gtpu_write_header(byte_buffer& pdu, const gtpu_header& header, srslog::basic_logger& logger)
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

  // TODO write header extensions

  pdu.chain_before(std::move(hdr_buf));
  return true;
}

bool gtpu_read_and_strip_header(gtpu_header& header, byte_buffer& pdu, srslog::basic_logger& logger)
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

  // Check supported flags
  if (!gtpu_supported_flags_check(header, logger)) {
    logger.error("gtpu_read_header - Unhandled GTP-U Flags. Flags: {}", header.flags);
    return false;
  }
  pdu.trim_head(1);

  // Message type
  header.message_type = *pdu.begin();
  if (!gtpu_supported_msg_type_check(header, logger)) {
    logger.error("gtpu_read_header - Unhandled GTP-U Message Type. Flags: {}", header.flags);
    return false;
  }
  pdu.trim_head(1);

  {
    header.length = {};
    auto it       = pdu.begin();
    for (uint32_t i = 0; i < 2; ++i) {
      header.length |= (uint32_t)(*it) << ((2 - 1) - i) * 8;
      ++it;
    }
    pdu.trim_head(2);
  }
  {
    uint32_t teid = {};
    auto     it   = pdu.begin();
    for (uint32_t i = 0; i < 4; ++i) {
      teid |= (uint32_t)(*it) << ((4 - 1) - i) * 8;
      ++it;
    }
    header.teid = teid;
    pdu.trim_head(4);
  }
  // TODO handle extended headers
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
