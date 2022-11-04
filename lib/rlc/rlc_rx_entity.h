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

#include "rlc_metrics_impl.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"

namespace srsgnb {

/// Base class used for receiving RLC bearers.
/// It provides interfaces for the RLC bearers, for the lower layers
class rlc_rx_entity : public rlc_rx_lower_layer_interface, public rlc_rx_metrics
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
