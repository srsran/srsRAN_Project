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

#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory_config.h"
#include "split6_flexible_o_du_low_impl.h"
#include "split6_o_du_low_unit_config.h"
#include <memory>

namespace srsran {

namespace fapi {
struct fapi_cell_config;
}
struct worker_manager;

/// Split 6 flexible O-DU low abstract factory.
class split6_flexible_o_du_low_factory
{
  split6_o_du_low_unit_config unit_config;
  worker_manager&             workers;

public:
  std::unique_ptr<split6_flexible_o_du_low_impl>
  create_split6_flexible_o_du_low_impl(const fapi::fapi_cell_config& config);

private:
  /// Creates the split 6 slot configurator plugin.
  virtual std::unique_ptr<split6_slot_configurator_plugin> create_plugin() = 0;

  /// Creates Radio Unit.
  std::unique_ptr<radio_unit> create_radio_unit(split6_flexible_o_du_low_impl& odu_low,
                                                const fapi::fapi_cell_config&  config);

  /// Creates O-DU low.
  std::unique_ptr<srs_du::o_du_low> create_o_du_low(const fapi::fapi_cell_config& config,
                                                    o_du_low_unit_dependencies&&  odu_low_dependencies);
};

/// Creates a split 6 flexible O-DU low factory.
std::unique_ptr<split6_flexible_o_du_low_factory> create_split6_flexible_o_du_low_factory();

} // namespace srsran
