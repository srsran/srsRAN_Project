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

#include "apps/services/metrics/metrics_config.h"
#include "o_du_low_unit_factory_config.h"
#include "srsran/du/du_low/o_du_low.h"

namespace srsran {

/// O-RAN DU low unit.
struct o_du_low_unit {
  std::unique_ptr<srs_du::o_du_low>         o_du_lo;
  std::vector<app_services::metrics_config> metrics;
};

/// O-RAN DU low unit factory.
class o_du_low_unit_factory
{
  o_du_low_hal_dependencies hal_dependencies;

public:
  o_du_low_unit_factory(const std::optional<du_low_unit_hal_config>& hal_config, unsigned nof_cells);

  /// Creates an O-RAN DU low unit with the given parameters.
  o_du_low_unit create(const o_du_low_unit_config& params, const o_du_low_unit_dependencies& dependencies);
};

} // namespace srsran
