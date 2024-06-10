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

#include "srsran/f1u/local_connector/f1u_local_connector.h"
#include "srsran/ran/lcid.h"

using namespace srsran;

std::unique_ptr<f1u_cu_up_gateway_bearer>
f1u_local_connector::create_cu_bearer(uint32_t                              ue_index,
                                      drb_id_t                              drb_id,
                                      const srs_cu_up::f1u_config&          config,
                                      const up_transport_layer_info&        ul_up_tnl_info,
                                      f1u_cu_up_gateway_bearer_rx_notifier& rx_notifier,
                                      task_executor&                        ul_exec)
{
  logger_cu.info("Creating CU gateway local bearer with UL GTP Tunnel={}", ul_up_tnl_info);
  std::unique_lock<std::mutex> lock(map_mutex);
  srsran_assert(cu_map.find(ul_up_tnl_info) == cu_map.end(),
                "Cannot create CU gateway local bearer with already existing UL GTP Tunnel={}",
                ul_up_tnl_info);
  std::unique_ptr<f1u_gateway_cu_bearer> cu_bearer =
      std::make_unique<f1u_gateway_cu_bearer>(ue_index, drb_id, ul_up_tnl_info, rx_notifier, ul_exec, *this);
  cu_map.insert({ul_up_tnl_info, cu_bearer.get()});
  return cu_bearer;
}

void f1u_local_connector::attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                                         const up_transport_layer_info& dl_up_tnl_info)
{
  std::unique_lock<std::mutex> lock(map_mutex);
  if (cu_map.find(ul_up_tnl_info) == cu_map.end()) {
    logger_cu.warning("Could not find UL GTP Tunnel at CU-CP to connect. UL GTP Tunnel={}, DL GTP Tunnel={}",
                      ul_up_tnl_info,
                      dl_up_tnl_info);
    return;
  }
  logger_cu.debug("Connecting CU F1-U bearer. UL GTP Tunnel={}, DL GTP Tunnel={}", ul_up_tnl_info, dl_up_tnl_info);

  if (du_map.find(dl_up_tnl_info) == du_map.end()) {
    logger_cu.warning("Could not find DL GTP Tunnel at DU to connect. UL GTP Tunnel={}, DL GTP Tunnel={}",
                      ul_up_tnl_info,
                      dl_up_tnl_info);
    return;
  }
  logger_cu.debug("Connecting DU F1-U bearer. UL GTP Tunnel={}, DL GTP Tunnel={}", ul_up_tnl_info, dl_up_tnl_info);
  f1u_gateway_du_bearer* du_tun = du_map.at(dl_up_tnl_info);
  f1u_gateway_cu_bearer* cu_tun = cu_map.at(ul_up_tnl_info);
  cu_tun->dl_tnl_info           = dl_up_tnl_info;
  cu_tun->attach_du_notifier(*du_tun->f1u_rx, dl_up_tnl_info);
}

void f1u_local_connector::disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info)
{
  std::unique_lock<std::mutex> lock(map_mutex);

  // Find bearer from ul_teid
  auto bearer_it = cu_map.find(ul_up_tnl_info);
  if (bearer_it == cu_map.end()) {
    logger_cu.warning("Could not find UL GTP Tunnel={} at CU to remove.", ul_up_tnl_info);
    return;
  }
  f1u_gateway_cu_bearer* cu_tun = bearer_it->second;

  // Disconnect UL path of DU first if we have a dl_teid for lookup
  if (cu_tun->dl_tnl_info.has_value()) {
    auto du_bearer_it = du_map.find(cu_tun->dl_tnl_info.value());
    if (du_bearer_it != du_map.end()) {
      logger_cu.debug(
          "Disconnecting DU F1-U bearer with DL GTP Tunnel={}. UL GTP Tunnel={}", cu_tun->dl_tnl_info, ul_up_tnl_info);
      du_bearer_it->second->detach_cu_notifier();
    } else {
      // Bearer could already been removed from DU.
      logger_cu.info("Could not find DL GTP Tunnel={} at DU to disconnect DU F1-U bearer. UL GTP "
                     "Tunnel={}",
                     cu_tun->dl_tnl_info,
                     ul_up_tnl_info);
    }
    cu_tun->detach_du_notifier(cu_tun->dl_tnl_info.value());
  } else {
    // The DU has already removed and disconnected its F1-U bearer before the CU-UP
    logger_cu.info("No associated DU F1-U bearer when disconnecting CU F1-U bearer. UL GTP Tunnel={}", ul_up_tnl_info);
  }

  // Remove DL path
  cu_map.erase(bearer_it);
  logger_cu.debug("Removed CU F1-U bearer with UL GTP Tunnel={}.", ul_up_tnl_info);
}

std::unique_ptr<srs_du::f1u_du_gateway_bearer>
f1u_local_connector::create_du_bearer(uint32_t                                   ue_index,
                                      drb_id_t                                   drb_id,
                                      srs_du::f1u_config                         config,
                                      const up_transport_layer_info&             dl_up_tnl_info,
                                      const up_transport_layer_info&             ul_up_tnl_info,
                                      srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                      timer_factory                              timers,
                                      task_executor&                             ue_executor)
{
  std::unique_lock<std::mutex> lock(map_mutex);
  if (cu_map.find(ul_up_tnl_info) == cu_map.end()) {
    logger_du.warning("Could not find CU F1-U bearer, when creating DU F1-U bearer. DL GTP Tunnel={}, UL GTP Tunnel={}",
                      dl_up_tnl_info,
                      ul_up_tnl_info);
    return nullptr;
  }
  srsran::f1u_gateway_cu_bearer* cu_tun = cu_map.at(ul_up_tnl_info);

  logger_du.debug("Creating DU F1-U bearer. DL GTP Tunnel={}, UL GTP Tunnel={}", dl_up_tnl_info, ul_up_tnl_info);
  std::unique_ptr<f1u_gateway_du_bearer> du_bearer =
      std::make_unique<f1u_gateway_du_bearer>(ue_index, drb_id, dl_up_tnl_info, &du_rx, ul_up_tnl_info, *this);

  du_bearer->attach_cu_notifier(cu_tun->cu_rx);

  du_map.insert({dl_up_tnl_info, du_bearer.get()});
  return du_bearer;
}

void f1u_local_connector::remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info)
{
  std::unique_lock<std::mutex> lock(map_mutex);

  auto du_bearer_it = du_map.find(dl_up_tnl_info);
  if (du_bearer_it == du_map.end()) {
    logger_du.warning("Could not find DL-TEID at DU to remove. DL GTP Tunnel={}", dl_up_tnl_info);
    return;
  }
  logger_du.debug("Removing DU F1-U bearer. DL GTP Tunnel={}", dl_up_tnl_info);

  auto cu_bearer_it = cu_map.find(du_bearer_it->second->ul_up_tnl_info);
  if (cu_bearer_it != cu_map.end()) {
    logger_du.debug("Detaching DU notifier to CU. UL GTP Tunnel={}", du_bearer_it->second->ul_up_tnl_info);
    cu_bearer_it->second->detach_du_notifier(dl_up_tnl_info);
  }

  du_bearer_it->second->detach_cu_notifier();
  du_map.erase(du_bearer_it);
}
