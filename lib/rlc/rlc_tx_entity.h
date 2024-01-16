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
  rlc_tx_entity(uint32_t                             du_index,
                du_ue_index_t                        ue_index,
                rb_id_t                              rb_id,
                rlc_tx_upper_layer_data_notifier&    upper_dn_,
                rlc_tx_upper_layer_control_notifier& upper_cn_,
                rlc_tx_lower_layer_notifier&         lower_dn_,
                bool                                 metrics_enabled,
                rlc_pcap&                            pcap_) :
    logger("RLC", {du_index, ue_index, rb_id, "DL"}),
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
  rlc_tx_metrics get_metrics() { return metrics.get_metrics(); }
  void           reset_metrics() { return metrics.reset_metrics(); }
};

} // namespace srsran
