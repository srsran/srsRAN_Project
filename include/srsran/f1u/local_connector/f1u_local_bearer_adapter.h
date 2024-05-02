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
#include "srsran/f1u/cu_up/f1u_rx_pdu_handler.h"
#include "srsran/f1u/du/f1u_bearer_logger.h"
#include "srsran/f1u/du/f1u_rx_pdu_handler.h"
#include "srsran/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsran/ran/up_transport_layer_info.h"
#include <mutex>

namespace srsran {
class f1u_dl_local_adapter : public f1u_cu_up_gateway_bearer_tx_interface
{
public:
  explicit f1u_dl_local_adapter(uint32_t ue_index, drb_id_t drb_id, const up_transport_layer_info& ul_tnl_info) :
    logger("CU-F1-U", {ue_index, drb_id, ul_tnl_info})
  {
  }

  void attach_du_handler(srs_du::f1u_rx_pdu_handler& handler_, const up_transport_layer_info& dl_tnl_info_)
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
      logger.log_info("Cannot handle NR-U DL message. DU bearer does not exist.");
      return;
    }
    logger.log_debug("Passing PDU to DU bearer. {}", dl_tnl_info);
    handler->handle_pdu(std::move(msg));
  };

private:
  srs_cu_up::f1u_bearer_logger logger;

  srs_du::f1u_rx_pdu_handler* handler = nullptr;

  optional<up_transport_layer_info> dl_tnl_info;
};

class f1u_ul_local_adapter : public srs_du::f1u_tx_pdu_notifier
{
public:
  explicit f1u_ul_local_adapter(uint32_t ue_index, drb_id_t drb_id, const up_transport_layer_info& dl_tnl_info) :
    logger("DU-F1-U", {ue_index, drb_id, dl_tnl_info})
  {
  }

  void attach_cu_handler(srs_cu_up::f1u_rx_pdu_handler& handler_)
  {
    std::unique_lock<std::mutex> lock(handler_mutex);
    handler = &handler_;
  }

  void detach_cu_handler()
  {
    std::unique_lock<std::mutex> lock(handler_mutex);
    handler = nullptr;
  }

  void on_new_pdu(nru_ul_message msg) override
  {
    std::unique_lock<std::mutex> lock(handler_mutex);
    if (handler == nullptr) {
      logger.log_info("Cannot handle NR-U UL message. CU-UP bearer does not exist.");
      return;
    }
    handler->handle_pdu(std::move(msg));
  };

private:
  srs_du::f1u_bearer_logger logger;

  srs_cu_up::f1u_rx_pdu_handler* handler = nullptr;
  std::mutex                     handler_mutex;
};

} // namespace srsran
