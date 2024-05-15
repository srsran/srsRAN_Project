/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsran/gateways/network_gateway.h"
#include "srsran/gtpu/gtpu_demux.h"

namespace srsran::srs_cu_up {

/// Adapter between Network Gateway (Data) and GTP-U demux
class network_gateway_data_gtpu_demux_adapter : public srsran::network_gateway_data_notifier_with_src_addr
{
public:
  network_gateway_data_gtpu_demux_adapter()           = default;
  ~network_gateway_data_gtpu_demux_adapter() override = default;

  void connect_gtpu_demux(gtpu_demux_rx_upper_layer_interface& gtpu_demux_) { gtpu_demux = &gtpu_demux_; }

  void disconnect() { gtpu_demux = nullptr; }

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    if (gtpu_demux != nullptr) {
      gtpu_demux->handle_pdu(std::move(pdu), src_addr);
    } else {
      srslog::fetch_basic_logger("GTPU", false).debug("Dropped DL GTP-U PDU. Demux adapter is disconnected.");
    }
  }

private:
  gtpu_demux_rx_upper_layer_interface* gtpu_demux = nullptr;
};

/// Adapter between NR-U and F1-U gateway adapter
class nru_tx_f1u_gateway_adapter : public f1u_tx_pdu_notifier
{
public:
  nru_tx_f1u_gateway_adapter()  = default;
  ~nru_tx_f1u_gateway_adapter() = default;

  void connect_f1u_gateway_bearer(f1u_cu_up_gateway_bearer_tx_interface& f1u_gw_bearer_)
  {
    f1u_gw_bearer = &f1u_gw_bearer_;
  }

  void on_new_pdu(nru_dl_message msg) override
  {
    srsran_assert(f1u_gw_bearer != nullptr, "GTP-U handler must not be nullptr");
    f1u_gw_bearer->on_new_sdu(std::move(msg));
  }

private:
  f1u_cu_up_gateway_bearer_tx_interface* f1u_gw_bearer = nullptr;
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

} // namespace srsran::srs_cu_up
