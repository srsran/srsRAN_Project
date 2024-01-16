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

#include "srsran/f1u/cu_up/f1u_bearer.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/sdap/sdap.h"

namespace srsran {
namespace srs_cu_up {

/// Adapter between PDCP and SDAP
class pdcp_sdap_adapter : public pdcp_rx_upper_data_notifier
{
public:
  pdcp_sdap_adapter()  = default;
  ~pdcp_sdap_adapter() = default;

  void connect_sdap(sdap_rx_pdu_handler& sdap_handler_) { sdap_handler = &sdap_handler_; }

  void on_new_sdu(byte_buffer sdu) override
  {
    srsran_assert(sdap_handler != nullptr, "SDAP handler must not be nullptr");
    sdap_handler->handle_pdu(std::move(sdu));
  }

private:
  sdap_rx_pdu_handler* sdap_handler = nullptr;
};

/// Adapter between PDCP Rx and E1AP (to be forwarded to RRC in the DU)
class pdcp_rx_e1ap_adapter : public pdcp_rx_upper_control_notifier
{
public:
  pdcp_rx_e1ap_adapter()  = default;
  ~pdcp_rx_e1ap_adapter() = default;

  void connect_e1ap()
  {
    // TODO: Connect a E1AP handler
    srslog::fetch_basic_logger("PDCP").debug("No E1AP handler for PDCP Rx control events. All events will be ignored.");
  }

  void on_protocol_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_protocol_failure() from PDCP Rx: No E1AP handler.");
  }

  void on_integrity_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_integrity_failure() from PDCP Rx: No E1AP handler.");
  }

  void on_max_count_reached() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_max_count_reached() from PDCP Rx: No E1AP handler.");
  }
};

/// Adapter between PDCP and F1-U
class pdcp_f1u_adapter : public pdcp_tx_lower_notifier
{
public:
  pdcp_f1u_adapter() = default;
  ~pdcp_f1u_adapter() override {}

  void connect_f1u(f1u_tx_sdu_handler& f1u_handler_) { f1u_handler = &f1u_handler_; }
  void disconnect_f1u() { f1u_handler = nullptr; }

  void on_new_pdu(pdcp_tx_pdu pdu) override
  {
    if (f1u_handler == nullptr) {
      srslog::fetch_basic_logger("PDCP").warning("Unconnected F1-U handler. Dropping PDCP PDU");
    } else {
      f1u_handler->handle_sdu(std::move(pdu));
    }
  }

  void on_discard_pdu(uint32_t pdcp_sn) override
  {
    srsran_assert(f1u_handler != nullptr, "F1-U handler must not be nullptr");
    f1u_handler->discard_sdu(pdcp_sn);
  }

private:
  f1u_tx_sdu_handler* f1u_handler = nullptr;
};

/// Adapter between PDCP Tx and E1AP (to be forwarded to RRC in the DU)
class pdcp_tx_e1ap_adapter : public pdcp_tx_upper_control_notifier
{
public:
  pdcp_tx_e1ap_adapter()  = default;
  ~pdcp_tx_e1ap_adapter() = default;

  void connect_e1ap()
  {
    // TODO: connect a E1AP handler
    srslog::fetch_basic_logger("PDCP").debug("No E1AP handler for PDCP Tx control events. All events will be ignored.");
  }

  void on_protocol_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_protocol_failure() from PDCP Tx: No E1AP handler.");
  }

  void on_max_count_reached() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_max_count_reached() from PDCP Tx: No E1AP handler.");
  }
};

} // namespace srs_cu_up
} // namespace srsran
