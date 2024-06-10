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

#include "srsran/f1u/cu_up/f1u_bearer_logger.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/f1u/du/f1u_bearer_logger.h"
#include "srsran/f1u/du/f1u_gateway.h"
#include "srsran/srslog/srslog.h"
#include <cstdint>
#include <unordered_map>

namespace srsran {

/// \brief Object used to represent a bearer at the CU F1-U gateway
/// On the co-located case this is done by connecting both entities directly.
///
/// It will keep a notifier to the DU NR-U RX and provide the methods to pass
/// an SDU to it.
class f1u_gateway_cu_bearer : public f1u_cu_up_gateway_bearer
{
public:
  f1u_gateway_cu_bearer(uint32_t                              ue_index,
                        drb_id_t                              drb_id,
                        const up_transport_layer_info&        ul_tnl_info_,
                        f1u_cu_up_gateway_bearer_rx_notifier& cu_rx_,
                        task_executor&                        ul_exec_,
                        srs_cu_up::f1u_bearer_disconnector&   disconnector_) :
    logger("CU-F1-U", {ue_index, drb_id, ul_tnl_info_}),
    disconnector(disconnector_),
    ul_tnl_info(ul_tnl_info_),
    cu_rx(cu_rx_),
    ul_exec(ul_exec_)
  {
  }

  ~f1u_gateway_cu_bearer() override { stop(); }

  void stop() override
  {
    if (not stopped) {
      disconnector.disconnect_cu_bearer(ul_tnl_info);
    }
    stopped = true;
  }

  void attach_du_notifier(srs_du::f1u_du_gateway_bearer_rx_notifier& notifier_,
                          const up_transport_layer_info&             dl_tnl_info_)
  {
    notifier = &notifier_;
    dl_tnl_info.emplace(dl_tnl_info_);
  }

  void detach_du_notifier(const up_transport_layer_info& dl_tnl_info_)
  {
    if (dl_tnl_info == dl_tnl_info_) {
      notifier = nullptr;
      dl_tnl_info.reset();
    } else {
      logger.log_info("Cannot dettach DU bearer, DL-FTEID does not match. F-TEID={}, requested F-TEID={}",
                      dl_tnl_info,
                      dl_tnl_info_);
    }
  }

  void on_new_pdu(nru_dl_message msg) override
  {
    if (notifier == nullptr) {
      logger.log_info("Cannot handle F1-U GW DL message. F1-U DU GW bearer does not exist.");
      return;
    }
    logger.log_debug("Passing PDU to DU bearer. {}", dl_tnl_info);
    notifier->on_new_pdu(std::move(msg));
  }

private:
  bool                                       stopped = false;
  srs_cu_up::f1u_bearer_logger               logger;
  srs_du::f1u_du_gateway_bearer_rx_notifier* notifier = nullptr;
  srs_cu_up::f1u_bearer_disconnector&        disconnector;
  up_transport_layer_info                    ul_tnl_info;

public:
  /// Holds notifier that will point to NR-U bearer on the UL path
  f1u_cu_up_gateway_bearer_rx_notifier& cu_rx;

  /// Holds the DL UP TNL info associated with the F1-U bearer.
  std::optional<up_transport_layer_info> dl_tnl_info;

  /// Holds the DL UP TNL info associated with the F1-U bearer.
  task_executor& ul_exec;
};

class f1u_gateway_du_bearer : public srs_du::f1u_du_gateway_bearer
{
public:
  f1u_gateway_du_bearer(uint32_t                                   ue_index,
                        drb_id_t                                   drb_id,
                        const up_transport_layer_info&             dl_tnl_info_,
                        srs_du::f1u_du_gateway_bearer_rx_notifier* f1u_rx_,
                        const up_transport_layer_info&             ul_up_tnl_info_,
                        srs_du::f1u_bearer_disconnector&           disconnector_) :
    f1u_rx(f1u_rx_),
    ul_up_tnl_info(ul_up_tnl_info_),
    dl_tnl_info(dl_tnl_info_),
    logger("DU-F1-U", {ue_index, drb_id, dl_tnl_info_}),
    disconnector(disconnector_)
  {
  }

  ~f1u_gateway_du_bearer() override { disconnector.remove_du_bearer(dl_tnl_info); }

  void stop() override
  {
    if (not stopped) {
      disconnector.remove_du_bearer(dl_tnl_info);
    }
    stopped = true;
  }

  void attach_cu_notifier(f1u_cu_up_gateway_bearer_rx_notifier& handler_)
  {
    std::unique_lock<std::mutex> lock(notifier_mutex);
    notifier = &handler_;
  }

  void detach_cu_notifier()
  {
    std::unique_lock<std::mutex> lock(notifier_mutex);
    notifier = nullptr;
  }

  void on_new_pdu(nru_ul_message msg) override
  {
    std::unique_lock<std::mutex> lock(notifier_mutex);
    if (notifier == nullptr) {
      logger.log_info("Cannot handle NR-U UL message. CU-UP bearer does not exist.");
      return;
    }
    notifier->on_new_pdu(std::move(msg));
  };
  bool                                       stopped = false;
  srs_du::f1u_du_gateway_bearer_rx_notifier* f1u_rx  = nullptr;
  up_transport_layer_info                    ul_up_tnl_info;
  up_transport_layer_info                    dl_tnl_info;

  srs_du::f1u_bearer_logger logger;

  f1u_cu_up_gateway_bearer_rx_notifier* notifier = nullptr;
  std::mutex                            notifier_mutex;
  srs_du::f1u_bearer_disconnector&      disconnector;
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

  std::unique_ptr<f1u_cu_up_gateway_bearer> create_cu_bearer(uint32_t                              ue_index,
                                                             drb_id_t                              drb_id,
                                                             const srs_cu_up::f1u_config&          config,
                                                             const up_transport_layer_info&        ul_up_tnl_info,
                                                             f1u_cu_up_gateway_bearer_rx_notifier& rx_notifier,
                                                             task_executor&                        ul_exec) override;

  void attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                      const up_transport_layer_info& dl_up_tnl_info) override;

  void disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info) override;

  std::unique_ptr<srs_du::f1u_du_gateway_bearer> create_du_bearer(uint32_t                       ue_index,
                                                                  drb_id_t                       drb_id,
                                                                  srs_du::f1u_config             config,
                                                                  const up_transport_layer_info& dl_up_tnl_info,
                                                                  const up_transport_layer_info& ul_up_tnl_info,
                                                                  srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                                                  timer_factory                              timers,
                                                                  task_executor& ue_executor) override;

  void remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info) override;

  expected<std::string> get_du_bind_address(gnb_du_id_t gnb_du_id) override
  {
    return fmt::format("127.0.0.{}", 1 + static_cast<uint32_t>(gnb_du_id));
  }

private:
  srslog::basic_logger& logger_cu;
  srslog::basic_logger& logger_du;
  // Key is the UL UP TNL Info (CU-CP address and UL TEID reserved by CU-CP)
  std::unordered_map<up_transport_layer_info, f1u_gateway_cu_bearer*> cu_map;
  // Key is the DL UP TNL Info (DU address and DL TEID reserved by DU)
  std::unordered_map<up_transport_layer_info, f1u_gateway_du_bearer*> du_map;

  std::mutex map_mutex; // shared mutex for access to cu_map and du_map
};

} // namespace srsran
