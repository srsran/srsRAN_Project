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
#include "apps/units/flexible_du/du_unit.h"
#include "split6_du_unit_config.h"
#include "split6_plugin.h"

namespace srsran {

/// DU Split6 application unit implementation.
class split6_du_application_unit_impl : public application_unit
{
public:
  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override;

  // See interface for documentation.
  bool on_configuration_validation() const override;

  // See interface for documentation.
  void on_loggers_registration() override;

  /// Creates the DU unit.
  du_unit create_unit(const du_unit_dependencies& dependencies);

private:
  split6_du_unit_config          unit_cfg;
  std::unique_ptr<split6_plugin> plugin;
};

} // namespace srsran
