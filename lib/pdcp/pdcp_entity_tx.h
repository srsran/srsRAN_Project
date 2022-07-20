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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/ran/bearer_logger.h"

namespace srsgnb {
/// Base class used for transmitting PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_entity_tx : public pdcp_tx_upper_data_interface, public pdcp_tx_lower_interface
{
public:
  pdcp_entity_tx(pdcp_tx_lower_notifier& lower_dn, pdcp_tx_upper_control_notifier& upper_cn) :
    lower_dn(lower_dn), upper_cn(upper_cn)
  {
  }

private:
  pdcp_tx_lower_notifier&         lower_dn;
  pdcp_tx_upper_control_notifier& upper_cn;

  void handle_sdu(byte_buffer buf) final
  {
    bool max_hfn_reached = false; // FIXME actually check HFN
    if (max_hfn_reached) {
      upper_cn.on_max_hfn_reached();
    }
    lower_dn.on_new_pdu(std::move(buf));
  }
  void stop_discard_timer(uint32_t count) final {}
};
} // namespace srsgnb
