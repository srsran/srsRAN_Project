/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_TX_UM_ENTITY_H
#define SRSGNB_RLC_TX_UM_ENTITY_H

#include "rlc_sdu_queue.h"
#include "rlc_tx_entity.h"
#include "rlc_um_pdu.h"

namespace srsgnb {

class rlc_tx_um_entity : public rlc_tx_entity
{
private:
  // Config storage
  const rlc_um_config cfg;

  // TX SDU buffers
  rlc_sdu_queue sdu_queue;
  rlc_sdu       sdu;
  uint32_t      next_so = 0; // The segment offset for the next generated PDU

  // Mutexes
  std::mutex mutex;

  uint32_t TX_Next = 0; // send state as defined in TS 38.322 v15.3 Section 7
                        // It holds the value of the SN to be assigned for the next newly generated UMD PDU with
                        // segment. It is initially set to 0, and is updated after the UM RLC entity submits a UMD PDU
                        // including the last segment of an RLC SDU to lower layers.

  const uint32_t mod; // Tx counter modulus

  // Header sizes are computed during configure based on SN length
  const uint32_t head_len_full      = 0;
  const uint32_t head_len_first     = 0;
  const uint32_t head_len_not_first = 0;

public:
  rlc_tx_um_entity(du_ue_index_t                        du_index,
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
  {}

  /*
   * Interfaces for higher layers
   */
  void handle_sdu(rlc_sdu sdu) override
  {
    if (sdu_queue.write(sdu)) {
      logger.log_info(
          "Tx SDU (length: {} B, PDCP SN: {}, enqueued SDUs: {}", sdu.buf.length(), sdu.pdcp_sn, sdu_queue.size_sdus());
    } else {
      logger.log_warning("Dropped Tx SDU (length: {} B, PDCP SN: {}, enqueued SDUs: {}",
                         sdu.buf.length(),
                         sdu.pdcp_sn,
                         sdu_queue.size_sdus());
    }
  }

  /*
   * Interfaces for lower layers
   */
  rlc_byte_buffer pull_pdu(uint32_t nof_bytes) override
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

    rlc_um_pdu_header header = {};
    header.sn                = TX_Next;
    header.sn_size           = cfg.sn_field_length;

    // Get a new SDU, if none is currently being transmitted
    if (sdu.buf.empty()) {
      if (not sdu_queue.read(sdu)) {
        logger.log_info("No data available to be sent");
        return {};
      }
      srsran_sanity_check(next_so == 0, "New Tx SDU, but next_so is not 0 (next_so = {})", next_so);
    }

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

    if (nof_bytes <= head_len + 1) {
      logger.log_info("Cannot build a PDU - {} B available, {} B required for header", nof_bytes, head_len);
      return {};
    }

    // Calculate the amount of data to move
    uint32_t space   = nof_bytes - head_len;
    uint32_t to_move = space >= sdu.buf.length() ? sdu.buf.length() : space;

    // Log
    logger.log_debug("adding {} - ({}/{})", to_string(header.si).c_str(), to_move, sdu.buf.length());

    // Move data from SDU to PDU
    rlc_byte_buffer pdu_buf = {};
    pdu_buf.set_header(std::move(header_buf));
    pdu_buf.set_payload(sdu.buf, next_so, to_move);

    // advance SO offset
    next_so += to_move;

    // Release SDU and update SN if needed
    if (header.si == rlc_si_field::last_segment) {
      sdu.buf.clear();
      TX_Next = (TX_Next + 1) % mod;
      next_so = 0;
    }

    // Assert number of bytes
    srsran_assert(pdu_buf.length() <= nof_bytes,
                  "Error while packing MAC PDU (more bytes written ({}) than expected ({})!",
                  pdu_buf.length(),
                  nof_bytes);

    if (header.si == rlc_si_field::full_sdu) {
      // log without SN (TODO)
      // logger.log_info(pdu_buf, "Tx PDU (%d B)", pdu->N_bytes);
    } else {
      // TODO
      // logger.log_info(payload, ret, "Tx PDU SN=%d (%d B)", header.sn, pdu->N_bytes);
    }

    debug_state();

    return pdu_buf;
  }

  bool get_si_and_expected_header_size(uint32_t      so,
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

  void get_buffer_state(uint32_t& bytes) override
  {
    // TODO
    bytes = 0;
  }

  void debug_state() { logger.log_debug("TX_Next={}, next_so={}", TX_Next, next_so); }
};

} // namespace srsgnb

#endif
