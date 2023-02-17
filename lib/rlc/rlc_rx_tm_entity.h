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
  rlc_rx_tm_entity(du_ue_index_t du_index, rb_id_t rb_id, rlc_rx_upper_layer_data_notifier& upper_dn_) :
    rlc_rx_entity(du_index, rb_id, upper_dn_)
  {
    metrics.metrics_set_mode(rlc_mode::tm);
    logger.log_info("RLC TM created.");
  }

  void handle_pdu(byte_buffer_slice buf) override
  {
    metrics.metrics_add_pdus(1, buf.length());
    logger.log_info(buf.begin(), buf.end(), "RX SDU. sdu_len={}", buf.length());
    metrics.metrics_add_sdus(1, buf.length());
    upper_dn.on_new_sdu(std::move(buf));
  }
};

} // namespace srsgnb
