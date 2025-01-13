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

#include "apps/units/application_unit.h"
#include "apps/units/o_cu_cp/o_cu_cp_builder.h"
#include <yaml-cpp/node/node.h>

namespace srsran {
struct worker_manager_config;

/// O-RAN CU-CP application unit interface.
class o_cu_cp_application_unit : public application_unit
{
public:
  virtual ~o_cu_cp_application_unit() = default;

  /// Creates an O-RAN CU-CP unit with the given dependencies.
  virtual o_cu_cp_unit create_o_cu_cp(o_cu_cp_unit_dependencies& dependencies) = 0;

  /// Returns the CU-CP configuration of this O-RAN CU-CP application unit.
  virtual o_cu_cp_unit_config&       get_o_cu_cp_unit_config()       = 0;
  virtual const o_cu_cp_unit_config& get_o_cu_cp_unit_config() const = 0;

  /// Dumps the O-RAN CU-CP configuration into the given YAML node.
  virtual void dump_config(YAML::Node& node) const = 0;

  /// Fills the given worker manager configuration with the O-RAN CU-CP parameters.
  virtual void fill_worker_manager_config(worker_manager_config& config) = 0;
};

/// Creates an O-RAN CU-CP application unit.
std::unique_ptr<o_cu_cp_application_unit> create_o_cu_cp_application_unit(std::string_view app_name);

} // namespace srsran
