/*
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
#include "srsgnb/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
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

  void on_new_sdu(byte_buffer_slice_chain sdu) override
  {
    srsgnb_assert(pdcp_rx_handler != nullptr, "PDCP handler must not be nullptr");
    pdcp_rx_handler->handle_pdu(std::move(sdu));
  }

  void on_delivered_sdu(uint32_t count) override { pdcp_tx_handler->handle_pdu_delivery_notification(count); }

private:
  pdcp_rx_lower_interface* pdcp_rx_handler = nullptr;
  pdcp_tx_lower_interface* pdcp_tx_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsgnb
