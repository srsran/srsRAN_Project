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

#include "pdcp_entity_tx_rx_base.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/ran/bearer_logger.h"

namespace srsgnb {
/// Base class used for receiving RLC bearers.
/// It provides interfaces for the RLC bearers, for the lower layers
class pdcp_entity_rx : public pdcp_entity_tx_rx_base, public pdcp_rx_lower_interface
{
public:
  pdcp_entity_rx(uint32_t                     ue_index,
                 lcid_t                       lcid,
                 pdcp_config::pdcp_rx_config  cfg_,
                 pdcp_rx_upper_data_notifier& upper_dn_) :
    pdcp_entity_tx_rx_base(lcid, cfg_.sn_size), logger("PDCP", ue_index, lcid), cfg(cfg_), upper_dn(upper_dn_)
  {
  }

  /*
   * Header helpers
   */
  bool read_data_pdu_header(const byte_buffer& buf, uint32_t& sn) const;

private:
  bearer_logger               logger;
  pdcp_config::pdcp_rx_config cfg;

  void handle_pdu(byte_buffer buf) final;

  /*
   * Notifiers
   */
  pdcp_rx_upper_data_notifier& upper_dn;
};

} // namespace srsgnb
