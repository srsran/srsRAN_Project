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

#include "f1u_split_connector.h"
#include "srsran/gtpu/gtpu_tunnel_nru_factory.h"
#include "srsran/ran/lcid.h"

using namespace srsran;
using namespace srs_cu_up;

class srs_cu_up::gtpu_tx_udp_gw_adapter : public gtpu_tunnel_common_tx_upper_layer_notifier
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

  srs_cu_up::ngu_tnl_pdu_session* handler = nullptr;
};

class srs_cu_up::gtpu_rx_f1u_adapter : public srsran::gtpu_tunnel_nru_rx_lower_layer_notifier
{
public:
  /// \brief Interface for the GTP-U to pass a SDU (i.e. NR-U DL message) into the lower layer.
  /// \param dl_message NR-U DL message with optional T-PDU.
  void on_new_sdu(nru_dl_message dl_message) override {}

  /// \brief Interface for the GTP-U to pass a SDU (i.e. NR-U UL message) into the lower layer.
  /// \param ul_message NR-U UL message with optional T-PDU.
  void on_new_sdu(nru_ul_message ul_message) override
  {
    if (handler != nullptr) {
      handler->on_new_pdu(std::move(ul_message));
    }
  }

  void connect(f1u_cu_up_gateway_bearer_rx_notifier& handler_) { handler = &handler_; }

  void disconnect() { handler = nullptr; }

  f1u_cu_up_gateway_bearer_rx_notifier* handler = nullptr;
};

/// Adapter between Network Gateway (Data) and GTP-U demux
class srs_cu_up::network_gateway_data_gtpu_demux_adapter : public srsran::network_gateway_data_notifier_with_src_addr
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

f1u_split_gateway_cu_bearer::f1u_split_gateway_cu_bearer(uint32_t                              ue_index_,
                                                         drb_id_t                              drb_id,
                                                         const up_transport_layer_info&        ul_tnl_info_,
                                                         f1u_cu_up_gateway_bearer_rx_notifier& cu_rx_,
                                                         ngu_tnl_pdu_session&                  udp_session,
                                                         task_executor&                        ul_exec_,
                                                         srs_cu_up::f1u_bearer_disconnector&   disconnector_) :
  ul_exec(ul_exec_),
  ue_index(ue_index_),
  logger("CU-F1-U", {ue_index_, drb_id, ul_tnl_info_}),
  disconnector(disconnector_),
  ul_tnl_info(ul_tnl_info_),
  cu_rx(cu_rx_)
{
  gtpu_to_network_adapter = std::make_unique<gtpu_tx_udp_gw_adapter>();
  gtpu_to_f1u_adapter     = std::make_unique<gtpu_rx_f1u_adapter>();
  gtpu_to_network_adapter->connect(udp_session);
  gtpu_to_f1u_adapter->connect(cu_rx);
}

f1u_split_gateway_cu_bearer::~f1u_split_gateway_cu_bearer()
{
  stop();
}

void f1u_split_gateway_cu_bearer::stop()
{
  if (not stopped) {
    disconnector.disconnect_cu_bearer(ul_tnl_info);
  }
  stopped = true;
}

f1u_split_connector::f1u_split_connector(ngu_gateway* udp_gw_,
                                         gtpu_demux*  demux_,
                                         dlt_pcap&    gtpu_pcap_,
                                         uint16_t     peer_port_) :
  logger_cu(srslog::fetch_basic_logger("CU-F1-U")),
  peer_port(peer_port_),
  udp_gw(udp_gw_),
  demux(demux_),
  gtpu_pcap(gtpu_pcap_)
{
  gw_data_gtpu_demux_adapter = std::make_unique<srs_cu_up::network_gateway_data_gtpu_demux_adapter>();
  udp_session                = udp_gw->create(*gw_data_gtpu_demux_adapter);
  gw_data_gtpu_demux_adapter->connect_gtpu_demux(*demux);
}

f1u_split_connector::~f1u_split_connector() = default;

