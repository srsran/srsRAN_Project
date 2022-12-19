/*
 *
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsgnb/f1u/cu_up/f1u_rx_pdu_handler.h"
#include "srsgnb/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsgnb/f1u/du/f1u_rx_pdu_handler.h"
#include "srsgnb/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsgnb/f1u/du/f1u_tx_pdu_notifier.h"

namespace srsgnb {
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
  void on_delivered_sdu(uint32_t count) override {}

private:
  srs_du::f1u_rx_pdu_handler* handler = nullptr;
};

class f1u_ul_local_adapter : public srs_du::f1u_tx_pdu_notifier
{
public:
  void attach_cu_handler(srs_cu_up::f1u_rx_pdu_handler& handler_) { handler = &handler_; }

  void on_new_pdu(nru_ul_message msg) override { handler->handle_pdu(std::move(msg)); };

private:
  srs_cu_up::f1u_rx_pdu_handler* handler = nullptr;
};

} // namespace srsgnb
