/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rlc_bearer_logger.h"
#include "rlc_rx_metrics_container.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/rlc/rlc_rx.h"

namespace srsran {

/// Base class used for receiving RLC bearers.
/// It provides interfaces for the RLC bearers, for the lower layers
class rlc_rx_entity : public rlc_rx_lower_layer_interface, public rlc_rx_metrics
{
protected:
  rlc_rx_entity(uint32_t                          du_index,
                du_ue_index_t                     ue_index,
                rb_id_t                           rb_id,
                rlc_rx_upper_layer_data_notifier& upper_dn_,
                rlc_pcap&                         pcap_) :
    logger("RLC", {du_index, ue_index, rb_id, "UL"}), upper_dn(upper_dn_), pcap(pcap_)
  {
  }

  rlc_bearer_logger                 logger;
  rlc_rx_metrics_container          metrics;
  rlc_rx_upper_layer_data_notifier& upper_dn;
  rlc_pcap&                         pcap;

public:
  rlc_rx_metrics get_metrics() { return metrics.get_metrics(); }
  void           reset_metrics() { return metrics.reset_metrics(); }
};

} // namespace srsran
