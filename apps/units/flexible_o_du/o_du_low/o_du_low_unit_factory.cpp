/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_low_unit_factory.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "du_low_config.h"
#include "du_low_config_translator.h"
#include "du_low_hal_factory.h"
#include "srsran/du/du_low/o_du_low_factory.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"

using namespace srsran;

o_du_low_unit_factory::o_du_low_unit_factory(const std::optional<du_low_unit_hal_config>& hal_config) :
  hal_dependencies(make_du_low_hal_dependencies(hal_config))
{
}

static srs_du::du_low_dependencies generate_du_low_dependencies(const o_du_low_unit_dependencies& dependencies,
                                                                const o_du_low_hal_dependencies&  hal_dependencies,
                                                                unsigned                          nof_cells)
{
  srs_du::du_low_dependencies out_deps;
  out_deps.logger = &srslog::fetch_basic_logger("DU");
  out_deps.cells.reserve(nof_cells);

  upper_phy_factory_dependencies& upper_phy_common_deps = out_deps.upper_phy_common_deps;
  upper_phy_common_deps.executors                       = dependencies.workers.get_upper_phy_execution_config();

  if (hal_dependencies.hw_encoder_factory) {
    upper_phy_common_deps.hw_encoder_factory = hal_dependencies.hw_encoder_factory;
  }

  if (hal_dependencies.hw_decoder_factory) {
    upper_phy_common_deps.hw_decoder_factory = hal_dependencies.hw_decoder_factory;
  }

  for (unsigned i = 0, e = nof_cells; i != e; ++i) {
    upper_phy_dependencies& upper_phy_cell    = out_deps.cells.emplace_back().upper_phy_deps;
    upper_phy_cell.rg_gateway                 = &dependencies.rg_gateway;
    upper_phy_cell.rx_symbol_request_notifier = &dependencies.rx_symbol_request_notifier;
  }

  return out_deps;
}

o_du_low_unit o_du_low_unit_factory::create(const o_du_low_unit_config& params, const o_du_low_unit_dependencies& deps)
{
  srs_du::o_du_low_config o_du_low_cfg;

  // Copy FAPI configuration.
  o_du_low_cfg.fapi_cfg = params.fapi_cfg;

  // Configure the metrics.
  o_du_low_cfg.enable_metrics = params.du_low_unit_cfg.metrics_cfg.enable_du_low;

  // Generate O-DU low configuration.
  generate_o_du_low_config(o_du_low_cfg, params.du_low_unit_cfg, params.cells);

  // Generate O-DU low dependencies.
  srs_du::o_du_low_dependencies o_du_low_deps;
  o_du_low_deps.du_low_deps = generate_du_low_dependencies(deps, hal_dependencies, params.cells.size());

  o_du_low_unit unit;
  unit.o_du_lo = srs_du::make_o_du_low(o_du_low_cfg, o_du_low_deps);
  report_error_if_not(unit.o_du_lo, "Invalid O-DU low");

  return unit;
}
