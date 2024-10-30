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

#include "srsran/ran/du_types.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/rb_id.h"
#include "srsran/rlc/rlc_rx_metrics.h"
#include "srsran/rlc/rlc_tx_metrics.h"

namespace srsran {

/// Container to hold TX/RX metrics
struct rlc_metrics {
  gnb_du_id_t    du_index;
  du_ue_index_t  ue_index;
  rb_id_t        rb_id;
  rlc_tx_metrics tx;
  rlc_rx_metrics rx;
  unsigned       counter;
  timer_duration metrics_period;
};

/// \brief Notifier interface used to report RLC metrics.
class rlc_metrics_notifier
{
public:
  virtual ~rlc_metrics_notifier() = default;

  /// \brief This method will be called periodically to report the latest RLC metrics statistics.
  virtual void report_metrics(const rlc_metrics& metrics) = 0;
};
} // namespace srsran
