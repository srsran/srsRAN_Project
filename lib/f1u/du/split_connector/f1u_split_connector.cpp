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
using namespace srs_du;

std::unique_ptr<f1u_du_gateway_bearer>
f1u_split_connector::create_du_bearer(uint32_t                                   ue_index,
                                      drb_id_t                                   drb_id,
                                      srs_du::f1u_config                         config,
                                      const up_transport_layer_info&             dl_up_tnl_info,
                                      const up_transport_layer_info&             ul_up_tnl_info,
                                      srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                      timer_factory                              timers,
                                      task_executor&                             ue_executor)
{
  logger_du.info(
      "Creating DU gateway local bearer with UL GTP Tunnel={} DL GTP Tunnel={}", ul_up_tnl_info, dl_up_tnl_info);
  std::unique_ptr<f1u_split_gateway_du_bearer> du_bearer = std::make_unique<f1u_split_gateway_du_bearer>(
      ue_index, drb_id, dl_up_tnl_info, du_rx, ul_up_tnl_info, *this, gtpu_pcap, peer_port);
  {
    std::unique_lock<std::mutex> lock(map_mutex);
    srsran_assert(du_map.find(ul_up_tnl_info) == du_map.end(),
                  "Cannot create DU gateway local bearer with already existing UL GTP Tunnel={}",
                  ul_up_tnl_info);

    du_bearer->gtpu_to_network_adapter.connect(*udp_session);
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

expected<std::string> f1u_split_connector::get_du_bind_address(gnb_du_id_t gnb_du_id)
{
  std::string ip_address;
  if (not udp_session->get_bind_address(ip_address)) {
    return make_unexpected(default_error_t{});
  }
  return ip_address;
}
