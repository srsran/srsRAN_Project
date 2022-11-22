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
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/sdap/sdap.h"
#include <cstdio>

namespace srsgnb {
namespace srs_cu_up {

/// These adapters glue two network layers to establish a communication between them. With this pattern, each layer is
/// decoupled from the other.
/// This is necessary when the F1u interface is not required (no CU/DU split) and the RLC forwards packets directly to
/// the PDCP. In this case we would create a RLC-PDCP adapter following the same idea as done here, without having to
/// touch a single line of code of the RLC or PDCP layers. They are agnostic to the deployment scenario.

/// Adapter interface from PDCP to SDAP.
class sdap_packet_handler : public srsgnb::pdcp_rx_upper_data_notifier
{
  srsgnb::srs_cu_up::sdap_pdu_handler& sdap;

public:
  explicit sdap_packet_handler(srsgnb::srs_cu_up::sdap_pdu_handler& sdap) : sdap(sdap) {}

  void on_new_sdu(srsgnb::byte_buffer sdu) override
  {
    std::printf("[PDCP-SDAP-ADAPTER] Received a packet from PDCP layer, forwarding data packet of size = %u bytes to "
                "upper layer (SDAP)\n",
                (unsigned)sdu.length());
    sdap.handle_pdu(std::move(sdu));
  }
};

} // namespace srs_cu_up

} // namespace srsgnb