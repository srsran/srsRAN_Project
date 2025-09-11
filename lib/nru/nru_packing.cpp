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

#include "srsran/nru/nru_packing.h"
#include "srsran/support/bit_encoding.h"

using namespace srsran;

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

nru_pdu_type nru_packing::get_pdu_type(byte_buffer_view container)
{
  srsran_assert(!container.empty(), "Cannot get PDU type of empty PDU");
  uint8_t field_value = container[0] >> 4;
  return uint_to_nru_pdu_type(field_value);
}

bool nru_packing::unpack(nru_dl_user_data& dl_user_data, byte_buffer_view container) const
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
  if (uint_to_nru_pdu_type(pdu_type) != nru_pdu_type::dl_user_data) {
    logger.error("Failed to unpack DL user data: Invalid pdu_type={}", uint_to_nru_pdu_type(pdu_type));
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

  // Spare (v16.1.0)
  spare = {};
  VERIFY_READ(decoder.unpack(spare, 3));
  if (spare != 0) {
    logger.error("Failed to unpack DL user data: Spare bits set in second octet. value={:#x}", spare);
    return false;
  }

  // Request OutOfSeq Report
  decoder.unpack(dl_user_data.request_out_of_seq_report, 1);

  // Report Delivered
  decoder.unpack(dl_user_data.report_delivered, 1);

  // User data existence flag
  decoder.unpack(dl_user_data.user_data_existence_flag, 1);

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

  // DL report NR PDCP PDU SN
  if (dl_user_data.report_delivered) {
    VERIFY_READ(decoder.unpack(dl_user_data.dl_report_pdcp_sn, 24));
  }

  return true;
}

bool nru_packing::pack(byte_buffer& out_buf, const nru_dl_user_data& dl_user_data) const
{
  size_t      start_len = out_buf.length();
  bit_encoder encoder{out_buf};

  // PDU Type
  VERIFY_WRITE(encoder.pack(nru_pdu_type_to_uint(nru_pdu_type::dl_user_data), 4));

  // Spare (v15.2.0)
  VERIFY_WRITE(encoder.pack(0, 1));

  // DL discard blocks
  bool dl_discard_blocks = dl_user_data.discard_blocks.has_value() && !dl_user_data.discard_blocks.value().empty();
  VERIFY_WRITE(encoder.pack(dl_discard_blocks, 1));

  // DL flush
  VERIFY_WRITE(encoder.pack(dl_user_data.dl_discard_pdcp_sn.has_value(), 1));

  // Report polling
  VERIFY_WRITE(encoder.pack(dl_user_data.report_polling, 1));

  // Spare (v16.1.0)
  VERIFY_WRITE(encoder.pack(0, 3));

  // Request OutOfSeq Report
  VERIFY_WRITE(encoder.pack(dl_user_data.request_out_of_seq_report, 1));

  // Report Delivered
  VERIFY_WRITE(encoder.pack(dl_user_data.report_delivered, 1));

  // User data existence flag
  VERIFY_WRITE(encoder.pack(dl_user_data.user_data_existence_flag, 1));

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

  // DL report NR PDCP PDU SN
  if (dl_user_data.report_delivered) {
    VERIFY_WRITE(encoder.pack(dl_user_data.dl_report_pdcp_sn, 24));
  }

  // Add padding such that length is (n*4-2) octets, where n is a positive integer.
  while (((out_buf.length() - start_len) + 2) % 4) {
    VERIFY_WRITE(out_buf.append(0x0));
  }

  return true;
}

