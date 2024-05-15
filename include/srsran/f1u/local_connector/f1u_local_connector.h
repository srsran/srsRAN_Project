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
class f1u_gateway_cu_bearer : public f1u_cu_up_gateway_bearer_tx_interface
{
public:
  f1u_gateway_cu_bearer(uint32_t                              ue_index,
                        drb_id_t                              drb_id,
                        const up_transport_layer_info&        ul_tnl_info,
                        f1u_cu_up_gateway_bearer_rx_notifier& cu_rx_) :
    cu_rx(cu_rx_), logger("CU-F1-U", {ue_index, drb_id, ul_tnl_info})
  {
  }

  void attach_du_handler(srs_du::f1u_du_gateway_bearer_rx_notifier& handler_,
                         const up_transport_layer_info&             dl_tnl_info_)
  {
    handler = &handler_;
    dl_tnl_info.emplace(dl_tnl_info_);
  }

  void detach_du_handler(const up_transport_layer_info& dl_tnl_info_)
  {
    if (dl_tnl_info == dl_tnl_info_) {
      handler = nullptr;
      dl_tnl_info.reset();
    } else {
      logger.log_info("Cannot dettach DU bearer, DL-FTEID does not match. F-TEID={}, requested F-TEID={}",
                      dl_tnl_info,
                      dl_tnl_info_);
    }
  }

  void on_new_sdu(nru_dl_message msg) override
  {
    if (handler == nullptr) {
      logger.log_info("Cannot handle F1-U GW DL message. F1-U DU GW bearer does not exist.");
      return;
    }
    logger.log_debug("Passing PDU to DU bearer. {}", dl_tnl_info);
    handler->on_new_pdu(std::move(msg));
  }

  /// Holds notifier that will point to NR-U bearer on the UL path
  f1u_cu_up_gateway_bearer_rx_notifier& cu_rx;

  /// Holds the DL UP TNL info associated with the F1-U bearer.
  optional<up_transport_layer_info> dl_tnl_info;

private:
  srs_cu_up::f1u_bearer_logger               logger;
  srs_du::f1u_du_gateway_bearer_rx_notifier* handler = nullptr;
};

class f1u_gateway_du_bearer : public srs_du::f1u_du_gateway_bearer_tx_interface
{
public:
  f1u_gateway_du_bearer(uint32_t                                   ue_index,
                        drb_id_t                                   drb_id,
                        const up_transport_layer_info&             dl_tnl_info,
                        srs_du::f1u_du_gateway_bearer_rx_notifier* f1u_rx_,
                        const up_transport_layer_info&             ul_up_tnl_info_) :
    f1u_rx(f1u_rx_), ul_up_tnl_info(ul_up_tnl_info_), logger("DU-F1-U", {ue_index, drb_id, dl_tnl_info})
  {
  }

  void attach_cu_handler(f1u_cu_up_gateway_bearer_rx_notifier& handler_)
  {
    std::unique_lock<std::mutex> lock(handler_mutex);
    handler = &handler_;
  }

  void detach_cu_handler()
  {
    std::unique_lock<std::mutex> lock(handler_mutex);
    handler = nullptr;
  }

  void on_new_sdu(nru_ul_message msg) override
  {
    std::unique_lock<std::mutex> lock(handler_mutex);
    if (handler == nullptr) {
      logger.log_info("Cannot handle NR-U UL message. CU-UP bearer does not exist.");
      return;
    }
    handler->on_new_pdu(std::move(msg));
  };

  srs_du::f1u_du_gateway_bearer_rx_notifier* f1u_rx = nullptr;
  up_transport_layer_info                    ul_up_tnl_info;

  srs_du::f1u_bearer_logger logger;

  f1u_cu_up_gateway_bearer_rx_notifier* handler = nullptr;
  std::mutex                            handler_mutex;
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

  f1u_cu_up_gateway_bearer_tx_interface* create_cu_bearer(uint32_t                              ue_index,
                                                          drb_id_t                              drb_id,
                                                          const srs_cu_up::f1u_config&          config,
                                                          const up_transport_layer_info&        ul_up_tnl_info,
                                                          f1u_cu_up_gateway_bearer_rx_notifier& rx_notifier,
                                                          task_executor&                        ul_exec,
                                                          timer_factory                         ue_dl_timer_factory,
                                                          unique_timer& ue_inactivity_timer) override;

  void attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                      const up_transport_layer_info& dl_up_tnl_info) override;

  void disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info) override;
  srs_du::f1u_du_gateway_bearer_tx_interface* create_du_bearer(uint32_t                       ue_index,
                                                               drb_id_t                       drb_id,
                                                               srs_du::f1u_config             config,
                                                               const up_transport_layer_info& dl_up_tnl_info,
                                                               const up_transport_layer_info& ul_up_tnl_info,
                                                               srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                                               timer_factory                              timers,
                                                               task_executor& ue_executor) override;

  void remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info) override;

private:
  srslog::basic_logger& logger_cu;
  srslog::basic_logger& logger_du;
  // Key is the UL UP TNL Info (CU-CP address and UL TEID reserved by CU-CP)
  std::unordered_map<up_transport_layer_info, std::unique_ptr<f1u_gateway_cu_bearer>> cu_map;
  // Key is the DL UP TNL Info (DU address and DL TEID reserved by DU)
  std::unordered_map<up_transport_layer_info, std::unique_ptr<f1u_gateway_du_bearer>> du_map;

  std::mutex map_mutex; // shared mutex for access to cu_map and du_map
};

} // namespace srsran
