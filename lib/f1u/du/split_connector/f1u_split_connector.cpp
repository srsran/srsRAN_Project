/*
 *
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_split_connector.h"
#include "srsran/f1u/cu_up/f1u_session_manager_factory.h" // TODO FIXME!!!!
#include "srsran/gtpu/gtpu_tunnel_nru_factory.h"
#include "srsran/ran/rb_id.h"

using namespace srsran;
using namespace srs_du;

f1u_split_connector::f1u_split_connector(const gtpu_gateway_maps& udp_gw_maps,
                                         gtpu_demux*              demux_,
                                         dlt_pcap&                gtpu_pcap_,
                                         uint16_t                 peer_port_,
                                         std::string              f1u_ext_addr_) :
  logger_du(srslog::fetch_basic_logger("DU-F1-U")),
  demux(demux_),
  gtpu_pcap(gtpu_pcap_),
  peer_port(peer_port_),
  f1u_ext_addr(std::move(f1u_ext_addr_))
{
  srsran_assert(not udp_gw_maps.default_gws.empty(), "Cannot create CU F1-U split connector, no default GW present");
  gw_data_gtpu_demux_adapter = std::make_unique<network_gateway_data_gtpu_demux_adapter>();
  // Create default session(s)
  for (const std::unique_ptr<gtpu_gateway>& udp_gw : udp_gw_maps.default_gws) {
    f1u_sessions.default_gw_sessions.push_back(udp_gw->create(*gw_data_gtpu_demux_adapter));
  }
  std::map<five_qi_t, std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>> five_qi_gw_sessions;
  for (auto const& [five_qi, five_qi_gws] : udp_gw_maps.five_qi_gws) {
    for (auto const& five_qi_gw : five_qi_gws) {
      f1u_sessions.five_qi_gw_sessions[five_qi].push_back(five_qi_gw->create(*gw_data_gtpu_demux_adapter));
    }
  }
  gw_data_gtpu_demux_adapter->connect_gtpu_demux(*demux);

  // Create 5QI specific session(s)
  f1u_session_mngr = create_f1u_cu_up_session_manager(f1u_sessions);
}

std::unique_ptr<f1u_du_gateway_bearer>
f1u_split_connector::create_du_bearer(uint32_t                                   ue_index,
                                      drb_id_t                                   drb_id,
                                      five_qi_t                                  five_qi,
                                      srs_du::f1u_config                         config,
                                      const up_transport_layer_info&             dl_up_tnl_info,
                                      const up_transport_layer_info&             ul_up_tnl_info,
                                      srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                      timer_factory                              timers,
                                      task_executor&                             ue_executor)
{
  logger_du.info("Creating DU gateway local bearer with UL GTP Tunnel={} DL GTP Tunnel={} {}",
                 ul_up_tnl_info,
                 dl_up_tnl_info,
                 five_qi);
  auto&                                        udp_session = f1u_session_mngr->get_next_f1u_gateway(five_qi);
  std::unique_ptr<f1u_split_gateway_du_bearer> du_bearer   = std::make_unique<f1u_split_gateway_du_bearer>(
      ue_index, drb_id, dl_up_tnl_info, du_rx, ul_up_tnl_info, *this, gtpu_pcap, peer_port);
  {
    std::unique_lock<std::mutex> lock(map_mutex);
    srsran_assert(du_map.find(ul_up_tnl_info) == du_map.end(),
                  "Cannot create DU gateway local bearer with already existing UL GTP Tunnel={}",
                  ul_up_tnl_info);

    du_bearer->gtpu_to_network_adapter.connect(udp_session);
    du_map.insert({dl_up_tnl_info, du_bearer.get()});
  }

  // attach RX to DEMUX
  if (!demux->add_tunnel(dl_up_tnl_info.gtp_teid, ue_executor, du_bearer->get_tunnel_rx_interface())) {
    logger_du.error("Could not attach UL-TEID to demux RX. TEID {} already exists", ul_up_tnl_info.gtp_teid);
    return nullptr;
  }

  return du_bearer;
}

void f1u_split_connector::remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info)
{
  f1u_split_gateway_du_bearer* du_bearer = nullptr;
  {
    std::unique_lock<std::mutex> lock(map_mutex);
    if (du_map.find(dl_up_tnl_info) == du_map.end()) {
      logger_du.warning("Could not find UL GTP Tunnel at CU-CP to disconnect", dl_up_tnl_info);
      return;
    }
    du_bearer = du_map.at(dl_up_tnl_info);
  }
  // disconnect adapters
  du_bearer->gtpu_to_network_adapter.disconnect();
  du_bearer->gtpu_to_f1u_adapter.disconnect();

  // Remove UL from GTP-U demux
  demux->remove_tunnel(dl_up_tnl_info.gtp_teid);

  // Remove DL path
  {
    std::unique_lock<std::mutex> lock(map_mutex);
    du_map.erase(dl_up_tnl_info);
  }
  logger_du.debug("Removed CU F1-U bearer with UL GTP Tunnel={}.", dl_up_tnl_info);
}

// TODO FIXME!!!
expected<std::string> f1u_split_connector::get_du_bind_address(gnb_du_id_t gnb_du_id) const
{
  std::string ip_address;

  if (f1u_ext_addr == "auto" || f1u_ext_addr == "") {
    if (not f1u_sessions.default_gw_sessions[0]->get_bind_address(ip_address)) {
      return make_unexpected(default_error_t{});
    }
  } else {
    ip_address = f1u_ext_addr;
  }
  return ip_address;
}
