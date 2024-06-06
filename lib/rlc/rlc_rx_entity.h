/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  rlc_rx_entity(gnb_du_id_t                       gnb_du_id,
                du_ue_index_t                     ue_index,
                rb_id_t                           rb_id,
                rlc_rx_upper_layer_data_notifier& upper_dn_,
                bool                              metrics_enable,
                rlc_pcap&                         pcap_) :
    logger("RLC", {gnb_du_id, ue_index, rb_id, "UL"}), upper_dn(upper_dn_), metrics(metrics_enable), pcap(pcap_)
  {
  }

  rlc_bearer_logger                 logger;
  rlc_rx_upper_layer_data_notifier& upper_dn;
  rlc_rx_metrics_container          metrics;
  rlc_pcap&                         pcap;

public:
  /// \brief Stops all internal timers.
  ///
  /// This function is inteded to be called upon removal of the bearer before destroying it.
  /// It stops all timers with handlers that may delegate tasks to another executor that could face a deleted object at
  /// a later execution time.
  /// Before this function is called, the adjacent layers should already be disconnected so that no timer is restarted.
  ///
  /// Note: This function shall only be called from ue_executor.
  virtual void stop() = 0;

  rlc_rx_metrics get_metrics() { return metrics.get_metrics(); }
  void           reset_metrics() { return metrics.reset_metrics(); }
};

} // namespace srsran
