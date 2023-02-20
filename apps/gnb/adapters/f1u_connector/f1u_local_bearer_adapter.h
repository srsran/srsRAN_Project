/*
 *
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsran/f1u/cu_up/f1u_rx_pdu_handler.h"
#include "srsran/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsran/f1u/du/f1u_rx_pdu_handler.h"
#include "srsran/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsran/f1u/du/f1u_tx_pdu_notifier.h"

namespace srsran {
class f1u_dl_local_adapter : public srs_cu_up::f1u_tx_pdu_notifier
{
public:
  void attach_du_handler(srs_du::f1u_rx_pdu_handler& handler_) { handler = &handler_; }
  void on_new_pdu(nru_dl_message msg) override { handler->handle_pdu(std::move(msg)); };

private:
  srs_du::f1u_rx_pdu_handler* handler = nullptr;
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
  void attach_cu_handler(srs_cu_up::f1u_rx_pdu_handler& handler_) { handler = &handler_; }
  void detach_cu_handler() { handler = nullptr; }

  void on_new_pdu(nru_ul_message msg) override
  {
    if (handler == nullptr) {
      srslog::fetch_basic_logger("F1-U").warning("Cannot handle NR-U UL message: CU handler not attached.");
      return;
    }
    handler->handle_pdu(std::move(msg));
  };

private:
  srs_cu_up::f1u_rx_pdu_handler* handler = nullptr;
};

} // namespace srsran
