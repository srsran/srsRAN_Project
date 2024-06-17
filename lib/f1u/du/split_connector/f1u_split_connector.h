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

#pragma once

#include "srsran/f1u/du/f1u_bearer_logger.h"
#include "srsran/f1u/du/f1u_gateway.h"
#include "srsran/gtpu/gtpu_demux.h"
#include "srsran/gtpu/gtpu_tunnel_common_tx.h"
#include "srsran/gtpu/gtpu_tunnel_nru.h"
#include "srsran/gtpu/gtpu_tunnel_nru_factory.h"
#include "srsran/gtpu/gtpu_tunnel_nru_rx.h"
#include "srsran/gtpu/ngu_gateway.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/srslog/srslog.h"
#include <cstdint>
#include <unordered_map>

namespace srsran::srs_du {

class gtpu_tx_udp_gw_adapter : public gtpu_tunnel_common_tx_upper_layer_notifier
{
public:
  /// \brief Interface for the GTP-U to pass PDUs to the IO gateway
  /// \param sdu PDU to be transmitted.
  void on_new_pdu(byte_buffer buf, const sockaddr_storage& addr) override
  {
    if (handler != nullptr) {
      handler->handle_pdu(std::move(buf), addr);
    }
  }

  void connect(srs_cu_up::ngu_tnl_pdu_session& handler_) { handler = &handler_; }

  void disconnect() { handler = nullptr; }

  srs_cu_up::ngu_tnl_pdu_session* handler;
};

class gtpu_rx_f1u_adapter : public srsran::gtpu_tunnel_nru_rx_lower_layer_notifier
{
public:
  /// \brief Interface for the GTP-U to pass a SDU (i.e. NR-U DL message) into the lower layer.
  /// \param dl_message NR-U DL message with optional T-PDU.
  void on_new_sdu(nru_dl_message dl_message) override
  {
    if (handler != nullptr) {
      handler->on_new_pdu(std::move(dl_message));
    }
  }

  /// \brief Interface for the GTP-U to pass a SDU (i.e. NR-U UL message) into the lower layer.
  /// \param ul_message NR-U UL message with optional T-PDU.
  void on_new_sdu(nru_ul_message ul_message) override {}

  void connect(f1u_du_gateway_bearer_rx_notifier& handler_) { handler = &handler_; }

  void disconnect() { handler = nullptr; }

  f1u_du_gateway_bearer_rx_notifier* handler;
};

/// Adapter between Network Gateway (Data) and GTP-U demux
class network_gateway_data_gtpu_demux_adapter : public srsran::network_gateway_data_notifier_with_src_addr
{
public:
  network_gateway_data_gtpu_demux_adapter()           = default;
  ~network_gateway_data_gtpu_demux_adapter() override = default;

  void connect_gtpu_demux(gtpu_demux_rx_upper_layer_interface& gtpu_demux_) { gtpu_demux = &gtpu_demux_; }

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    srsran_assert(gtpu_demux != nullptr, "GTP-U handler must not be nullptr");
    gtpu_demux->handle_pdu(std::move(pdu), src_addr);
  }

private:
  gtpu_demux_rx_upper_layer_interface* gtpu_demux = nullptr;
};

/// \brief Object used to represent a bearer at the CU F1-U gateway
/// On the co-located case this is done by connecting both entities directly.
///
/// It will keep a notifier to the DU NR-U RX and provide the methods to pass
/// an SDU to it.
class f1u_split_gateway_du_bearer : public f1u_du_gateway_bearer
{
public:
  f1u_split_gateway_du_bearer(uint32_t                                   ue_index,
                              drb_id_t                                   drb_id,
                              const up_transport_layer_info&             dl_tnl_info_,
                              srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx_,
                              const up_transport_layer_info&             ul_up_tnl_info_,
                              srs_du::f1u_bearer_disconnector&           disconnector_,
                              dlt_pcap&                                  gtpu_pcap,
                              uint16_t                                   peer_port) :
    logger("DU-F1-U", {ue_index, drb_id, dl_tnl_info_}),
    disconnector(disconnector_),
    dl_tnl_info(dl_tnl_info_),
    ul_tnl_info(ul_up_tnl_info_),
    du_rx(du_rx_)
  {
    gtpu_to_f1u_adapter.connect(du_rx);

    gtpu_tunnel_nru_creation_message msg{};
    // msg.ue_index                            = 0; TODO
    msg.cfg.rx.local_teid = dl_tnl_info.gtp_teid;
    msg.cfg.tx.peer_teid  = ul_tnl_info.gtp_teid;
    msg.cfg.tx.peer_addr  = ul_tnl_info.tp_address.to_string();
    msg.cfg.tx.peer_port  = peer_port;
    msg.gtpu_pcap         = &gtpu_pcap;
    msg.tx_upper          = &gtpu_to_network_adapter;
    msg.rx_lower          = &gtpu_to_f1u_adapter;

    tunnel = srsran::create_gtpu_tunnel_nru(msg);
  }

