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

#pragma once

#include "srsran/f1u/cu_up/f1u_bearer_logger.h"
#include "srsran/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsran/f1u/cu_up/f1u_rx_pdu_handler.h"
#include "srsran/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsran/f1u/du/f1u_bearer_logger.h"
#include "srsran/f1u/du/f1u_rx_pdu_handler.h"
#include "srsran/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {
class f1u_dl_local_adapter : public srs_cu_up::f1u_tx_pdu_notifier
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
      logger.log_info("Cannot dettach DU handler: DL-TEID does not match.");
    }
  }
  void on_new_pdu(nru_dl_message msg) override
  {
    if (handler == nullptr) {
      logger.log_info("Cannot handle NR-U DL message: DU handler not attached.");
      return;
    }
    logger.log_debug("Passing PDU to DU handler. {}", dl_tnl_info);
    handler->handle_pdu(std::move(msg));
  };

private:
  srs_cu_up::f1u_bearer_logger      logger;
  srs_du::f1u_rx_pdu_handler*       handler     = nullptr;
  optional<up_transport_layer_info> dl_tnl_info = {};
};

class f1u_tx_delivery_local_adapter : public srs_cu_up::f1u_rx_delivery_notifier
{
public:
  void attach_du_handler(srs_du::f1u_rx_pdu_handler& handler_) { handler = &handler_; }
  void on_transmit_notification(uint32_t highest_pdcp_sn) override {}
  void on_delivery_notification(uint32_t highest_pdcp_sn) override {}

private:
  srs_du::f1u_rx_pdu_handler* handler = nullptr;
};

class f1u_ul_local_adapter : public srs_du::f1u_tx_pdu_notifier
{
public:
  explicit f1u_ul_local_adapter(uint32_t ue_index, drb_id_t drb_id, const up_transport_layer_info& dl_tnl_info) :
    logger("DU-F1-U", {ue_index, drb_id, dl_tnl_info})
  {
  }
  void attach_cu_handler(srs_cu_up::f1u_rx_pdu_handler& handler_) { handler = &handler_; }
  void detach_cu_handler() { handler = nullptr; }

  void on_new_pdu(nru_ul_message msg) override
  {
    if (handler == nullptr) {
      srslog::fetch_basic_logger("DU-F1-U").info("Cannot handle NR-U UL message: CU handler not attached.");
      return;
    }
    handler->handle_pdu(std::move(msg));
  };

private:
  srs_cu_up::f1u_rx_pdu_handler* handler = nullptr;
  srs_du::f1u_bearer_logger      logger;
};

} // namespace srsran
