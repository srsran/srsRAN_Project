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
#include "srsran/rlc/rlc_metrics.h"

namespace srsran {
class rlc_metrics_aggregator
{
public:
  rlc_metrics_aggregator(rlc_metrics_notifier* rlc_metrics_notif_);

  // \breif push metrics from the lower RLC executor to the agregator
  // This will be called will transfer the execution by pushing a copy to
  // ue executor
  void push_tx_low_metrics(rlc_tx_metrics_lower m_lower_);
  // \breif push metrics from the high RLC executors to the agregator
  // As these are called from the UE executor no execution transfer is required.
  void push_tx_high_metrics(rlc_tx_metrics_higher m_higher_);

private:
  rlc_metrics_notifier* rlc_metrics_notif;

  uint32_t              du;
  du_ue_index_t         ue;
  rb_id_t               rb;
  rlc_tx_metrics_lower  m_lower;
  rlc_tx_metrics_higher m_higher;
};
} // namespace srsran
