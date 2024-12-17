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

#include "apps/units/flexible_o_du/o_du_unit.h"
#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "srsran/ru/ru.h"

namespace srsran {

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
