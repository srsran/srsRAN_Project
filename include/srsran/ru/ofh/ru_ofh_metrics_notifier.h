/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ru/ofh/ru_ofh_metrics.h"

namespace srsran {

/// Open Fronthaul Radio Unit metrics notifier.
class ru_ofh_metrics_notifier
{
public:
  virtual ~ru_ofh_metrics_notifier() = default;

  /// Notifies new Open Fronthaul Radio Unit metrics.
  virtual void on_new_metrics(const ru_ofh_metrics& metrics) = 0;
};

} // namespace srsran
