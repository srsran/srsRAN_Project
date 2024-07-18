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

#include "srsran/f1ap/du/f1c_bearer.h"
#include "srsran/f1ap/du/f1c_rx_sdu_notifier.h"
#include "srsran/f1u/du/f1u_gateway.h"
#include "srsran/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsran/mac/mac_sdu_handler.h"
#include "srsran/mac/mac_ue_control_information_handler.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"

namespace srsran {
namespace srs_du {

// F1AP

/// \brief Adapter to connect F1AP Rx SDU notifier to RLC Tx SDU handler.
class f1c_rx_sdu_rlc_adapter final : public f1c_rx_sdu_notifier
{
public:
  void connect(rlc_tx_upper_layer_data_interface& rlc_tx_) { rlc_tx = &rlc_tx_; }

  void disconnect();

  void on_new_sdu(byte_buffer pdu) override
  {
    srsran_assert(rlc_tx != nullptr, "RLC Tx PDU notifier is disconnected");

    rlc_tx->handle_sdu(std::move(pdu), /* is_retx = */ false);
  }

private:
  rlc_tx_upper_layer_data_interface* rlc_tx = nullptr;
};

// F1-U

/// \brief Adapter to connect F1-U Rx SDU notifier to RLC Tx SDU handler.
class f1u_rx_rlc_sdu_adapter final : public f1u_rx_sdu_notifier
{
public:
  void connect(rlc_tx_upper_layer_data_interface& rlc_tx_) { rlc_tx = &rlc_tx_; }

  /// \brief Stop forwarding SDUs to the RLC layer.
  void disconnect();

  void on_new_sdu(byte_buffer sdu, bool is_retx) override
  {
    srsran_assert(rlc_tx != nullptr, "RLC Tx SDU notifier is disconnected");
    rlc_tx->handle_sdu(std::move(sdu), is_retx);
  }

  void on_discard_sdu(uint32_t pdcp_sn) override { rlc_tx->discard_sdu(pdcp_sn); }

private:
  rlc_tx_upper_layer_data_interface* rlc_tx = nullptr;
};

// F1-U Gateway
class f1u_gateway_nru_rx_adapter final : public f1u_du_gateway_bearer_rx_notifier
{
public:
  void connect(srs_du::f1u_rx_pdu_handler& nru_rx_) { nru_rx = &nru_rx_; }

  /// \brief Stop forwarding SDUs to the RLC layer.
  void disconnect();

  void on_new_pdu(nru_dl_message msg) override
  {
    srsran_assert(nru_rx != nullptr, "NR-U RX PDU notifier is disconnected");
    nru_rx->handle_pdu(std::move(msg));
  }

private:
  srs_du::f1u_rx_pdu_handler* nru_rx = nullptr;
};

// RLC
class rlc_rx_rrc_sdu_adapter : public rlc_rx_upper_layer_data_notifier
{
public:
  void connect(f1c_bearer& bearer_) { f1bearer = &bearer_; }

  void disconnect();

  void on_new_sdu(byte_buffer_chain pdu) override
  {
    srsran_assert(f1bearer != nullptr, "RLC Rx Bearer notifier is disconnected");
    f1bearer->handle_sdu(std::move(pdu));
  }

private:
  f1c_bearer* f1bearer = nullptr;
};

class rlc_f1u_tx_sdu_adapter : public rlc_rx_upper_layer_data_notifier
{
public:
  void connect(f1u_tx_sdu_handler& bearer_) { f1bearer = &bearer_; }

  void disconnect();

  void on_new_sdu(byte_buffer_chain sdu) override
  {
    srsran_assert(f1bearer != nullptr, "RLC Rx bearer notifier is disconnected");
    f1bearer->handle_sdu(std::move(sdu));
  }

private:
  f1u_tx_sdu_handler* f1bearer = nullptr;
};

class rlc_f1c_tx_data_notifier : public rlc_tx_upper_layer_data_notifier
{
public:
  void connect(f1c_bearer& bearer_) { bearer.store(&bearer_, std::memory_order_relaxed); }

  void disconnect();

  void on_transmitted_sdu(uint32_t max_deliv_pdcp_sn) override
  {
    f1c_bearer* b = bearer.load(std::memory_order_relaxed);
    srsran_assert(b != nullptr, "RLC to F1-C TX data notifier is disconnected");
    b->handle_transmit_notification(max_deliv_pdcp_sn);
  }

  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override
  {
    f1c_bearer* b = bearer.load(std::memory_order_relaxed);
    srsran_assert(b != nullptr, "RLC to F1-C TX data notifier is disconnected");
    b->handle_delivery_notification(max_deliv_pdcp_sn);
  }

  void on_retransmitted_sdu(uint32_t max_retx_pdcp_sn) override
  {
    srsran_assertion_failure("Unexpected call of on_retransmitted_sdu on SRB. max_retx_pdcp_sn={}", max_retx_pdcp_sn);
  }

  void on_delivered_retransmitted_sdu(uint32_t max_deliv_retx_pdcp_sn) override
  {
    srsran_assertion_failure("Unexpected call of on_delivered_retransmitted_sdu on SRB. max_deliv_retx_pdcp_sn={}",
                             max_deliv_retx_pdcp_sn);
  }

private:
  /// An atomic pointer to the handler. This pointer may be changed by \c disconnect from UE thread while the cell
  /// thread still uses this to notify F1 of transmitted/delivered PDCP SNs.
  /// The lifetime of the F1 bearer exceeds the \c disconnect and is synchronized with the scheduler.
  std::atomic<f1c_bearer*> bearer = nullptr;
};

class rlc_f1u_tx_data_notifier : public rlc_tx_upper_layer_data_notifier
{
public:
  void connect(f1u_tx_delivery_handler& handler_) { handler.store(&handler_, std::memory_order_relaxed); }

