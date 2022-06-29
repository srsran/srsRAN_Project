/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_TX_TM_ENTITY_H
#define SRSGNB_RLC_TX_TM_ENTITY_H

#include "rlc_sdu_queue.h"
#include "rlc_tx_entity.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

class rlc_tx_tm_entity : public rlc_tx_entity
{
private:
  rlc_sdu_queue sdu_queue;

public:
  rlc_tx_tm_entity(du_ue_index_t du_index, lcid_t lcid, rlc_tx_upper_layer_control_notifier& upper_cn) :
    rlc_tx_entity(du_index, lcid, upper_cn)
  {}

  // Interfaces for higher layers
  void handle_sdu(rlc_sdu sdu) override
  {
    if (sdu_queue.write(sdu)) {
      logger.log_info("Tx SDU (length: {} B, enqueued SDUs: {}", sdu.buf.length(), sdu_queue.size_sdus());
    } else {
      logger.log_warning("Dropped Tx SDU (length: {} B, enqueued SDUs: {}", sdu.buf.length(), sdu_queue.size_sdus());
    }
  }

  // Interfaces for lower layers
  bool pull_pdu(rlc_byte_buffer& pdu, uint32_t nof_bytes) override
  {
    srsran_assert(pdu.empty(), "MAC PDU payload provided is not empty.");

    if (sdu_queue.is_empty()) {
      logger.log_info("No data available to be sent. Provided space ({} B)", nof_bytes);
      return false;
    }

    uint32_t front_size = {};
    if (not sdu_queue.front_size_bytes(front_size)) {
      logger.log_warning("Could not get size of front packet. Provided space ({} B)", nof_bytes);
      return false;
    }

    if (front_size > nof_bytes) {
      logger.log_info("Tx PDU size larger than provided space ({} > {})", front_size, nof_bytes);
      return false;
    }

    rlc_sdu sdu = {};
    if (not sdu_queue.read(sdu)) {
      logger.log_warning("Could not read SDU, but queue should not be empty. Provided space ({} B)", nof_bytes);
      return false;
    }

    size_t sdu_size = sdu.buf.length();
    if (sdu_size != front_size) {
      logger.log_error("Tx PDU size different than front size ({} != {})", sdu_size, front_size);
      return false;
    }

    // In TM there is no header, just pass the plain SDU
    pdu.set_payload(sdu.buf);
    logger.log_info("Tx PDU ({} B). Provided space ({} B)", sdu_size, nof_bytes);
    return true;
  }

  void get_buffer_state(uint32_t& bytes) override
  {
    // TODO
    bytes = 0;
  }
};

} // namespace srsgnb

#endif // SRSGNB_RLC_TX_TM_ENTITY_H
