/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/units/application_unit.h"
#include "apps/units/cu_up/cu_up_builder.h"
#include <yaml-cpp/node/node.h>

namespace srsran {
struct worker_manager_config;

/// CU-UP application unit interface.
class cu_up_application_unit : public application_unit
{
public:
  virtual ~cu_up_application_unit() = default;

  /// Creates a CU-UP using the given dependencies.
  virtual std::unique_ptr<srs_cu_up::cu_up_interface>
  create_cu_up_unit(const cu_up_unit_dependencies& dependencies) = 0;

  /// Returns the CU-UP unit configuration of this CU-UP application unit.
  virtual cu_up_unit_config&       get_cu_up_unit_config()       = 0;
  virtual const cu_up_unit_config& get_cu_up_unit_config() const = 0;

  /// Dumps the CU-UP configuration into the given YAML node.
  virtual void dump_config(YAML::Node& node) const = 0;

  /// Fills the given worker manager configuration with the CU-UP parameters.
  virtual void fill_worker_manager_config(worker_manager_config& config) = 0;
};

/// Creates a CU-UP application unit.
std::unique_ptr<cu_up_application_unit> create_cu_up_application_unit(std::string_view app_name);

} // namespace srsran
