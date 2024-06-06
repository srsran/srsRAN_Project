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
#include "rlc_tx_metrics_container.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/rlc/rlc_tx.h"

namespace srsran {

/// Base class used for transmitting RLC bearers.
/// It provides interfaces for the RLC bearers, for both higher layers and lower layers
/// It also stores interfaces for the higher layers, both for the user-plane
/// and the control plane.
class rlc_tx_entity : public rlc_tx_upper_layer_data_interface,
                      public rlc_tx_lower_layer_interface,
                      public rlc_tx_metrics
{
protected:
  rlc_tx_entity(gnb_du_id_t                          gnb_du_id,
                du_ue_index_t                        ue_index,
                rb_id_t                              rb_id,
                rlc_tx_upper_layer_data_notifier&    upper_dn_,
                rlc_tx_upper_layer_control_notifier& upper_cn_,
                rlc_tx_lower_layer_notifier&         lower_dn_,
                bool                                 metrics_enabled,
                rlc_pcap&                            pcap_) :
    logger("RLC", {gnb_du_id, ue_index, rb_id, "DL"}),
    metrics(metrics_enabled),
    upper_dn(upper_dn_),
    upper_cn(upper_cn_),
    lower_dn(lower_dn_),
    pcap(pcap_)
  {
  }

  rlc_bearer_logger                    logger;
  rlc_tx_metrics_container             metrics;
  rlc_tx_upper_layer_data_notifier&    upper_dn;
  rlc_tx_upper_layer_control_notifier& upper_cn;
  rlc_tx_lower_layer_notifier&         lower_dn;
  rlc_pcap&                            pcap;

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

  rlc_tx_metrics get_metrics() { return metrics.get_metrics(); }
  void           reset_metrics() { return metrics.reset_metrics(); }
};

} // namespace srsran