bool nru_packing::unpack(nru_dl_data_delivery_status& dl_data_delivery_status, byte_buffer_view container) const
{
  if (container.empty()) {
    logger.error("Failed to unpack DL data delivery status: pdu_len=0");
    return false;
  }

  if ((container.length() + 2) % 4 != 0) {
    logger.error("Failed to unpack DL data delivery status: pdu_len={} != n*4-2", container.length());
    return false;
  }

  bit_decoder decoder{container};

  // PDU Type
  uint8_t pdu_type = {};
  VERIFY_READ(decoder.unpack(pdu_type, 4));
  if (uint_to_nru_pdu_type(pdu_type) != nru_pdu_type::dl_data_delivery_status) {
    logger.error("Failed to unpack DL data delivery status: Invalid pdu_type={}", uint_to_nru_pdu_type(pdu_type));
    return false;
  }

  // Highest transmitted NR PDCP SN indication
  bool highest_transmitted_pdcp_sn_indication = {};
  VERIFY_READ(decoder.unpack(highest_transmitted_pdcp_sn_indication, 1));

  // Highest delivered NR PDCP SN indication
  bool highest_delivered_pdcp_sn_indication = {};
  VERIFY_READ(decoder.unpack(highest_delivered_pdcp_sn_indication, 1));

  // Final frame indication
  VERIFY_READ(decoder.unpack(dl_data_delivery_status.final_frame_ind, 1));

  // Lost packet report
  bool lost_packet_report = {};
  VERIFY_READ(decoder.unpack(lost_packet_report, 1));

  // Spare (v15.2.0)
  uint8_t spare = {};
  VERIFY_READ(decoder.unpack(spare, 4));
  if (spare != 0) {
    logger.error("Failed to unpack DL data delivery status: Spare bits set in second octet. value={:#x}", spare);
    return false;
  }

  // Data rate indication
  bool data_rate_indication = {};
  VERIFY_READ(decoder.unpack(data_rate_indication, 1));

  // Highest retransmitted NR PDCP indication
  bool highest_retransmitted_pdcp_sn_indication = {};
  VERIFY_READ(decoder.unpack(highest_retransmitted_pdcp_sn_indication, 1));

  // Highest delivered retransmitted NR PDCP indication
  bool highest_delivered_retransmitted_pdcp_sn_indication = {};
  VERIFY_READ(decoder.unpack(highest_delivered_retransmitted_pdcp_sn_indication, 1));

  // Cause report
  bool cause_report = {};
  VERIFY_READ(decoder.unpack(cause_report, 1));

  // Desired buffer size for the data radio bearer
  VERIFY_READ(decoder.unpack(dl_data_delivery_status.desired_buffer_size_for_drb, 32));

  // Desired data rate (if present)
  if (data_rate_indication) {
    uint32_t desired_data_rate = {};
    VERIFY_READ(decoder.unpack(desired_data_rate, 32));
    dl_data_delivery_status.desired_data_rate = desired_data_rate;
  }

  // Lost SN ranges (if present)
  if (lost_packet_report) {
    // Number of lost NR-U Sequence Number ranges reported (if present)
    uint8_t nof_lost_sn_ranges = {};
    VERIFY_READ(decoder.unpack(nof_lost_sn_ranges, 8));

    if (nof_lost_sn_ranges > nru_max_nof_lost_nru_sn_ranges) {
      logger.error(
          "Failed to unpack DL data delivery status: nof_lost_sn_ranges={} exceeds max_nof_lost_nru_sn_ranges={}",
          nof_lost_sn_ranges,
          nru_max_nof_lost_nru_sn_ranges);
      return false;
    }

    // Read all lost NR-U SN ranges
    dl_data_delivery_status.lost_nru_sn_ranges = nru_lost_nru_sn_ranges{};
    for (uint8_t i = 0; i < nof_lost_sn_ranges; i++) {
      dl_data_delivery_status.lost_nru_sn_ranges->push_back(nru_lost_nru_sn_range{});
      VERIFY_READ(decoder.unpack(dl_data_delivery_status.lost_nru_sn_ranges.value().back().nru_sn_start, 24));
      VERIFY_READ(decoder.unpack(dl_data_delivery_status.lost_nru_sn_ranges.value().back().nru_sn_end, 24));
    }
  }

  // Highest successfully delivered NR PDCP Sequence Number (if present)
  if (highest_delivered_pdcp_sn_indication) {
    uint32_t highest_delivered_pdcp_sn = {};
    VERIFY_READ(decoder.unpack(highest_delivered_pdcp_sn, 24));
    dl_data_delivery_status.highest_delivered_pdcp_sn = highest_delivered_pdcp_sn;
  }

  // Highest transmitted NR PDCP Sequence Number (if present)
  if (highest_transmitted_pdcp_sn_indication) {
    uint32_t highest_transmitted_pdcp_sn = {};
    VERIFY_READ(decoder.unpack(highest_transmitted_pdcp_sn, 24));
    dl_data_delivery_status.highest_transmitted_pdcp_sn = highest_transmitted_pdcp_sn;
  }

  // Cause Value (if present)
  if (cause_report) {
    uint32_t cause_value = {};
    VERIFY_READ(decoder.unpack(cause_value, 8));
    dl_data_delivery_status.cause_value = cause_value;
  }

  // Highest successfully delivered retransmitted NR PDCP Sequence Number (if present)
  if (highest_delivered_retransmitted_pdcp_sn_indication) {
    uint32_t highest_delivered_retransmitted_pdcp_sn = {};
    VERIFY_READ(decoder.unpack(highest_delivered_retransmitted_pdcp_sn, 24));
    dl_data_delivery_status.highest_delivered_retransmitted_pdcp_sn = highest_delivered_retransmitted_pdcp_sn;
  }

  // Highest retransmitted NR PDCP Sequence Number (if present)
  if (highest_retransmitted_pdcp_sn_indication) {
    uint32_t highest_retransmitted_pdcp_sn = {};
    VERIFY_READ(decoder.unpack(highest_retransmitted_pdcp_sn, 24));
    dl_data_delivery_status.highest_retransmitted_pdcp_sn = highest_retransmitted_pdcp_sn;
  }

  return true;
}

