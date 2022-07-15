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
#include "srsgnb/pdcp/pdcp.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {
/// Base class used for receiving RLC bearers.
/// It provides interfaces for the RLC bearers, for the lower layers
class pdcp_entity_rx : public pdcp_rx_pdu_handler
{
protected:
  pdcp_entity_rx(du_ue_index_t du_index, lcid_t lcid, pdcp_rx_upper_layer_data_notifier& upper_dn) :
    logger("PDCP", du_index, lcid), upper_dn(upper_dn)
  {
  }

  bearer_logger                      logger;
  pdcp_rx_upper_layer_data_notifier& upper_dn;
};
} // namespace srsgnb
