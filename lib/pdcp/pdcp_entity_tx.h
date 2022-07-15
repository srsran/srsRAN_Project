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
/// Base class used for transmitting PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_entity_tx : public pdcp_tx_sdu_handler, public pdcp_tx_lower_layer_data_handler
{
protected:
  pdcp_entity_tx(du_ue_index_t                         du_index,
                 lcid_t                                lcid,
                 pdcp_tx_lower_layer_data_notifier&    lower_dn,
                 pdcp_tx_upper_layer_control_notifier& upper_cn) :
    logger("PDCP", du_index, lcid), lower_dn(lower_dn), upper_cn(upper_cn)
  {
  }

  bearer_logger                         logger;
  pdcp_tx_lower_layer_data_notifier&    lower_dn;
  pdcp_tx_upper_layer_control_notifier& upper_cn;
};
} // namespace srsgnb
