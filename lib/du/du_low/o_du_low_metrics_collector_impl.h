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

#include "srsran/du/du_low/o_du_low_metrics_collector.h"

namespace srsran {
namespace srs_du {

/// ORAN DU low metrics collector implementation.
class o_du_low_metrics_collector_impl : public o_du_low_metrics_collector
{
public:
  void collect_o_du_low_metrics(o_du_low_metrics& metrics) override;
};

} // namespace srs_du
} // namespace srsran
