/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_RLC_RLC_TX_ENTITY_H
#define SRSGNB_LIB_RLC_RLC_TX_ENTITY_H

#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_metrics.h"

namespace srsgnb {

/// Base class used for transmitting RLC bearers.
/// It provides interfaces for the RLC bearers, for both higher layers and lower layers
/// It also stores interfaces for the higher layers, both for the user-plane
/// and the control plane.
class rlc_tx_entity : public rlc_tx_sdu_handler, public rlc_tx_pdu_transmitter, public rlc_bearer_tx_metrics
{
protected:
  rlc_tx_entity(du_ue_index_t du_index, lcid_t lcid, rlc_tx_upper_layer_control_notifier& upper_cn) :
    logger("RLC", du_index, lcid), upper_cn(upper_cn)
  {
  }

  bearer_logger                        logger;
  rlc_tx_upper_layer_control_notifier& upper_cn;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_RLC_RLC_TX_ENTITY_H
