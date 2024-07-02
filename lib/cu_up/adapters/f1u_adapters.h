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

#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsran/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_up {

/// Adapter between F1-U and PDCP
class f1u_pdcp_adapter final : public f1u_rx_sdu_notifier, public f1u_rx_delivery_notifier
{
public:
  void connect_pdcp(pdcp_rx_lower_interface& pdcp_rx_handler_, pdcp_tx_lower_interface& pdcp_tx_handler_)
  {
    pdcp_rx_handler = &pdcp_rx_handler_;
    pdcp_tx_handler = &pdcp_tx_handler_;
  }

  void on_new_sdu(byte_buffer_chain sdu) override
  {
    if (pdcp_rx_handler == nullptr) {
      srslog::fetch_basic_logger("F1-U").warning("Unconnected PDCP handler. Dropping F1-U SDU");
    } else {
      pdcp_rx_handler->handle_pdu(std::move(sdu));
    }
  }

  void on_transmit_notification(uint32_t highest_pdcp_sn) override
  {
    pdcp_tx_handler->handle_transmit_notification(highest_pdcp_sn);
  }

  void on_delivery_notification(uint32_t highest_pdcp_sn) override
  {
    pdcp_tx_handler->handle_delivery_notification(highest_pdcp_sn);
  }

  void on_retransmit_notification(uint32_t highest_pdcp_sn) override
  {
    pdcp_tx_handler->handle_retransmit_notification(highest_pdcp_sn);
  }

  void on_delivery_retransmitted_notification(uint32_t highest_pdcp_sn) override
  {
    pdcp_tx_handler->handle_delivery_retransmitted_notification(highest_pdcp_sn);
  }

private:
  pdcp_rx_lower_interface* pdcp_rx_handler = nullptr;
  pdcp_tx_lower_interface* pdcp_tx_handler = nullptr;
};

/// Adapter between NR-U and F1-U gateway adapter
class f1u_gateway_rx_nru_adapter : public f1u_cu_up_gateway_bearer_rx_notifier
{
public:
  f1u_gateway_rx_nru_adapter()           = default;
  ~f1u_gateway_rx_nru_adapter() override = default;

  void connect_nru_bearer(f1u_rx_pdu_handler& f1u_handler_) { f1u_handler = &f1u_handler_; }

  void on_new_pdu(nru_ul_message msg) override
  {
    srsran_assert(f1u_handler != nullptr, "GTP-U handler must not be nullptr");
    f1u_handler->handle_pdu(std::move(msg));
  }

private:
  f1u_rx_pdu_handler* f1u_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsran
