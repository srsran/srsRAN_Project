/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "gtpu_tunnel_base_rx.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_tunnel_nru_rx.h"
#include "srsran/nru/nru_packing.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// Class used for receiving GTP-U NR-U bearers, e.g. on F1-U interface.
class gtpu_tunnel_nru_rx_impl : public gtpu_tunnel_base_rx
{
public:
  gtpu_tunnel_nru_rx_impl(srs_cu_up::ue_index_t                             ue_index,
                          gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config cfg,
                          gtpu_tunnel_nru_rx_lower_layer_notifier&          rx_lower_) :
    gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix{ue_index, cfg.local_teid, "RX"}),
    packer(logger.get_basic_logger()),
    lower_dn(rx_lower_),
    config(cfg)
  {
    logger.log_info("GTPU NR-U Rx configured. node={} local_teid={}", config.node, config.local_teid);
    srsran_assert(cfg.node != nru_node::invalid, "GTP-U RX node not correctly initialized");
  }
  ~gtpu_tunnel_nru_rx_impl() override = default;

protected:
  // domain-specific PDU handler
  void handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr) final
  {
    size_t pdu_len = pdu.buf.length();
    // gtpu_teid_t      teid                  = pdu.hdr.teid;
    bool             have_nr_ran_container = false;
    byte_buffer_view nr_ran_container;
    for (auto ext_hdr : pdu.hdr.ext_list) {
      switch (ext_hdr.extension_header_type) {
        case gtpu_extension_header_type::nr_ran_container:
          if (!have_nr_ran_container) {
            have_nr_ran_container = true;
            nr_ran_container      = ext_hdr.container;
          } else {
            logger.log_warning("Ignoring multiple NR RAN container. pdu_len={}", pdu_len);
          }
          break;
        default:
          logger.log_warning("Ignoring unexpected extension header at F1-U interface. type={} pdu_len={}",
                             ext_hdr.extension_header_type,
                             pdu_len);
      }
    }

    logger.log_debug(pdu.buf.begin(), pdu.buf.end(), "RX PDU. pdu_len={}", pdu_len);

    // DL message
    if (config.node == nru_node::du) {
      if (not have_nr_ran_container) {
        logger.log_warning("Dropping DL message without NRU DL user data. pdu_len={}", pdu_len);
        return;
      }
      nru_dl_user_data dud;
      if (not packer.unpack(dud, nr_ran_container)) {
        logger.log_warning("Dropping DL message. Cause: could not unpack NR RAN container. pdu_len={}", pdu_len);
        return;
      }
      nru_dl_message dl_message = {};
      dl_message.t_pdu          = gtpu_extract_msg(std::move(pdu)); // header is invalidated after extraction;
      dl_message.dl_user_data   = std::move(dud);

      logger.log_info(
          dl_message.t_pdu.begin(), dl_message.t_pdu.end(), "RX DL user data. t_pdu_len={}", dl_message.t_pdu.length());
      lower_dn.on_new_sdu(std::move(dl_message));
      return;
    }

    // UL message
    if (config.node == nru_node::cu_up) {
      nru_ul_message ul_message = {};

      // Get DDDS, if exists.
      std::optional<nru_dl_data_delivery_status> ddds_opt;
      if (have_nr_ran_container) {
        nru_dl_data_delivery_status ddds;
        if (not packer.unpack(ddds, nr_ran_container)) {
          logger.log_warning("Dropping UL message. Cause: could not unpack NR RAN container. pdu_len={}", pdu_len);
          return;
        }
        ddds_opt = ddds;
      }
      ul_message.data_delivery_status = ddds_opt;

      // Extract T-PDU
      expected<byte_buffer_chain> buf =
          byte_buffer_chain::create(gtpu_extract_msg(std::move(pdu))); // header is invalidated after extraction;
      if (not buf.has_value()) {
        logger.log_error("Dropped PDU: Failed to create byte_buffer_chain. pdu_len={}", pdu_len);
        return;
      }
      if (!buf.value().empty()) {
        ul_message.t_pdu = std::move(buf.value());
      }
      if (ul_message.t_pdu.has_value()) {
        logger.log_info(ul_message.t_pdu.value().begin(),
                        ul_message.t_pdu.value().end(),
                        "RX DL data delivery status. t_pdu_len={}",
                        ul_message.t_pdu.value().length());
      } else {
        logger.log_info("RX DL data delivery status");
      }
      lower_dn.on_new_sdu(std::move(ul_message));
      return;
    }

    // We should never come here
    logger.log_error("Unhandled NR-U PDU. pdu_len={}", pdu_len);
  }

private:
  nru_packing                              packer;
  gtpu_tunnel_nru_rx_lower_layer_notifier& lower_dn;

  /// Rx config
  gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config config;
};
} // namespace srsran
