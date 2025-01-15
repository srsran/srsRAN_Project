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

#include "apps/units/application_unit.h"
#include "apps/units/flexible_o_du/o_du_unit.h"
#include <yaml-cpp/node/node.h>

namespace srsran {
struct o_du_high_unit_config;
struct worker_manager_config;

/// Flexible DU application unit interface.
class flexible_o_du_application_unit : public application_unit
{
public:
  virtual ~flexible_o_du_application_unit() = default;

  /// Creates a flexible O-RAN DU using the given dependencies.
  virtual o_du_unit create_flexible_o_du_unit(const o_du_unit_dependencies& dependencies) = 0;

  /// Returns the O-RAN DU high unit configuration of this flexible DU.
  virtual o_du_high_unit_config&       get_o_du_high_unit_config()       = 0;
  virtual const o_du_high_unit_config& get_o_du_high_unit_config() const = 0;

  /// Dumps the flexible O-RAN DU configuration into the given YAML node.
  virtual void dump_config(YAML::Node& node) const = 0;

  /// Fills the given worker manager configuration with the flexible O-RAN DU parameters.
  virtual void fill_worker_manager_config(worker_manager_config& config) = 0;
};

/// \brief Creates a flexible O-RAN DU application unit.
///
/// Different splits must provide an implementation of this free function so the applications can instantiate a flexible
/// DU application unit.
std::unique_ptr<flexible_o_du_application_unit> create_flexible_o_du_application_unit(std::string_view app_name);

} // namespace srsran