  void disconnect();

  void on_transmitted_sdu(uint32_t max_deliv_pdcp_sn) override
  {
    f1u_tx_delivery_handler* h = handler.load(std::memory_order_relaxed);
    srsran_assert(h != nullptr, "RLC to F1-U TX data notifier is disconnected");
    h->handle_transmit_notification(max_deliv_pdcp_sn);
  }

  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override
  {
    f1u_tx_delivery_handler* h = handler.load(std::memory_order_relaxed);
    srsran_assert(h != nullptr, "RLC to F1-U TX data notifier is disconnected");
    h->handle_delivery_notification(max_deliv_pdcp_sn);
  }

  void on_retransmitted_sdu(uint32_t max_retx_pdcp_sn) override
  {
    f1u_tx_delivery_handler* h = handler.load(std::memory_order_relaxed);
    srsran_assert(h != nullptr, "RLC to F1-U TX data notifier is disconnected");
    h->handle_retransmit_notification(max_retx_pdcp_sn);
  }

  void on_delivered_retransmitted_sdu(uint32_t max_deliv_retx_pdcp_sn) override
  {
    f1u_tx_delivery_handler* h = handler.load(std::memory_order_relaxed);
    srsran_assert(h != nullptr, "RLC to F1-U TX data notifier is disconnected");
    h->handle_delivery_retransmitted_notification(max_deliv_retx_pdcp_sn);
  }

private:
  /// An atomic pointer to the handler. This pointer may be changed by \c disconnect from UE thread while the cell
  /// thread still uses this to notify F1 of transmitted/delivered PDCP SNs.
  /// The lifetime of the F1 bearer exceeds the \c disconnect and is synchronized with the scheduler.
  std::atomic<f1u_tx_delivery_handler*> handler = nullptr;
};

class rlc_tx_control_notifier : public rlc_tx_upper_layer_control_notifier
{
public:
  void on_protocol_failure() override
  {
    // TODO
  }

  void on_max_retx() override
  {
    // TODO
  }
};

class rlc_tx_mac_buffer_state_updater : public rlc_tx_lower_layer_notifier
{
public:
  void connect(du_ue_index_t ue_index_, lcid_t lcid_, mac_ue_control_information_handler& mac_)
  {
    srsran_assert(ue_index_ != INVALID_DU_UE_INDEX, "Invalid UE index");
    srsran_assert(lcid_ != INVALID_LCID, "Invalid UE index");
    ue_index = ue_index_;
    lcid     = lcid_;
    mac      = &mac_;
  }

  void disconnect()
  {
    if (connected.exchange(false, std::memory_order_relaxed)) {
      // Push an empty buffer state update to MAC, so the scheduler doesn't keep allocating grants for this bearer.
      mac->handle_dl_buffer_state_update(mac_dl_buffer_state_indication_message{ue_index, lcid, 0});
    }
  }

  void on_buffer_state_update(unsigned bsr) override
  {
    srsran_assert(mac != nullptr, "RLC Tx Buffer State notifier is disconnected");
    mac_dl_buffer_state_indication_message bs{};
    bs.ue_index = ue_index;
    bs.lcid     = lcid;
    bs.bs       = bsr;
    if (SRSRAN_UNLIKELY(not connected.load(std::memory_order_relaxed))) {
      // Discard.
      bs.bs = 0;
    }
    mac->handle_dl_buffer_state_update(bs);
  }

private:
  du_ue_index_t                       ue_index = INVALID_DU_UE_INDEX;
  std::atomic<bool>                   connected{true};
  lcid_t                              lcid;
  mac_ue_control_information_handler* mac = nullptr;
};

// MAC

class mac_sdu_rx_adapter : public mac_sdu_rx_notifier
{
public:
  void connect(rlc_rx_lower_layer_interface& rlc_rx_) { rlc_handler = &rlc_rx_; }

  void disconnect();

  void on_new_sdu(byte_buffer_slice sdu) override
  {
    srsran_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
    rlc_handler->handle_pdu(std::move(sdu));
  }

private:
  rlc_rx_lower_layer_interface* rlc_handler = nullptr;
};

class mac_sdu_tx_adapter : public mac_sdu_tx_builder
{
public:
  void connect(rlc_tx_lower_layer_interface& rlc_tx) { rlc_handler = &rlc_tx; }

  void disconnect() { connected = false; }

  size_t on_new_tx_sdu(span<uint8_t> mac_sdu_buf) override
  {
    srsran_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
    return SRSRAN_LIKELY(connected.load(std::memory_order_relaxed)) ? rlc_handler->pull_pdu(mac_sdu_buf) : 0;
  }

  unsigned on_buffer_state_update() override
  {
    srsran_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
    return SRSRAN_LIKELY(connected.load(std::memory_order_relaxed)) ? rlc_handler->get_buffer_state() : 0;
  }

private:
  std::atomic<bool>             connected{true};
  rlc_tx_lower_layer_interface* rlc_handler = nullptr;
};

} // namespace srs_du
} // namespace srsran