std::unique_ptr<f1u_cu_up_gateway_bearer>
f1u_split_connector::create_cu_bearer(uint32_t                              ue_index,
                                      drb_id_t                              drb_id,
                                      const srs_cu_up::f1u_config&          config,
                                      const up_transport_layer_info&        ul_up_tnl_info,
                                      f1u_cu_up_gateway_bearer_rx_notifier& rx_notifier,
                                      task_executor&                        ul_exec)
{
  logger_cu.info("Creating CU gateway local bearer with UL GTP Tunnel={}", ul_up_tnl_info);
  auto cu_bearer = std::make_unique<f1u_split_gateway_cu_bearer>(
      ue_index, drb_id, ul_up_tnl_info, rx_notifier, *udp_session, ul_exec, *this);
  std::unique_lock<std::mutex> lock(map_mutex);
  srsran_assert(cu_map.find(ul_up_tnl_info) == cu_map.end(),
                "Cannot create CU gateway local bearer with already existing UL GTP Tunnel={}",
                ul_up_tnl_info);
  cu_map.insert({ul_up_tnl_info, cu_bearer.get()});

  // create GTP-U tunnel rx
  gtpu_tunnel_nru_rx_creation_message msg{};
  msg.ue_index          = int_to_ue_index(ue_index);
  msg.rx_cfg.local_teid = ul_up_tnl_info.gtp_teid;
  msg.rx_lower          = cu_bearer->gtpu_to_f1u_adapter.get();

  std::unique_ptr<gtpu_tunnel_common_rx_upper_layer_interface> tunnel_rx = srsran::create_gtpu_tunnel_nru_rx(msg);

  // attach it to the F1-U bearer
  cu_bearer->attach_tunnel_rx(std::move(tunnel_rx));

  // attach tunnel rx to DEMUX
  if (!demux->add_tunnel(ul_up_tnl_info.gtp_teid, cu_bearer->ul_exec, cu_bearer->get_tunnel_rx_interface())) {
    logger_cu.error("Could not attach UL-TEID to demux RX. TEID {} already exists", ul_up_tnl_info.gtp_teid);
    // continue here; but the new tunnel won't be able to rx any data because the TEID was already registered at demux
  }

  return cu_bearer;
}

void f1u_split_connector::attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                                         const up_transport_layer_info& dl_up_tnl_info)
{
  // Get CU bearer
  f1u_split_gateway_cu_bearer* cu_bearer = nullptr;
  {
    std::unique_lock<std::mutex> lock(map_mutex);
    if (cu_map.find(ul_up_tnl_info) == cu_map.end()) {
      logger_cu.warning("Could not find UL GTP Tunnel at CU-CP to connect. UL GTP Tunnel={}, DL GTP Tunnel={}",
                        ul_up_tnl_info,
                        dl_up_tnl_info);
      return;
    }
    cu_bearer = cu_map.at(ul_up_tnl_info);
  }

  // create GTP-U tunnel tx
  gtpu_tunnel_nru_tx_creation_message msg{};
  msg.ue_index         = int_to_ue_index(cu_bearer->ue_index);
  msg.tx_cfg.peer_teid = dl_up_tnl_info.gtp_teid;
  msg.tx_cfg.peer_addr = dl_up_tnl_info.tp_address.to_string();
  msg.tx_cfg.peer_port = peer_port;
  msg.gtpu_pcap        = &gtpu_pcap;
  msg.tx_upper         = cu_bearer->gtpu_to_network_adapter.get();

  std::unique_ptr<gtpu_tunnel_nru_tx_lower_layer_interface> tunnel_tx = srsran::create_gtpu_tunnel_nru_tx(msg);

  // attach it to the F1-U bearer
  cu_bearer->attach_tunnel_tx(std::move(tunnel_tx));

  logger_cu.info(
      "Connected CU bearer to DU bearer. UL GTP Tunnel={}, DL GTP Tunnel={}", ul_up_tnl_info, dl_up_tnl_info);
}

void f1u_split_connector::disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info)
{
  f1u_split_gateway_cu_bearer* cu_bearer = nullptr;
  {
    std::unique_lock<std::mutex> lock(map_mutex);
    if (cu_map.find(ul_up_tnl_info) == cu_map.end()) {
      logger_cu.warning("Could not disconnect CU F1-U bearer with unknown UL GTP Tunnel={}", ul_up_tnl_info);
      return;
    }
    cu_bearer = cu_map.at(ul_up_tnl_info);
  }

  // disconnect adapters
  cu_bearer->gtpu_to_network_adapter->disconnect();
  cu_bearer->gtpu_to_f1u_adapter->disconnect();

  // Remove UL from GTP-U demux
  demux->remove_tunnel(ul_up_tnl_info.gtp_teid);

  // Remove DL path
  {
    std::unique_lock<std::mutex> lock(map_mutex);
    cu_map.erase(ul_up_tnl_info);
  }
  logger_cu.debug("Removed CU F1-U bearer with UL GTP Tunnel={}.", ul_up_tnl_info);
}
