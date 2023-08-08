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

#include "srsran/adt/span.h"
#include "srsran/e2/e2.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include <deque>

namespace srsran {

constexpr int MAX_UE_METRICS = 10;

/// \brief Class used to receive metrics reports from scheduler and sends them to the e2 interface.
class e2_du_metrics_connector : public scheduler_ue_metrics_notifier, public e2_du_metrics_interface
{
public:
  e2_du_metrics_connector();

  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;

  void get_metrics(scheduler_ue_metrics& ue_metrics) override;

private:
  std::deque<scheduler_ue_metrics> ue_metrics_queue;
};
} // namespace srsran
