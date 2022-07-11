/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_RLC_RLC_RX_ENTITY_H
#define SRSGNB_LIB_RLC_RLC_RX_ENTITY_H

#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_metrics.h"

namespace srsgnb {

/// Base class used for receiving RLC bearers.
/// It provides interfaces for the RLC bearers, for the lower layers
class rlc_rx_entity : public rlc_rx_pdu_handler, public rlc_bearer_rx_metrics
{
protected:
  rlc_rx_entity(du_ue_index_t du_index, lcid_t lcid, rlc_rx_upper_layer_data_notifier& upper_dn) :
    logger("RLC", du_index, lcid), upper_dn(upper_dn)
  {
  }

  bearer_logger                     logger;
  rlc_rx_upper_layer_data_notifier& upper_dn;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_RLC_RLC_RX_ENTITY_H
