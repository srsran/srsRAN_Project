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
  decoder.unpack(pdu_type, 4);
  if (uint_to_nrup_pdu_type(pdu_type) != nrup_pdu_type::dl_user_data) {
    logger.error("Failed to unpack DL user data: Invalid pdu_type={}", uint_to_nrup_pdu_type(pdu_type));
    return false;
  }

  // Spare (v15.2.0)
  uint8_t spare = {};
  decoder.unpack(spare, 1);
  if (spare != 0) {
    logger.error("Failed to unpack DL user data: Spare bit set in first octet. value={:#x}", spare);
    return false;
  }

  // DL discard blocks
  bool dl_discard_blocks = {};
  decoder.unpack(dl_discard_blocks, 1);

  // DL flush
  bool dl_flush = {};
  decoder.unpack(dl_flush, 1);

  // Report polling
  decoder.unpack(dl_user_data.report_polling, 1);

  // Spare (v15.2.0)
  spare = {};
  decoder.unpack(spare, 6);
  if (spare != 0) {
    logger.error("Failed to unpack DL user data: Spare bits set in first octet. value={:#x}", spare);
    return false;
  }

  // Assistance Info Report Polling Flag
  decoder.unpack(dl_user_data.assist_info_report_polling_flag, 1);

  // Retransmission flag
  decoder.unpack(dl_user_data.retransmission_flag, 1);

  // NR-U Sequence Number
  decoder.unpack(dl_user_data.nru_sn, 24);

  // DL discard NR PDCP PDU SN (if present)
  if (dl_flush) {
    uint32_t dl_discard_pdcp_sn = {};
    decoder.unpack(dl_discard_pdcp_sn, 24);
    dl_user_data.dl_discard_pdcp_sn = dl_discard_pdcp_sn;
  }

  // DL discard number of blocks
  if (dl_discard_blocks) {
    uint8_t nof_discard_blocks = {};
    decoder.unpack(nof_discard_blocks, 8);

    if (nof_discard_blocks > nru_max_nof_pdcp_sn_discard_blocks) {
      logger.error("Failed to unpack DL user data: nof_discard_blocks={} exceeds max_nof_pdcp_sn_discard_blocks={}",
                   nof_discard_blocks,
                   nru_max_nof_pdcp_sn_discard_blocks);
      return false;
    }

    dl_user_data.discard_blocks = nru_pdcp_sn_discard_blocks{};
    for (uint8_t i = 0; i < nof_discard_blocks; i++) {
      dl_user_data.discard_blocks.value().push_back(nru_pdcp_sn_discard_block{});
      decoder.unpack(dl_user_data.discard_blocks.value().back().pdcp_sn_start, 24);
      decoder.unpack(dl_user_data.discard_blocks.value().back().block_size, 8);
    }
  }

  return true;
};

bool nrup_packing::pack(byte_buffer& out_buf, const nru_dl_user_data& dl_user_data) const
{
  return false;
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
