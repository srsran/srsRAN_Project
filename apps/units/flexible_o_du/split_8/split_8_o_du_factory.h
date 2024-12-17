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

#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_factory.h"
#include "split_8_o_du_unit_config.h"

namespace srsran {

/// Split 8 O-RAN DU factory.
class split8_du_factory : public flexible_o_du_factory
{
  const split_8_o_du_unit_config& unit_config;

public:
  explicit split8_du_factory(const split_8_o_du_unit_config& config_) :
    flexible_o_du_factory({config_.odu_high_cfg, config_.du_low_cfg}), unit_config(config_)
  {
  }

private:
  std::unique_ptr<radio_unit> create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                const flexible_o_du_ru_dependencies& ru_dependencies) override;
};

} // namespace srsran
