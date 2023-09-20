/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/f1u/cu_up/f1u_bearer_factory.h"
#include "srsran/f1u/du/f1u_bearer_factory.h"
#include "srsran/ran/lcid.h"

using namespace srsran;

std::unique_ptr<srs_cu_up::f1u_bearer>
f1u_local_connector::create_cu_bearer(uint32_t                             ue_index,
                                      drb_id_t                             drb_id,
                                      const up_transport_layer_info&       ul_up_tnl_info,
                                      srs_cu_up::f1u_rx_delivery_notifier& rx_delivery_notifier,
                                      srs_cu_up::f1u_rx_sdu_notifier&      rx_sdu_notifier,
                                      timer_factory                        timers)
{
  logger_cu.info("Creating CU F1-U bearer with UL GTP Tunnel={}", ul_up_tnl_info);
  std::unique_lock<std::mutex> lock(map_mutex);
  srsran_assert(cu_map.find(ul_up_tnl_info) == cu_map.end(),
                "Cannot create CU F1-U bearer with already existing UL GTP Tunnel={}",
                ul_up_tnl_info);
  std::unique_ptr<f1u_dl_local_adapter> cu_tx =
      std::make_unique<f1u_dl_local_adapter>(ue_index, drb_id, ul_up_tnl_info);
  std::unique_ptr<srs_cu_up::f1u_bearer> f1u_bearer = srs_cu_up::create_f1u_bearer(
      ue_index, drb_id, ul_up_tnl_info, *cu_tx, rx_delivery_notifier, rx_sdu_notifier, timers, *this);
  f1u_cu_bearer cu_bearer(std::move(cu_tx), f1u_bearer.get());
  cu_map.insert({ul_up_tnl_info, std::move(cu_bearer)});
  return f1u_bearer;
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
  f1u_du_bearer& du_tun = du_map.at(dl_up_tnl_info);
  f1u_cu_bearer& cu_tun = cu_map.at(ul_up_tnl_info);
  cu_tun.dl_up_tnl_info = dl_up_tnl_info;
  cu_tun.cu_tx->attach_du_handler(du_tun.f1u_bearer->get_rx_pdu_handler(), dl_up_tnl_info);
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

  // Disconnect UL path of DU first if we have a dl_teid for lookup
  if (bearer_it->second.dl_up_tnl_info.has_value()) {
    auto du_bearer_it = du_map.find(bearer_it->second.dl_up_tnl_info.value());
    if (du_bearer_it != du_map.end()) {
      logger_cu.debug("Disconnecting DU F1-U bearer with DL GTP Tunnel={} from CU handler. UL GTP Tunnel={}",
                      *bearer_it->second.dl_up_tnl_info,
                      ul_up_tnl_info);
      du_bearer_it->second.du_tx->detach_cu_handler();
    } else {
      // Bearer could already been removed from DU.
      logger_cu.info("Could not find DL GTP Tunnel={} at DU to disconnect DU F1-U bearer from CU handler. UL GTP "
                     "Tunnel={}",
                     *bearer_it->second.dl_up_tnl_info,
                     ul_up_tnl_info);
    }
  } else {
    logger_cu.warning("No DL-TEID provided to disconnect DU F1-U bearer from CU handler. UL GTP Tunnel={}",
                      ul_up_tnl_info);
  }

  // Remove DL path
  logger_cu.debug("Removing CU F1-U bearer with UL GTP Tunnel={}.", ul_up_tnl_info);
  cu_map.erase(bearer_it);
}

srs_du::f1u_bearer* f1u_local_connector::create_du_bearer(uint32_t                       ue_index,
                                                          drb_id_t                       drb_id,
                                                          srs_du::f1u_config             config,
                                                          const up_transport_layer_info& dl_up_tnl_info,
                                                          const up_transport_layer_info& ul_up_tnl_info,
                                                          srs_du::f1u_rx_sdu_notifier&   du_rx,
                                                          timer_factory                  timers)
{
  std::unique_lock<std::mutex> lock(map_mutex);
  if (cu_map.find(ul_up_tnl_info) == cu_map.end()) {
    logger_du.warning("Could not find CU F1-U bearer, when creating DU F1-U bearer. DL GTP Tunnel={}, UL GTP Tunnel={}",
                      dl_up_tnl_info,
                      ul_up_tnl_info);
    return nullptr;
  }

  logger_du.debug("Creating DU F1-U bearer. DL GTP Tunnel={}, UL GTP Tunnel={}", dl_up_tnl_info, ul_up_tnl_info);
  std::unique_ptr<f1u_ul_local_adapter> du_tx =
      std::make_unique<f1u_ul_local_adapter>(ue_index, drb_id, dl_up_tnl_info);

  srs_du::f1u_bearer_creation_message f1u_msg = {};
  f1u_msg.ue_index                            = ue_index;
  f1u_msg.drb_id                              = drb_id;
  f1u_msg.config                              = config;
  f1u_msg.rx_sdu_notifier                     = &du_rx;
  f1u_msg.tx_pdu_notifier                     = du_tx.get();
  f1u_msg.timers                              = timers;

  std::unique_ptr<srs_du::f1u_bearer> f1u_bearer = srs_du::create_f1u_bearer(f1u_msg);
  srs_du::f1u_bearer*                 ptr        = f1u_bearer.get();
  auto&                               cu_tun     = cu_map.at(ul_up_tnl_info);

  du_tx->attach_cu_handler(cu_tun.f1u_bearer->get_rx_pdu_handler());

  f1u_du_bearer du_bearer(std::move(du_tx), std::move(f1u_bearer), ul_up_tnl_info);
  du_map.insert({dl_up_tnl_info, std::move(du_bearer)});
  return ptr;
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

  auto cu_bearer_it = cu_map.find(du_bearer_it->second.ul_up_tnl_info);
  if (cu_bearer_it != cu_map.end()) {
    logger_du.debug("Detaching CU handler do to removal at DU. UL GTP Tunnel={}", du_bearer_it->second.ul_up_tnl_info);
    cu_bearer_it->second.cu_tx->detach_du_handler(dl_up_tnl_info);
  }

  du_map.erase(du_bearer_it);
}
