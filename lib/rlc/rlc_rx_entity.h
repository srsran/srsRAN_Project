/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
                bool                              metrics_enable,
                rlc_pcap&                         pcap_) :
    logger("RLC", {du_index, ue_index, rb_id, "UL"}), upper_dn(upper_dn_), metrics(metrics_enable), pcap(pcap_)
  {
  }

  rlc_bearer_logger                 logger;
  rlc_rx_upper_layer_data_notifier& upper_dn;
  rlc_rx_metrics_container          metrics;
  rlc_pcap&                         pcap;

public:
  rlc_rx_metrics get_metrics() { return metrics.get_metrics(); }
  void           reset_metrics() { return metrics.reset_metrics(); }
};

} // namespace srsran
