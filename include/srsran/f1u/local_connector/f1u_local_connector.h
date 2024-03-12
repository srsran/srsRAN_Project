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

#include "f1u_local_bearer_adapter.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/f1u/du/f1u_gateway.h"
#include "srsran/srslog/srslog.h"
#include <cstdint>
#include <unordered_map>

namespace srsran {

struct f1u_cu_bearer {
  std::unique_ptr<f1u_dl_local_adapter> cu_tx      = nullptr;
  srs_cu_up::f1u_bearer*                f1u_bearer = nullptr;
  /// Holds the DL UP TNL info associated with the F1-U bearer.
  optional<up_transport_layer_info> dl_up_tnl_info;

  f1u_cu_bearer(std::unique_ptr<f1u_dl_local_adapter> cu_tx_, srs_cu_up::f1u_bearer* f1u_bearer_) :
    cu_tx(std::move(cu_tx_)), f1u_bearer(f1u_bearer_)
  {
  }
};

struct f1u_du_bearer {
  std::unique_ptr<f1u_ul_local_adapter> du_tx      = nullptr;
  std::unique_ptr<srs_du::f1u_bearer>   f1u_bearer = nullptr;
  up_transport_layer_info               ul_up_tnl_info;
  f1u_du_bearer(std::unique_ptr<f1u_ul_local_adapter> du_tx_,
                std::unique_ptr<srs_du::f1u_bearer>   f1u_bearer_,
                const up_transport_layer_info&        ul_up_tnl_info_) :
    du_tx(std::move(du_tx_)), f1u_bearer(std::move(f1u_bearer_)), ul_up_tnl_info(ul_up_tnl_info_)
  {
  }
};

/// \brief Object used to connect the DU and CU-UP F1-U bearers
/// On the co-located case this is done by connecting both entities directly.
///
/// Note that CU and DU bearer creation and removal can be performed from different threads and are therefore
/// protected by a common mutex.
class f1u_local_connector final : public srs_du::f1u_du_gateway, public f1u_cu_up_gateway
{
public:
  f1u_local_connector() :
    logger_cu(srslog::fetch_basic_logger("CU-F1-U")), logger_du(srslog::fetch_basic_logger("DU-F1-U"))
  {
  }

  srs_du::f1u_du_gateway* get_f1u_du_gateway() { return this; }
  f1u_cu_up_gateway*      get_f1u_cu_up_gateway() { return this; }

  std::unique_ptr<srs_cu_up::f1u_bearer> create_cu_bearer(uint32_t                             ue_index,
                                                          drb_id_t                             drb_id,
                                                          const up_transport_layer_info&       ul_up_tnl_info,
                                                          srs_cu_up::f1u_rx_delivery_notifier& rx_delivery_notifier,
                                                          srs_cu_up::f1u_rx_sdu_notifier&      rx_sdu_notifier,
                                                          task_executor&                       ul_exec,
                                                          timer_factory                        ue_dl_timer_factory,
                                                          unique_timer& ue_inactivity_timer) override;

  void attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                      const up_transport_layer_info& dl_up_tnl_info) override;

  void disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info) override;

  srs_du::f1u_bearer* create_du_bearer(uint32_t                       ue_index,
                                       drb_id_t                       drb_id,
                                       srs_du::f1u_config             config,
                                       const up_transport_layer_info& dl_up_tnl_info,
                                       const up_transport_layer_info& ul_up_tnl_info,
                                       srs_du::f1u_rx_sdu_notifier&   du_rx,
                                       timer_factory                  timers,
                                       task_executor&                 ue_executor) override;

  void remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info) override;

private:
  srslog::basic_logger& logger_cu;
  srslog::basic_logger& logger_du;
  // Key is the UL UP TNL Info (CU-CP address and UL TEID reserved by CU-CP)
  std::unordered_map<up_transport_layer_info, f1u_cu_bearer> cu_map;
  // Key is the DL UP TNL Info (DU address and DL TEID reserved by DU)
  std::unordered_map<up_transport_layer_info, f1u_du_bearer> du_map;

  std::mutex map_mutex; // shared mutex for access to cu_map and du_map
};

} // namespace srsran
