/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_rx_am_entity.h"

using namespace srsgnb;

rlc_rx_am_entity::rlc_rx_am_entity(du_ue_index_t                     du_index,
                                   lcid_t                            lcid,
                                   const rlc_rx_am_config&           config,
                                   rlc_rx_upper_layer_data_notifier& upper_dn,
                                   timer_manager&                    timers) :
  rlc_rx_entity(du_index, lcid, upper_dn), cfg(config), mod(cardinality(to_number(cfg.sn_field_length)))
{
}

// Interfaces for lower layers
void rlc_rx_am_entity::handle_pdu(byte_buffer_slice buf)
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

rlc_am_status_pdu rlc_rx_am_entity::get_status_pdu()
{
  // TODO
  return rlc_am_status_pdu(cfg.sn_field_length);
}

uint32_t rlc_rx_am_entity::get_status_pdu_length()
{
  // TODO
  return 0;
}

bool rlc_rx_am_entity::status_report_required()
{
  // TODO
  return false;
}
