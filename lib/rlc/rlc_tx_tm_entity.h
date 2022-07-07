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
  {
  }

  // Interfaces for higher layers
  void handle_sdu(rlc_sdu sdu) override
  {
    size_t sdu_length = sdu.buf.length();
    logger.log_info(sdu.buf.begin(),
                    sdu.buf.end(),
                    "Tx SDU (length: {} B, enqueued SDUs: {})",
                    sdu.buf.length(),
                    sdu_queue.size_sdus());
    if (sdu_queue.write(sdu)) {
      metrics_add_sdus(1, sdu_length);
    } else {
      logger.log_warning("Dropped Tx SDU (length: {} B, enqueued SDUs: {})", sdu.buf.length(), sdu_queue.size_sdus());
      metrics_add_lost_sdus(1);
    }
  }

  // Interfaces for lower layers
  rlc_byte_buffer pull_pdu(uint32_t nof_bytes) override
  {
    if (sdu_queue.is_empty()) {
      logger.log_info("No data available to be sent. Provided space ({} B)", nof_bytes);
      return {};
    }

    uint32_t front_size = {};
    if (not sdu_queue.front_size_bytes(front_size)) {
      logger.log_warning("Could not get size of front packet. Provided space ({} B)", nof_bytes);
      return {};
    }

    if (front_size > nof_bytes) {
      logger.log_info("Tx PDU size larger than provided space ({} > {})", front_size, nof_bytes);
      return {};
    }

    rlc_sdu sdu = {};
    if (not sdu_queue.read(sdu)) {
      logger.log_warning("Could not read SDU, but queue should not be empty. Provided space ({} B)", nof_bytes);
      return {};
    }

    size_t sdu_size = sdu.buf.length();
    srsran_sanity_check(
        sdu_size == front_size, "Tx PDU size different than front size ({} != {})", sdu_size, front_size);

    // In TM there is no header, just pass the plain SDU
    rlc_byte_buffer pdu = {};
    pdu.set_payload(std::move(sdu.buf));
    logger.log_info("Tx PDU ({} B). Provided space ({} B)", sdu_size, nof_bytes);
    metrics_add_pdus(1, pdu.length());
    return pdu;
  }

  void get_buffer_state(uint32_t& bytes) override
  {
    // TODO
    bytes = 0;
  }
};

} // namespace srsgnb

#endif // SRSGNB_RLC_TX_TM_ENTITY_H
