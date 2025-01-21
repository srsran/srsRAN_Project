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
#include "du_low_hal_factory.h"
#include "srsran/adt/span.h"
#include "srsran/du/du_low/o_du_low.h"
#include "srsran/du/du_low/o_du_low_config.h"

namespace srsran {

namespace srs_du {
struct du_cell_config;
}

struct du_low_unit_config;
struct worker_manager;

/// O-RAN DU low unit.
struct o_du_low_unit {
  std::unique_ptr<srs_du::o_du_low>         o_du_lo;
  std::vector<app_services::metrics_config> metrics;
};

/// O-RAN DU low unit configuration.
struct o_du_low_unit_config {
  const du_low_unit_config&                   du_low_unit_cfg;
  std::vector<srs_du::cell_prach_ports_entry> prach_ports;
  span<const srs_du::du_cell_config>          du_cells;
  span<const unsigned>                        max_puschs_per_slot;
};

/// O-RAN DU low unit dependencies.
struct o_du_low_unit_dependencies {
  upper_phy_rg_gateway&                 rg_gateway;
  upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier;
  worker_manager&                       workers;
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
