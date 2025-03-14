/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/du/du_low/o_du_low_metrics_collector.h"

namespace srsran {
namespace srs_du {

class du_low_metrics_collector;

/// O-DU low metrics collector implementation.
class o_du_low_metrics_collector_impl : public o_du_low_metrics_collector
{
  du_low_metrics_collector* collector = nullptr;

public:
  explicit o_du_low_metrics_collector_impl(du_low_metrics_collector* collector_) : collector(collector_) {}

  // See interface for documentation.
  void collect_metrics(o_du_low_metrics& metrics) override;

  /// Returns true if the metrics are enabled, otherwise false.
  bool enabled() const { return collector != nullptr; }
};

} // namespace srs_du
} // namespace srsran
