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

#include "apps/units/flexible_o_du/flexible_o_du_application_unit.h"
#include "split6_o_du_unit_config.h"
#include "split6_plugin.h"

namespace srsran {

/// O-RAN DU Split6 application unit implementation.
class split6_o_du_application_unit_impl : public flexible_o_du_application_unit
{
public:
  split6_o_du_application_unit_impl(std::string_view app_name, std::unique_ptr<split6_plugin> plugin_);

  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override;

  // See interface for documentation.
  void on_configuration_parameters_autoderivation(CLI::App& app) override;

  // See interface for documentation.
  bool on_configuration_validation() const override;

  // See interface for documentation.
  void on_loggers_registration() override;

  // See interface for documentation.
  bool are_metrics_enabled() const override;

  // See interface for documentation.
  o_du_unit create_flexible_o_du_unit(const o_du_unit_dependencies& dependencies) override;

  // See interface for documentation.
  o_du_high_unit_config&       get_o_du_high_unit_config() override { return unit_cfg.odu_high_cfg; }
  const o_du_high_unit_config& get_o_du_high_unit_config() const override { return unit_cfg.odu_high_cfg; }

  // See interface for documentation.
  void dump_config(YAML::Node& node) const override;

  // See interface for documentation.
  void fill_worker_manager_config(worker_manager_config& config) override;

private:
  split6_o_du_unit_config        unit_cfg;
  std::unique_ptr<split6_plugin> plugin;
};

} // namespace srsran