bool nru_packing::pack(byte_buffer& out_buf, const nru_dl_data_delivery_status& dl_data_delivery_status) const
{
  size_t      start_len = out_buf.length();
  bit_encoder encoder{out_buf};

  // PDU Type
  VERIFY_WRITE(encoder.pack(nru_pdu_type_to_uint(nru_pdu_type::dl_data_delivery_status), 4));

  // Highest transmitted NR PDCP SN indication
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.highest_transmitted_pdcp_sn.has_value(), 1));

  // Highest delivered NR PDCP SN indication
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.highest_delivered_pdcp_sn.has_value(), 1));

  // Final frame indication
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.final_frame_ind, 1));

  // Lost packet report
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.lost_nru_sn_ranges.has_value(), 1));

  // Spare (v15.2.0)
  VERIFY_WRITE(encoder.pack(0, 4));

  // Data rate indication
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.desired_data_rate.has_value(), 1));

  // Highest retransmitted NR PDCP indication
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.highest_retransmitted_pdcp_sn.has_value(), 1));

  // Highest delivered retransmitted NR PDCP indication
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.highest_delivered_retransmitted_pdcp_sn.has_value(), 1));

  // Cause report
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.cause_value.has_value(), 1));

  // Desired buffer size for the data radio bearer
  VERIFY_WRITE(encoder.pack(dl_data_delivery_status.desired_buffer_size_for_drb, 32));

  // Desired data rate (if present)
  if (dl_data_delivery_status.desired_data_rate.has_value()) {
    VERIFY_WRITE(encoder.pack(dl_data_delivery_status.desired_data_rate.value(), 32));
  }

  // Lost SN ranges (if present)
  if (dl_data_delivery_status.lost_nru_sn_ranges.has_value()) {
    // Number of lost NR-U Sequence Number ranges reported (if present)
    VERIFY_WRITE(encoder.pack(dl_data_delivery_status.lost_nru_sn_ranges.value().size(), 8));

    // Write all ranges
    for (auto range : dl_data_delivery_status.lost_nru_sn_ranges.value()) {
      VERIFY_WRITE(encoder.pack(range.nru_sn_start, 24));
      VERIFY_WRITE(encoder.pack(range.nru_sn_end, 24));
    }
  }

  // Highest successfully delivered NR PDCP Sequence Number (if present)
  if (dl_data_delivery_status.highest_delivered_pdcp_sn.has_value()) {
    VERIFY_WRITE(encoder.pack(dl_data_delivery_status.highest_delivered_pdcp_sn.value(), 24));
  }

  // Highest transmitted NR PDCP Sequence Number (if present)
  if (dl_data_delivery_status.highest_transmitted_pdcp_sn.has_value()) {
    VERIFY_WRITE(encoder.pack(dl_data_delivery_status.highest_transmitted_pdcp_sn.value(), 24));
  }

  // Cause Value (if present)
  if (dl_data_delivery_status.cause_value.has_value()) {
    VERIFY_WRITE(encoder.pack(dl_data_delivery_status.cause_value.value(), 8));
  }

  // Highest successfully delivered retransmitted NR PDCP Sequence Number (if present)
  if (dl_data_delivery_status.highest_delivered_retransmitted_pdcp_sn.has_value()) {
    VERIFY_WRITE(encoder.pack(dl_data_delivery_status.highest_delivered_retransmitted_pdcp_sn.value(), 24));
  }

  // Highest retransmitted NR PDCP Sequence Number (if present)
  if (dl_data_delivery_status.highest_retransmitted_pdcp_sn.has_value()) {
    VERIFY_WRITE(encoder.pack(dl_data_delivery_status.highest_retransmitted_pdcp_sn.value(), 24));
  }

  // Add padding such that length is (n*4-2) octets, where n is a positive integer.
  while (((out_buf.length() - start_len) + 2) % 4) {
    VERIFY_WRITE(out_buf.append(0x0));
  }

  return true;
}
