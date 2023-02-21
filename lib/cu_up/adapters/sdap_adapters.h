/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/sdap/sdap.h"

namespace srsran {
namespace srs_cu_up {

/// Adapter between SDAP and GTP-U
class sdap_gtpu_adapter : public sdap_rx_sdu_notifier
{
public:
  sdap_gtpu_adapter()  = default;
  ~sdap_gtpu_adapter() = default;

  void connect_gtpu(gtpu_tunnel_tx_lower_layer_interface& gtpu_handler_) { gtpu_handler = &gtpu_handler_; }

  void on_new_sdu(byte_buffer sdu) override
  {
    srsran_assert(gtpu_handler != nullptr, "GTPU handler must not be nullptr");
    gtpu_handler->handle_sdu(std::move(sdu));
  }

private:
  gtpu_tunnel_tx_lower_layer_interface* gtpu_handler = nullptr;
};

class sdap_pdcp_adapter : public sdap_tx_pdu_notifier
{
public:
  sdap_pdcp_adapter()  = default;
  ~sdap_pdcp_adapter() = default;

  void connect_pdcp(pdcp_tx_upper_data_interface& pdcp_handler_) { pdcp_handler = &pdcp_handler_; }
  void disconnect_pdcp() { pdcp_handler = nullptr; }

  void on_new_pdu(byte_buffer pdu) override
  {
    srsran_assert(pdcp_handler != nullptr, "PDCP handler must not be nullptr");
    pdcp_handler->handle_sdu(std::move(pdu));
  }

private:
  pdcp_tx_upper_data_interface* pdcp_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsran
