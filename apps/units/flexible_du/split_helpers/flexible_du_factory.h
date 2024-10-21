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

#include "apps/units/flexible_du/du_unit.h"
#include "apps/units/flexible_du/split_helpers/flexible_du_configs.h"
#include "srsran/ru/ru.h"

namespace srsran {

/// Flexible DU factory interface.
class flexible_du_factory
{
  const flexible_du_unit_config config;

public:
  explicit flexible_du_factory(const flexible_du_unit_config& config_) : config(config_) {}

  virtual ~flexible_du_factory() = default;

  /// Creates a flexible DU using the given dependencies and configuration.
  du_unit create_flexible_du(const du_unit_dependencies& dependencies);

private:
  /// Creates a Radio Unit using the given config and dependencies.
  virtual std::unique_ptr<radio_unit> create_radio_unit(const flexible_du_ru_config&       ru_config,
                                                        const flexible_du_ru_dependencies& ru_dependencies) = 0;
};

} // namespace srsran
