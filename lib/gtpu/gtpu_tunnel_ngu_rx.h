/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "gtpu_tunnel_base_rx.h"
#include "srsran/ran/cu_types.h"

namespace srsran {

/// Class used for receiving GTP-U bearers.
class gtpu_tunnel_ngu_rx : public gtpu_tunnel_base_rx
{
public:
  gtpu_tunnel_ngu_rx(uint32_t                                 ue_index,
                     gtpu_config::gtpu_rx_config              cfg_,
                     gtpu_tunnel_ngu_rx_lower_layer_notifier& rx_lower_) :
    gtpu_tunnel_base_rx(ue_index, cfg_), lower_dn(rx_lower_)
  {
    // Validate configuration
    logger.log_info("GTPU NGU configured. {}", cfg);
  }
  ~gtpu_tunnel_ngu_rx() = default;

protected:
  // domain-specific PDU handler
  void handle_pdu(byte_buffer buf, const gtpu_header& hdr) final
  {
    qos_flow_id_t qos_flow = qos_flow_id_t::invalid;
    // TODO: pick corret qos_flow from hdr.ext_list

    logger.log_info(
        buf.begin(), buf.end(), "RX SDU. sdu_len={} teid={:#x} qos_flow={}", buf.length(), hdr.teid, qos_flow);
    lower_dn.on_new_sdu(std::move(buf), qos_flow);
  }

private:
  gtpu_tunnel_ngu_rx_lower_layer_notifier& lower_dn;
};
} // namespace srsran
