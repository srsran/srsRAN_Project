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

#include "srsgnb/f1u/cu_up/f1u_bearer.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/sdap/sdap.h"

namespace srsgnb {
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
    srsgnb_assert(sdap_handler != nullptr, "SDAP handler must not be nullptr");
    sdap_handler->handle_pdu(std::move(sdu));
  }

private:
  sdap_rx_pdu_handler* sdap_handler = nullptr;
};

/// Adapter between PDCP and F1-U
class pdcp_f1u_adapter : public pdcp_tx_lower_notifier
{
public:
  pdcp_f1u_adapter()           = default;
  ~pdcp_f1u_adapter() override = default;

  void connect_f1u(f1u_tx_sdu_handler& f1u_handler_) { f1u_handler = &f1u_handler_; }

  void on_new_pdu(byte_buffer pdu) override
  {
    srsgnb_assert(f1u_handler != nullptr, "F1-U handler must not be nullptr");
    f1u_handler->handle_sdu(std::move(pdu), 0);
  }

  void on_discard_pdu(uint32_t count) override
  {
    srsgnb_assert(f1u_handler != nullptr, "F1-U handler must not be nullptr");
    f1u_handler->discard_sdu(count);
  }

private:
  f1u_tx_sdu_handler* f1u_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsgnb
