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
#include "apps/units/flexible_du/o_du_unit.h"
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
  virtual o_du_unit create_flexible_du_unit(const du_unit_dependencies& dependencies, bool use_multicell = false) = 0;

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
/// DU application unit.
std::unique_ptr<flexible_du_application_unit> create_flexible_du_application_unit(std::string_view app_name);

} // namespace srsran
