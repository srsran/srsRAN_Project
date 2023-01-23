/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rlc_sdu_queue.h"
#include "rlc_tx_entity.h"
#include "srsgnb/support/srsgnb_assert.h"

namespace srsgnb {

class rlc_tx_tm_entity : public rlc_tx_entity
{
private:
  rlc_sdu_queue sdu_queue;

public:
  rlc_tx_tm_entity(du_ue_index_t                        du_index,
                   rb_id_t                              rb_id,
                   rlc_tx_upper_layer_data_notifier&    upper_dn_,
                   rlc_tx_upper_layer_control_notifier& upper_cn_,
                   rlc_tx_lower_layer_notifier&         lower_dn_) :
    rlc_tx_entity(du_index, rb_id, upper_dn_, upper_cn_, lower_dn_)
  {
    metrics.metrics_set_mode(rlc_mode::tm);
  }

  // Interfaces for higher layers
  // TS 38.322 v16.2.0 Sec. 5.2.1.1
  void handle_sdu(rlc_sdu sdu) override
  {
    size_t sdu_length = sdu.buf.length();
    logger.log_info(sdu.buf.begin(),
                    sdu.buf.end(),
                    "Tx SDU (length: {} B, enqueued SDUs: {})",
                    sdu.buf.length(),
                    sdu_queue.size_sdus());
    if (sdu_queue.write(sdu)) {
      metrics.metrics_add_sdus(1, sdu_length);
      handle_buffer_state_update();
    } else {
      logger.log_info("Dropped Tx SDU (length: {} B, enqueued SDUs: {})", sdu.buf.length(), sdu_queue.size_sdus());
      metrics.metrics_add_lost_sdus(1);
    }
  }

  // TS 38.322 v16.2.0 Sec. 5.4
  void discard_sdu(uint32_t pdcp_count) override
  {
    logger.log_warning("Ignoring invalid attempt to discard SDU in TM. pdcp_count={}", pdcp_count);
    metrics.metrics_add_discard_failure(1);
  }

  // Interfaces for lower layers
  // TS 38.322 v16.2.0 Sec. 5.2.1.1
  byte_buffer_slice_chain pull_pdu(uint32_t nof_bytes) override
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
      metrics.metrics_add_small_alloc(1);
      return {};
    }

    rlc_sdu sdu = {};
    if (not sdu_queue.read(sdu)) {
      logger.log_warning("Could not read SDU, but queue should not be empty. Provided space ({} B)", nof_bytes);
      return {};
    }

    size_t sdu_size = sdu.buf.length();
    srsgnb_sanity_check(
        sdu_size == front_size, "Tx PDU size different than front size ({} != {})", sdu_size, front_size);

    // In TM there is no header, just pass the plain SDU
    byte_buffer_slice_chain pdu = {};
    pdu.push_back(std::move(sdu.buf));
    logger.log_info("Tx PDU ({} B). Provided space ({} B)", sdu_size, nof_bytes);
    metrics.metrics_add_pdus(1, pdu.length());
    handle_buffer_state_update();

    return pdu;
  }

  uint32_t get_buffer_state() override { return sdu_queue.size_bytes(); }

  void handle_buffer_state_update()
  {
    unsigned bytes = get_buffer_state();
    logger.log_debug("Sending buffer state update to lower layer: {} B", bytes);
    lower_dn.on_buffer_state_update(bytes);
  }
};

} // namespace srsgnb
