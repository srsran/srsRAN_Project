/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "gtpu_tunnel_base_rx.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_tunnel_nru_rx.h"
#include "srsran/nru/nru_packing.h"

namespace srsran {

/// Class used for receiving GTP-U NR-U bearers, e.g. on F1-U interface.
class gtpu_tunnel_nru_rx_impl : public gtpu_tunnel_base_rx
{
public:
  gtpu_tunnel_nru_rx_impl(srs_cu_up::ue_index_t                             ue_index,
                          gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config cfg,
                          gtpu_tunnel_nru_rx_lower_layer_notifier&          rx_lower_) :
    gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix{ue_index, cfg.local_teid, "DL"}),
    packer(logger.get_basic_logger()),
    lower_dn(rx_lower_),
    config(cfg)
  {
    logger.log_info("GTPU NR-U Rx configured. local_teid={}", config.local_teid);
  }
  ~gtpu_tunnel_nru_rx_impl() override = default;

protected:
  // domain-specific PDU handler
  void handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr) final
  {
    gtpu_teid_t      teid                  = pdu.hdr.teid;
    nru_dl_user_data dl_user_data          = {};
    bool             have_nr_ran_container = false;
    for (auto ext_hdr : pdu.hdr.ext_list) {
      switch (ext_hdr.extension_header_type) {
        case gtpu_extension_header_type::nr_ran_container:
          if (!have_nr_ran_container) {
            have_nr_ran_container = packer.unpack(dl_user_data, ext_hdr.container);
            if (!have_nr_ran_container) {
              logger.log_error("Failed to unpack NR RAN container.");
            }
          } else {
            logger.log_warning("Ignoring multiple NR RAN container.");
          }
          break;
        default:
          logger.log_warning("Ignoring unexpected extension header at F1-U interface. type={}",
                             ext_hdr.extension_header_type);
      }
    }
    if (!have_nr_ran_container) {
      logger.log_warning("Incomplete PDU at F1-U interface: missing or invalid NR RAN container. pdu_len={} teid={}",
                         pdu.buf.length(),
                         teid);
      // As per TS 29.281 Sec. 5.2.2.6 the (...) NR RAN Container (...) may be transmitted in a G-PDU over the
      // X2-U, Xn-U and F1-U user plane interfaces (...).
      return;
    }

    logger.log_debug(pdu.buf.begin(), pdu.buf.end(), "RX PDU. sdu_len={}", pdu.buf.length());

    nru_dl_message dl_message = {};
    dl_message.t_pdu          = gtpu_extract_msg(std::move(pdu)); // header is invalidated after extraction;
    dl_message.dl_user_data   = std::move(dl_user_data);

    logger.log_info(dl_message.t_pdu.begin(), dl_message.t_pdu.end(), "RX SDU. sdu_len={}", dl_message.t_pdu.length());
    lower_dn.on_new_sdu(std::move(dl_message));
  }

private:
  nru_packing                              packer;
  gtpu_tunnel_nru_rx_lower_layer_notifier& lower_dn;

  /// Rx config
  gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config config;
};
} // namespace srsran
