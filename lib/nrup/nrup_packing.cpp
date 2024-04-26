/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/nrup/nrup_packing.h"
#include "srsran/support/bit_encoding.h"

namespace srsran {

/// Macro used to check a read/unpack operation and log an error message if the validation fails.
#define VERIFY_READ(cond)                                                                                              \
  if (not(cond)) {                                                                                                     \
    logger.error("Read failed in {}", __FUNCTION__);                                                                   \
    return false;                                                                                                      \
  }

/// Macro used to check a write/append/pack operation and log an error message if the validation fails.
#define VERIFY_WRITE(cond)                                                                                             \
  if (not(cond)) {                                                                                                     \
    logger.error("Write failed in {}", __FUNCTION__);                                                                  \
    return false;                                                                                                      \
  }

nrup_pdu_type nrup_packing::get_pdu_type(byte_buffer_view container)
{
  srsran_assert(!container.empty(), "Cannot get PDU type of empty PDU");
  uint8_t field_value = container[0] >> 4;
  return uint_to_nrup_pdu_type(field_value);
}

bool nrup_packing::unpack(nru_dl_user_data& dl_user_data, byte_buffer_view container) const
{
  if (container.empty()) {
    logger.error("Failed to unpack DL user data: pdu_len=0");
    return false;
  }

  if ((container.length() + 2) % 4 != 0) {
    logger.error("Failed to unpack DL user data: pdu_len={} != n*4-2", container.length());
    return false;
  }

  bit_decoder decoder{container};

  // PDU Type
  uint8_t pdu_type = {};
  VERIFY_READ(decoder.unpack(pdu_type, 4));
  if (uint_to_nrup_pdu_type(pdu_type) != nrup_pdu_type::dl_user_data) {
    logger.error("Failed to unpack DL user data: Invalid pdu_type={}", uint_to_nrup_pdu_type(pdu_type));
    return false;
  }

  // Spare (v15.2.0)
  uint8_t spare = {};
  VERIFY_READ(decoder.unpack(spare, 1));
  if (spare != 0) {
    logger.error("Failed to unpack DL user data: Spare bit set in first octet. value={:#x}", spare);
    return false;
  }

  // DL discard blocks
  bool dl_discard_blocks = {};
  VERIFY_READ(decoder.unpack(dl_discard_blocks, 1));

  // DL flush
  bool dl_flush = {};
  VERIFY_READ(decoder.unpack(dl_flush, 1));

  // Report polling
  VERIFY_READ(decoder.unpack(dl_user_data.report_polling, 1));

  // Spare (v15.2.0)
  spare = {};
  VERIFY_READ(decoder.unpack(spare, 6));
  if (spare != 0) {
    logger.error("Failed to unpack DL user data: Spare bits set in first octet. value={:#x}", spare);
    return false;
  }

  // Assistance Info Report Polling Flag
  decoder.unpack(dl_user_data.assist_info_report_polling_flag, 1);

  // Retransmission flag
  VERIFY_READ(decoder.unpack(dl_user_data.retransmission_flag, 1));

  // NR-U Sequence Number
  VERIFY_READ(decoder.unpack(dl_user_data.nru_sn, 24));

  // DL discard NR PDCP PDU SN (if present)
  if (dl_flush) {
    uint32_t dl_discard_pdcp_sn = {};
    VERIFY_READ(decoder.unpack(dl_discard_pdcp_sn, 24));
    dl_user_data.dl_discard_pdcp_sn = dl_discard_pdcp_sn;
  }

  // Discard blocks (if present)
  if (dl_discard_blocks) {
    // DL discard number of blocks
    uint8_t nof_discard_blocks = {};
    VERIFY_READ(decoder.unpack(nof_discard_blocks, 8));

    if (nof_discard_blocks > nru_max_nof_pdcp_sn_discard_blocks) {
      logger.error("Failed to unpack DL user data: nof_discard_blocks={} exceeds max_nof_pdcp_sn_discard_blocks={}",
                   nof_discard_blocks,
                   nru_max_nof_pdcp_sn_discard_blocks);
      return false;
    }

    // Read all discard blocks
    dl_user_data.discard_blocks = nru_pdcp_sn_discard_blocks{};
    for (uint8_t i = 0; i < nof_discard_blocks; i++) {
      dl_user_data.discard_blocks.value().push_back(nru_pdcp_sn_discard_block{});
      VERIFY_READ(decoder.unpack(dl_user_data.discard_blocks.value().back().pdcp_sn_start, 24));
      VERIFY_READ(decoder.unpack(dl_user_data.discard_blocks.value().back().block_size, 8));
    }
  }

  return true;
};

bool nrup_packing::pack(byte_buffer& out_buf, const nru_dl_user_data& dl_user_data) const
{
  size_t      start_len = out_buf.length();
  bit_encoder encoder{out_buf};

  // PDU Type
  VERIFY_WRITE(encoder.pack(nrup_pdu_type_to_uint(nrup_pdu_type::dl_user_data), 4));

  // Spare (v15.2.0)
  VERIFY_WRITE(encoder.pack(0, 1));

  // DL discard blocks
  bool dl_discard_blocks = dl_user_data.discard_blocks.has_value() && !dl_user_data.discard_blocks.value().empty();
  VERIFY_WRITE(encoder.pack(dl_discard_blocks, 1));

  // DL flush
  VERIFY_WRITE(encoder.pack(dl_user_data.dl_discard_pdcp_sn.has_value(), 1));

  // Report polling
  VERIFY_WRITE(encoder.pack(dl_user_data.report_polling, 1));

  // Spare (v15.2.0)
  VERIFY_WRITE(encoder.pack(0, 6));

  // Assistance Info Report Polling Flag
  VERIFY_WRITE(encoder.pack(dl_user_data.assist_info_report_polling_flag, 1));

  // Retransmission flag
  VERIFY_WRITE(encoder.pack(dl_user_data.retransmission_flag, 1));

  // NR-U Sequence Number
  VERIFY_WRITE(encoder.pack(dl_user_data.nru_sn, 24));

  // DL discard NR PDCP PDU SN (if present)
  if (dl_user_data.dl_discard_pdcp_sn.has_value()) {
    VERIFY_WRITE(encoder.pack(dl_user_data.dl_discard_pdcp_sn.value(), 24));
  }

  // Discard blocks (if present)
  if (dl_discard_blocks) {
    // DL discard number of blocks
    VERIFY_WRITE(encoder.pack(dl_user_data.discard_blocks.value().size(), 8));

    // Write all discard blocks
    for (auto discard_block : dl_user_data.discard_blocks.value()) {
      VERIFY_WRITE(encoder.pack(discard_block.pdcp_sn_start, 24));
      VERIFY_WRITE(encoder.pack(discard_block.block_size, 8));
    }
  }

  // Add padding such that length is (n*4-2) octets, where n is a positive integer.
  while (((out_buf.length() - start_len) + 2) % 4) {
    VERIFY_WRITE(out_buf.append(0x0));
  }

  return true;
};

bool nrup_packing::unpack(nru_dl_data_delivery_status& dl_data_delivery_status, byte_buffer_view container) const
{
  return false;
};

bool nrup_packing::pack(byte_buffer& out_buf, const nru_dl_data_delivery_status& dl_data_delivery_status) const
{
  return false;
};

} // namespace srsran
