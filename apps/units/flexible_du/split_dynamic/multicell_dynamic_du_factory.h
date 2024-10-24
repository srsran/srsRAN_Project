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

#include "apps/units/flexible_du/split_dynamic/dynamic_du_unit_config.h"
#include "apps/units/flexible_du/split_helpers/multicell_flexible_du_factory.h"

namespace srsran {

class multicell_dynamic_du_factory : public multicell_flexible_du_factory
{
  const dynamic_du_unit_config& unit_config;

public:
  explicit multicell_dynamic_du_factory(const dynamic_du_unit_config& config_) :
    multicell_flexible_du_factory({config_.odu_high_cfg, config_.du_low_cfg}), unit_config(config_)
  {
  }

private:
  std::unique_ptr<radio_unit> create_radio_unit(const flexible_du_ru_config&       ru_config,
                                                const flexible_du_ru_dependencies& ru_dependencies) override;
};

} // namespace srsran
