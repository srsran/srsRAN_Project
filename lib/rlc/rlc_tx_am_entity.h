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

#include "rlc_am_interconnect.h"
#include "rlc_am_pdu.h"
#include "rlc_sdu_queue.h"
#include "rlc_tx_entity.h"
#include "srsgnb/support/srsgnb_assert.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {

class rlc_tx_am_entity : public rlc_tx_entity, public rlc_tx_am_status_handler
{
private:
  // Config storage
  const rlc_tx_am_config cfg;

  // Status interface
  rlc_rx_am_status_provider* status_provider = nullptr;

  // Tx SDU buffers
  rlc_sdu_queue sdu_queue;

  const uint32_t mod; // Tx counter modulus

public:
  rlc_tx_am_entity(du_ue_index_t                        du_index,
                   lcid_t                               lcid,
                   const rlc_tx_am_config&              config,
                   rlc_tx_upper_layer_control_notifier& upper_cn,
                   rlc_tx_buffer_state_update_notifier& buffer_state_notif,
                   timer_manager&                       timers);

  // Tx/Rx interconnect
  void set_status_provider(rlc_rx_am_status_provider* status_provider_) { status_provider = status_provider_; }

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
  byte_buffer_slice_chain pull_pdu(uint32_t nof_bytes) override
  {
    // TODO
    byte_buffer_slice_chain pdu = {};
    return pdu;
  }

  uint32_t get_buffer_state() override
  {
    // TODO
    return 0;
  }

  // Status PDU handler for TX entity
  void handle_status_pdu(rlc_am_status_pdu status_pdu) override
  {
    // TODO
  }
};

} // namespace srsgnb
