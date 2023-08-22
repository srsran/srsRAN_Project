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
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/psup/psup_packing.h"
#include "srsran/ran/cu_types.h"

namespace srsran {

/// Class used for receiving GTP-U bearers.
class gtpu_tunnel_ngu_rx : public gtpu_tunnel_base_rx
{
public:
  gtpu_tunnel_ngu_rx(srs_cu_up::ue_index_t                    ue_index,
                     gtpu_config::gtpu_rx_config              cfg,
                     gtpu_tunnel_ngu_rx_lower_layer_notifier& rx_lower_) :
    gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix{ue_index, cfg.local_teid, "DL"}),
    psup_packer(logger.get_basic_logger()),
    lower_dn(rx_lower_)
  {
  }
  ~gtpu_tunnel_ngu_rx() = default;

protected:
  // domain-specific PDU handler
  void handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr) final
  {
    gtpu_teid_t                     teid                  = pdu.hdr.teid;
    psup_dl_pdu_session_information pdu_session_info      = {};
    bool                            have_pdu_session_info = false;
    for (auto ext_hdr : pdu.hdr.ext_list) {
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
          break;
        default:
          logger.log_warning("Ignoring unexpected extension header at NG-U interface. type={}",
                             ext_hdr.extension_header_type);
      }
    }
    if (!have_pdu_session_info) {
      logger.log_warning(
          "Incomplete PDU at NG-U interface: missing or invalid PDU session container. pdu_len={} teid={}",
          pdu.buf.length(),
          teid);
      // As per TS 29.281 Sec. 5.2.2.7 the (...) PDU Session Container (...) shall be transmitted in a G-PDU over the N3
      // and N9 user plane interfaces (...).
      return;
    }

    byte_buffer sdu = gtpu_extract_t_pdu(std::move(pdu));

    logger.log_info(sdu.begin(),
                    sdu.end(),
                    "RX SDU. sdu_len={} teid={} qos_flow={}",
                    sdu.length(),
                    teid,
                    pdu_session_info.qos_flow_id);
    lower_dn.on_new_sdu(std::move(sdu), pdu_session_info.qos_flow_id);
  }

private:
  psup_packing                             psup_packer;
  gtpu_tunnel_ngu_rx_lower_layer_notifier& lower_dn;
};
} // namespace srsran
