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

#include "ru_dummy_sector.h"
#include "srsran/adt/span.h"
#include "srsran/ru/ru_metrics_collector.h"

namespace srsran {

/// Dummy RU metrics collector implementation
class ru_dummy_metrics_collector : public ru_metrics_collector
{
  span<ru_dummy_sector> sectors;

public:
  explicit ru_dummy_metrics_collector(span<ru_dummy_sector> sectors_) : sectors(sectors_) {}

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;
};

} // namespace srsran
