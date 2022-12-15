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

#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/sdap/sdap.h"

namespace srsgnb {
namespace srs_cu_up {

/// Adapter between PDCP and SDAP
class pdcp_sdap_adapter : public pdcp_rx_upper_data_notifier
{
public:
  pdcp_sdap_adapter()  = default;
  ~pdcp_sdap_adapter() = default;

  void connect_sdap(sdap_pdu_handler& sdap_handler_) { sdap_handler = &sdap_handler_; }

  void on_new_sdu(byte_buffer sdu) override
  {
    srsgnb_assert(sdap_handler != nullptr, "SDAP handler must not be nullptr");
    sdap_handler->handle_pdu(std::move(sdu));
  }

private:
  sdap_pdu_handler* sdap_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsgnb
