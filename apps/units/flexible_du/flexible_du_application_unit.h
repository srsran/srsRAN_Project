/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "apps/units/application_unit.h"
#include "apps/units/flexible_du/du_unit.h"
#include <yaml-cpp/node/node.h>

namespace srsran {
struct worker_manager_config;
struct du_high_unit_config;

/// Flexible DU application unit interface.
class flexible_du_application_unit : public application_unit
{
public:
  virtual ~flexible_du_application_unit() = default;

  /// Creates a flexible DU using the given dependencies.
  virtual du_unit create_flexible_du_unit(const du_unit_dependencies& dependencies) = 0;

  /// Returns the DU high unit configuration of this flexible DU.
  virtual du_high_unit_config&       get_du_high_unit_config()       = 0;
  virtual const du_high_unit_config& get_du_high_unit_config() const = 0;

  /// Dumps the flexible DU configuration into the given YAML node.
  virtual void dump_config(YAML::Node& node) const = 0;

  /// Fills the given worker manager configuration with the flexible parameters.
  virtual void fill_worker_manager_config(worker_manager_config& config) = 0;
};

/// \brief Creates a flexible DU application unit.
///
/// Different splits must provide an implementation of this free function so the applications can instantiate a flexible
/// DU application unit. Only one of these implementations should be compiled, based on the DU_SPLIT_TYPE cmake
/// definition.
std::unique_ptr<flexible_du_application_unit> create_flexible_du_application_unit(std::string_view app_name);

} // namespace srsran
