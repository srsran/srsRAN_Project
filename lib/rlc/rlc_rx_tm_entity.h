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

#include "rlc_rx_entity.h"

namespace srsgnb {

class rlc_rx_tm_entity : public rlc_rx_entity
{
public:
  rlc_rx_tm_entity(du_ue_index_t du_index, lcid_t lcid, rlc_rx_upper_layer_data_notifier& upper_dn) :
    rlc_rx_entity(du_index, lcid, upper_dn)
  {
    metrics.metrics_set_mode(rlc_mode::tm);
  }

  void handle_pdu(byte_buffer_slice buf) override
  {
    metrics.metrics_add_pdus(1, buf.length());
    logger.log_info("Rx PDU ({} B)", buf.length());
    metrics.metrics_add_sdus(1, buf.length());
    upper_dn.on_new_sdu(std::move(buf));
  }
};

} // namespace srsgnb
