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

#include "apps/units/flexible_o_du/o_du_unit.h"
#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "srsran/ru/ru.h"

namespace srsran {

class flexible_o_du_metrics_notifier;

/// Flexible O-RAN DU factory interface.
class flexible_o_du_factory
{
  const flexible_o_du_unit_config config;

public:
  explicit flexible_o_du_factory(const flexible_o_du_unit_config& config_) : config(config_) {}
  virtual ~flexible_o_du_factory() = default;

  /// Creates a flexible O-RAN DU using the given dependencies and configuration.
  o_du_unit create_flexible_o_du(const o_du_unit_dependencies& dependencies);

private:
  /// Creates a Radio Unit using the given config and dependencies.
  virtual std::unique_ptr<radio_unit> create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                        const flexible_o_du_ru_dependencies& ru_dependencies) = 0;
};

} // namespace srsran
