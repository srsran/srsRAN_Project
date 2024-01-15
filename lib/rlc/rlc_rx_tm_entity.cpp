/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "rlc_rx_tm_entity.h"

using namespace srsran;

rlc_rx_tm_entity::rlc_rx_tm_entity(uint32_t                          du_index,
                                   du_ue_index_t                     ue_index,
                                   rb_id_t                           rb_id,
                                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                                   bool                              metrics_enabled,
                                   rlc_pcap&                         pcap_) :
  rlc_rx_entity(du_index, ue_index, rb_id, upper_dn_, metrics_enabled, pcap_),
  pcap_context(ue_index, rb_id, /* is_uplink */ true)
{
  metrics.metrics_set_mode(rlc_mode::tm);
  logger.log_info("RLC TM created.");
}

void rlc_rx_tm_entity::handle_pdu(byte_buffer_slice buf)
{
  metrics.metrics_add_pdus(1, buf.length());

  pcap.push_pdu(pcap_context, buf);

  logger.log_info(buf.begin(), buf.end(), "RX SDU. sdu_len={}", buf.length());
  metrics.metrics_add_sdus(1, buf.length());
  upper_dn.on_new_sdu(std::move(buf));
}
