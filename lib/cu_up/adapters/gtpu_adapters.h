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

#include "srsgnb/gtpu/gtpu_tunnel_rx.h"
#include "srsgnb/sdap/sdap.h"

namespace srsgnb {
namespace srs_cu_up {

/// Adapter between GTP-U and SDAP
class gtpu_sdap_adapter : public gtpu_tunnel_rx_lower_layer_notifier
{
public:
  gtpu_sdap_adapter()  = default;
  ~gtpu_sdap_adapter() = default;

  void connect_sdap(sdap_tx_sdu_handler& sdap_handler_) { sdap_handler = &sdap_handler_; }

  void on_new_sdu(byte_buffer sdu) override
  {
    srsgnb_assert(sdap_handler != nullptr, "SDAP handler must not be nullptr");
    sdap_handler->handle_sdu(std::move(sdu));
  }

private:
  sdap_tx_sdu_handler* sdap_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsgnb
