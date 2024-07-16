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

#include "rlc_tx_metrics_container.h"

namespace srsran {
class rlc_metrics_aggregator
{
public:
  // \breif push metrics from the lower RLC executor to the agregator
  // This will be called will transfer the execution by pushing a copy to
  // ue executor
  void push_low_metric(rlc_tx_metrics_atomic_lower m_lower);
  // \breif push metrics from the high RLC executors to the agregator
  // As these are called from the UE executor no execution transfer is required.
  void push_high_metrics(rlc_tx_metrics_atomic_higher m_higher);

  void handle_new_metrics_push();
};
} // namespace srsran
