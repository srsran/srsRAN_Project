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
#include "rlc_rx_entity.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {

class rlc_rx_am_entity : public rlc_rx_entity, public rlc_rx_am_status_provider
{
private:
  // Config storage
  const rlc_rx_am_config cfg;

  // Status handler
  rlc_tx_am_status_handler* status_handler = nullptr;

  /// Rx counter modulus
  const uint32_t mod;

public:
  rlc_rx_am_entity(du_ue_index_t                     du_index,
                   lcid_t                            lcid,
                   const rlc_rx_am_config&           config,
                   rlc_rx_upper_layer_data_notifier& upper_dn,
                   timer_manager&                    timers);

  // Rx/Tx interconnect
  void set_status_handler(rlc_tx_am_status_handler* status_handler_) { status_handler = status_handler_; }

  // Interfaces for lower layers
  void handle_pdu(byte_buffer_slice buf) override
  {
    metrics_add_pdus(1, buf.length());
    logger.log_info("Rx PDU ({} B)", buf.length());
    metrics_add_sdus(1, buf.length());
    if (rlc_am_status_pdu::is_control_pdu(buf.view())) {
      rlc_am_status_pdu status_pdu(cfg.sn_field_length);
      if (status_pdu.unpack(buf.view())) {
        logger.log_debug(buf.begin(), buf.end(), "Successfully unpacked control PDU ({} B)", buf.length());
        status_handler->handle_status_pdu(std::move(status_pdu));
      } else {
        logger.log_error(buf.begin(), buf.end(), "Failed to unpack control PDU ({} B)", buf.length());
      }
    } else {
      // TODO: handle data PDU
    }
  }

  // Status provider for RX entity
  rlc_am_status_pdu get_status_pdu() override
  {
    // TODO
    return rlc_am_status_pdu(cfg.sn_field_length);
  }

  uint32_t get_status_pdu_length() override
  {
    // TODO
    return 0;
  }

  bool status_report_required() override
  {
    // TODO
    return false;
  }
};

} // namespace srsgnb
