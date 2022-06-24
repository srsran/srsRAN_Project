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

  uint32_t UM_Window_Size;
  uint32_t mod; // Rx counter modulus

  uint32_t head_len_first = 0, head_len_segment = 0; // are computed during configure based on SN length

public:
  rlc_tx_um_entity(du_ue_index_t du_index, lcid_t lcid, rlc_tx_upper_layer_control_notifier& upper_cn) :
    rlc_tx_entity(du_index, lcid, upper_cn)
  {
    mod            = (cfg.um_nr.sn_field_length == rlc_um_nr_sn_size_t::size6bits) ? 64 : 4096;
    UM_Window_Size = (cfg.um_nr.sn_field_length == rlc_um_nr_sn_size_t::size6bits) ? 32 : 2048;

    // calculate header sizes for configured SN length
    rlc_um_pdu_header header = {};
    header.si                = rlc_si_field::first_segment;
    header.so                = 0;
    head_len_first           = rlc_um_nr_packed_length(header);

    header.so        = 1;
    head_len_segment = rlc_um_nr_packed_length(header);
  }

  /*
   * Interfaces for higher layers
   */
  void handle_sdu(rlc_sdu sdu) override
  {
    if (sdu_queue.write(*sdu)) {
      logger.log_info("Tx SDU (length: {} B, PDCP SN: {}, enqueued SDUs: {}",
                      sdu->buf.length(),
                      sdu->pdcp_sn,
                      sdu_queue.size_sdus());
    } else {
      logger.log_warning("Dropped Tx SDU (length: {} B, PDCP SN: {}, enqueued SDUs: {}",
                         sdu->buf.length(),
                         sdu->pdcp_sn,
                         sdu_queue.size_sdus());
    }
  }

  /*
   * Interfaces for lower layers
   */
  rlc_byte_buffer pull_pdu(uint32_t nof_bytes) override
  {
    logger.log_debug("PDU requested with up to {} B", nof_bytes);

    // Check available space -- we need at least rlc_um_pdu_header_size + 1 payload Byte
    if (nof_bytes < rlc_um_pdu_header_size + 1) {
      logger.log_info("Cannot build a PDU with {} B", nof_bytes);
      return false;
    }

    std::lock_guard<std::mutex> lock(mutex);

    rlc_um_pdu_header header = {};
    header.si                = rlc_si_field::full_sdu;
    header.sn                = TX_Next;
    header.sn_size           = cfg.um_nr.sn_field_length;

    if (sdu.buf.empty()) {
      if (not sdu_queue.read(sdu)) {
        logger.log_info("No data available to be sent");
        return false;
      }
      next_so = 0;

      // Check for full SDU case
      if (sdu.buf.length() <= nof_bytes - rlc_um_pdu_header_size) {
        header.si = rlc_si_field::full_sdu;
      } else {
        header.si = rlc_si_field::first_segment;
      }
    } else {
      // The SDU is not new; check for last segment
      if (sdu.buf.length() - next_so <= nof_bytes - head_len_segment) {
        header.si = rlc_si_field::last_segment;
      } else {
        header.si = rlc_si_field::neither_first_nor_last_segment;
      }
    }
    header.so = next_so;

    // Calculate actual header length
    size_t head_len = rlc_um_nr_packed_length(header);
    if (nof_bytes <= head_len + 1) {
      logger.log_info("Cannot build a PDU - {} B available, {} B required for header", nof_bytes, head_len);
      return 0;
    }

    // Calculate the amount of data to move
    uint32_t space   = nof_bytes - head_len;
    uint32_t to_move = space >= sdu.buf.length() ? sdu.buf.length() : space;

    // Log
    logger.log_debug("adding {} - ({}/{})", to_string(header.si).c_str(), to_move, sdu.buf.length());

    // Move data from SDU to PDU
    uint8_t* pdu_ptr = pdu->msg;
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;

    // Release SDU if emptied
    if (tx_sdu->N_bytes == 0) {
      tx_sdu.reset();
    }

    // advance SO offset
    next_so += to_move;

    // Update SN if needed
    if (header.si == rlc_si_field::last_segment) {
      TX_Next = (TX_Next + 1) % mod;
      next_so = 0;
    }

    // Add header and TX
    rlc_um_nr_write_data_pdu_header(header, pdu.get());
    memcpy(payload, pdu->msg, pdu->N_bytes);
    uint32_t ret = pdu->N_bytes;

    // Assert number of bytes
    srsran_expect(
        ret <= nof_bytes, "Error while packing MAC PDU (more bytes written (%d) than expected (%d)!", ret, nof_bytes);

    if (header.si == rlc_si_field::full_sdu) {
      // log without SN
      RlcHexInfo(payload, ret, "Tx PDU (%d B)", pdu->N_bytes);
    } else {
      RlcHexInfo(payload, ret, "Tx PDU SN=%d (%d B)", header.sn, pdu->N_bytes);
    }

    debug_state();

    return ret;
    return true;
  }

  void get_buffer_state(uint32_t& bytes) override
  {
    // TODO
    bytes = 0;
  }
};

} // namespace srsgnb

#endif
