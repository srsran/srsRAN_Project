/*
 *
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1u/du/split_connector/f1u_split_connector.h"
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
  logger_du.info("Creating CU gateway local bearer with UL GTP Tunnel={}", ul_up_tnl_info);
  std::unique_lock<std::mutex> lock(map_mutex);
  srsran_assert(du_map.find(ul_up_tnl_info) == du_map.end(),
                "Cannot create CU gateway local bearer with already existing UL GTP Tunnel={}",
                ul_up_tnl_info);

  std::unique_ptr<f1u_split_gateway_du_bearer> du_bearer =
      std::make_unique<f1u_split_gateway_du_bearer>(ue_index, drb_id, dl_up_tnl_info, du_rx, ul_up_tnl_info, *this);
  du_map.insert({dl_up_tnl_info, du_bearer.get()});
  return du_bearer;
}

void f1u_split_connector::remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info)
{
  std::unique_lock<std::mutex> lock(map_mutex);
  // TODO
  // Remove DL path
  // cu_map.erase(bearer_it);
  logger_du.debug("Removed CU F1-U bearer with UL GTP Tunnel={}.", dl_up_tnl_info);
}
