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
#include "psup_packing.h"
#include "srsran/gtpu/psup_message.h"
#include "srsran/ran/cu_types.h"

namespace srsran {

/// Class used for receiving GTP-U bearers.
class gtpu_tunnel_ngu_rx : public gtpu_tunnel_base_rx
{
public:
  gtpu_tunnel_ngu_rx(uint32_t                                 ue_index,
                     gtpu_config::gtpu_rx_config              cfg_,
                     gtpu_tunnel_ngu_rx_lower_layer_notifier& rx_lower_) :
    gtpu_tunnel_base_rx(ue_index, cfg_), psup_packer(logger.get_basic_logger()), lower_dn(rx_lower_)
  {
    // Validate configuration
    logger.log_info("GTPU NGU configured. {}", cfg);
  }
  ~gtpu_tunnel_ngu_rx() = default;

protected:
  // domain-specific PDU handler
  void handle_pdu(byte_buffer buf, const gtpu_header& hdr) final
  {
    psup_dl_pdu_session_information pdu_session_info      = {};
    bool                            have_pdu_session_info = false;
    for (auto ext_hdr : hdr.ext_list) {
      switch (ext_hdr.extension_header_type) {
        case gtpu_extension_header_type::pdu_session_container:
          if (!have_pdu_session_info) {
            have_pdu_session_info = psup_packer.unpack(pdu_session_info, ext_hdr.container);
            if (!have_pdu_session_info) {
              logger.log_error("Failed to unpack PDU session container.");
            }
          } else {
            logger.log_warning("Ignoring multiple PDU session container.");
          }
        default:
          logger.log_warning("Ignoring unexpected extension header at NG-U interface. type={}",
                             ext_hdr.extension_header_type);
      }
    }
    if (!have_pdu_session_info) {
      logger.log_error("Cannot handle PDU at NG-U interface: missing or invalid PDU session container.");
      return;
    }

    logger.log_info(buf.begin(),
                    buf.end(),
                    "RX SDU. sdu_len={} teid={:#x} qos_flow={}",
                    buf.length(),
                    hdr.teid,
                    pdu_session_info.qos_flow_id);
    lower_dn.on_new_sdu(std::move(buf), pdu_session_info.qos_flow_id);
  }

private:
  psup_packing                             psup_packer;
  gtpu_tunnel_ngu_rx_lower_layer_notifier& lower_dn;
};
} // namespace srsran
