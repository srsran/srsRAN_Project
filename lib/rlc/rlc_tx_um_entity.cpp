/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_tx_um_entity.h"

using namespace srsgnb;

rlc_tx_um_entity::rlc_tx_um_entity(du_ue_index_t                        du_index,
                                   lcid_t                               lcid,
                                   const rlc_um_config&                 config,
                                   rlc_tx_upper_layer_control_notifier& upper_cn) :
  rlc_tx_entity(du_index, lcid, upper_cn),
  cfg(config),
  mod((cfg.sn_field_length == rlc_um_sn_size::size6bits) ? 64 : 4096),
  head_len_full(rlc_um_pdu_header_size_complete_sdu),
  head_len_first(cfg.sn_field_length == rlc_um_sn_size::size6bits ? rlc_um_pdu_header_size_6bit_sn_no_so
                                                                  : rlc_um_pdu_header_size_12bit_sn_no_so),
  head_len_not_first(cfg.sn_field_length == rlc_um_sn_size::size6bits ? rlc_um_pdu_header_size_6bit_sn_with_so
                                                                      : rlc_um_pdu_header_size_12bit_sn_with_so)
{
}

/// Interface for higher layers to pass SDUs
/// into the RLC entity.
void rlc_tx_um_entity::handle_sdu(rlc_sdu sdu)
{
  size_t sdu_length = sdu.buf.length();
  logger.log_info(sdu.buf.begin(),
                  sdu.buf.end(),
                  "Tx SDU (length: {} B, PDCP SN: {}, enqueued SDUs: {}",
                  sdu.buf.length(),
                  sdu.pdcp_sn,
                  sdu_queue.size_sdus());
  if (sdu_queue.write(sdu)) {
    metrics_add_sdus(1, sdu_length);
  } else {
    logger.log_warning(
        "Dropped Tx SDU (length: {} B, PDCP SN: {}, enqueued SDUs: {}", sdu_length, sdu.pdcp_sn, sdu_queue.size_sdus());
    metrics_add_lost_sdus(1);
  }
}

/// Interface for higher layers to pass PDUs
/// into the RLC entity.
rlc_byte_buffer rlc_tx_um_entity::pull_pdu(uint32_t nof_bytes)
{
  logger.log_debug("PDU requested with up to {} B", nof_bytes);

  // Check available space -- we need at least the minimum header + 1 payload Byte
  if (nof_bytes < head_len_full + 1) {
    logger.log_info("Cannot build a PDU with {} B", nof_bytes);
    return {};
  }

  // Multiple threads can read from the SDU queue and change the
  // RLC UM Tx state (current SDU, TX_Next and next_so).
  // As such we need to lock to access these variables.
  std::lock_guard<std::mutex> lock(mutex);

  // Get a new SDU, if none is currently being transmitted
  if (sdu.buf.empty()) {
    if (not sdu_queue.read(sdu)) {
      logger.log_info("No data available to be sent");
      return {};
    }
    srsran_sanity_check(next_so == 0, "New Tx SDU, but next_so is not 0 (next_so = {})", next_so);
  }

  rlc_um_pdu_header header = {};
  header.sn                = TX_Next;
  header.sn_size           = cfg.sn_field_length;
  header.so                = next_so;

  // Get SI and expected header size
  uint32_t head_len = 0;
  if (not get_si_and_expected_header_size(next_so, sdu.buf.length(), nof_bytes, header.si, head_len)) {
    logger.log_info("Not enough space to send header");
    return {};
  }

  // Pack header
  byte_buffer header_buf = {};
  if (!rlc_um_write_data_pdu_header(header, header_buf)) {
    logger.log_error("Could not pack RLC UM header. {} B available", nof_bytes);
    return {};
  }
  srsran_sanity_check(head_len == header_buf.length(),
                      "Header length and expected header length do not match ({} != {})",
                      header_buf.length(),
                      head_len);

  if (nof_bytes <= head_len) {
    logger.log_info("Cannot build a PDU - {} B available, {} B required for header", nof_bytes, head_len);
    return {};
  }

  // Calculate the amount of data to move
  uint32_t space   = nof_bytes - head_len;
  uint32_t to_move = space >= sdu.buf.length() - next_so ? sdu.buf.length() - next_so : space;

  // Log
  logger.log_debug("adding {} - ({}/{})", to_string(header.si).c_str(), to_move, sdu.buf.length());

  // Move data from SDU to PDU
  rlc_byte_buffer pdu_buf = {};
  pdu_buf.set_header(std::move(header_buf));
  pdu_buf.set_payload(sdu.buf.shared_view(next_so, to_move));

  // Release SDU if needed
  if (header.si == rlc_si_field::full_sdu || header.si == rlc_si_field::last_segment) {
    sdu.buf.clear();
    next_so = 0;
  } else {
    // advance SO offset
    next_so += to_move;
  }

  // Update SN if needed
  if (header.si == rlc_si_field::last_segment) {
    TX_Next = (TX_Next + 1) % mod;
  }

  // Assert number of bytes
  srsran_assert(pdu_buf.length() <= nof_bytes,
                "Error while packing MAC PDU (more bytes written ({}) than expected ({})!",
                pdu_buf.length(),
                nof_bytes);

  if (header.si == rlc_si_field::full_sdu) {
    // log without SN
    logger.log_info(pdu_buf.begin(), pdu_buf.end(), "Tx PDU ({} B)", pdu_buf.length());
  } else {
    logger.log_info(pdu_buf.begin(), pdu_buf.end(), "Tx PDU SN={} ({} B)", header.sn, pdu_buf.length());
  }

  metrics_add_pdus(1, pdu_buf.length());

  debug_state();

  return pdu_buf;
}

/// Helper to get SI of an PDU
bool rlc_tx_um_entity::get_si_and_expected_header_size(uint32_t      so,
                                                       uint32_t      sdu_len,
                                                       uint32_t      nof_bytes,
                                                       rlc_si_field& si,
                                                       uint32_t&     head_len) const
{
  if (so == 0) {
    // Can we transmit the SDU fully?
    if (sdu_len <= nof_bytes - head_len_full) {
      // We can transmit the SDU fully
      si       = rlc_si_field::full_sdu;
      head_len = head_len_full;
    } else {
      // We can't transmit the SDU fully
      si       = rlc_si_field::first_segment;
      head_len = head_len_first;
      // Sanity check, do we have enough bytes for header?
      if (nof_bytes <= head_len_first) {
        return false;
      }
    }
  } else {
    // Sanity check, do we have enough bytes for header?
    if (nof_bytes <= head_len_not_first) {
      return false;
    }
    head_len = head_len_not_first;
    // Can we transmit the SDU segment fully?
    if (sdu_len - next_so <= nof_bytes - head_len_not_first) {
      // We can transmit the SDU fully
      si = rlc_si_field::last_segment;
    } else {
      // We can't transmit the SDU fully
      si = rlc_si_field::neither_first_nor_last_segment;
    }
  }
  return true;
}

/// Update the buffer status information
void rlc_tx_um_entity::get_buffer_state(uint32_t& bytes)
{
  std::lock_guard<std::mutex> lock(mutex);

  // minimum bytes needed to tx all queued SDUs + each header
  uint32_t gross_queue_bytes = sdu_queue.size_bytes() + sdu_queue.size_sdus() * head_len_full;

  // minimum bytes needed to tx SDU under segmentation + header (if applicable)
  uint32_t gross_segment_bytes = 0;
  if (not sdu.buf.empty()) {
    gross_segment_bytes = (sdu.buf.length() - next_so) + head_len_not_first;
  }

  bytes = gross_queue_bytes + gross_segment_bytes;
}
