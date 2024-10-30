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

#include "srsran/pdcp/pdcp_entity.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/rb_id.h"

namespace srsran {

/// \brief Notifier interface used to report RLC metrics.
class pdcp_metrics_notifier
{
public:
  virtual ~pdcp_metrics_notifier() = default;

  /// \brief This method will be called periodically to report the latest PDCP metrics statistics.
  virtual void report_metrics(const pdcp_metrics_container& metrics) = 0;
};
} // namespace srsran