  ~f1u_split_gateway_du_bearer() override { stop(); }

  void stop() override { disconnector.remove_du_bearer(dl_tnl_info); }

  void on_new_pdu(nru_ul_message msg) override
  {
    if (tunnel == nullptr) {
      logger.log_debug("DL GTPU tunnel not connected. Discarding SDU.");
      return;
    }
    tunnel->get_tx_lower_layer_interface()->handle_sdu(std::move(msg));
  }

  gtpu_tunnel_common_rx_upper_layer_interface* get_tunnel_rx_interface()
  {
    return tunnel->get_rx_upper_layer_interface();
  }

  /// Holds the RX executor associated with the F1-U bearer.
  // task_executor& dl_exec;

  gtpu_tx_udp_gw_adapter gtpu_to_network_adapter;
  gtpu_rx_f1u_adapter    gtpu_to_f1u_adapter;

private:
  f1u_bearer_logger                logger;
  f1u_bearer_disconnector&         disconnector;
  up_transport_layer_info          dl_tnl_info;
  up_transport_layer_info          ul_tnl_info;
  std::unique_ptr<gtpu_tunnel_nru> tunnel;

public:
  /// Holds notifier that will point to NR-U bearer on the DL path
  f1u_du_gateway_bearer_rx_notifier& du_rx;
};

/// \brief Object used to connect the DU and CU-UP F1-U bearers
/// On the co-located case this is done by connecting both entities directly.
///
/// Note that CU and DU bearer creation and removal can be performed from different threads and are therefore
/// protected by a common mutex.
class f1u_split_connector final : public f1u_du_udp_gateway
{
public:
  f1u_split_connector(srs_cu_up::ngu_gateway* udp_gw_,
                      gtpu_demux*             demux_,
                      dlt_pcap&               gtpu_pcap_,
                      uint16_t                peer_port_ = GTPU_PORT) :
    logger_du(srslog::fetch_basic_logger("DU-F1-U")),
    udp_gw(udp_gw_),
    demux(demux_),
    gtpu_pcap(gtpu_pcap_),
    peer_port(peer_port_)
  {
    udp_session = udp_gw->create(gw_data_gtpu_demux_adapter);
    gw_data_gtpu_demux_adapter.connect_gtpu_demux(*demux);
  }

  f1u_du_gateway* get_f1u_du_gateway() { return this; }

  std::optional<uint16_t> get_bind_port() override { return udp_session->get_bind_port(); }

  std::unique_ptr<f1u_du_gateway_bearer> create_du_bearer(uint32_t                                   ue_index,
                                                          drb_id_t                                   drb_id,
                                                          srs_du::f1u_config                         config,
                                                          const up_transport_layer_info&             dl_up_tnl_info,
                                                          const up_transport_layer_info&             ul_up_tnl_info,
                                                          srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                                          timer_factory                              timers,
                                                          task_executor& ue_executor) override;

  void remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info) override;

  expected<std::string> get_du_bind_address(gnb_du_id_t gnb_du_id) override;

private:
  srslog::basic_logger& logger_du;
  // Key is the UL UP TNL Info (CU-CP address and UL TEID reserved by CU-CP)
  std::unordered_map<up_transport_layer_info, f1u_split_gateway_du_bearer*> du_map;
  std::mutex map_mutex; // shared mutex for access to cu_map

  srs_cu_up::ngu_gateway*                         udp_gw;
  std::unique_ptr<srs_cu_up::ngu_tnl_pdu_session> udp_session;
  gtpu_demux*                                     demux;
  network_gateway_data_gtpu_demux_adapter         gw_data_gtpu_demux_adapter;
  dlt_pcap&                                       gtpu_pcap;

  uint16_t peer_port;
};

} // namespace srsran::srs_du
