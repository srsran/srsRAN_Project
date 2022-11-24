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

#include "srsgnb/gtpu/gtpu_tunnel_tx.h"
#include "srsgnb/sdap/sdap.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_up {

/// Adapter between SDAP and GTP-U
class sdap_gtpu_adapter : public sdap_sdu_rx_notifier
{
public:
  sdap_gtpu_adapter()  = default;
  ~sdap_gtpu_adapter() = default;

  void connect_gtpu(gtpu_tunnel_tx_lower_layer_interface& gtpu_handler_) { gtpu_handler = &gtpu_handler_; }

  void on_new_sdu(byte_buffer sdu) override
  {
    srsgnb_assert(gtpu_handler != nullptr, "GTPU handler must not be nullptr");
    gtpu_handler->handle_sdu(std::move(sdu));
  }

private:
  gtpu_tunnel_tx_lower_layer_interface* gtpu_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsgnb
