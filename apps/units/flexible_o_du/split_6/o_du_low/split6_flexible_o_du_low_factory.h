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
