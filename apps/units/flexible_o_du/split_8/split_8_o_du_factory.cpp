/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_8_o_du_factory.h"
#include "helpers/ru_sdr_factories.h"

using namespace srsran;

split8_du_factory::split8_du_factory(const split_8_o_du_unit_config& config_) :
  flexible_o_du_factory({config_.odu_high_cfg,
                         config_.du_low_cfg,
                         {config_.ru_cfg.metrics_cfg.metrics_cfg, config_.ru_cfg.metrics_cfg.enable_ru_metrics}}),
  unit_config(config_)
{
}

std::unique_ptr<radio_unit> split8_du_factory::create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                                 const flexible_o_du_ru_dependencies& ru_dependencies)
{
  return create_sdr_radio_unit(unit_config.ru_cfg, ru_config, ru_dependencies);
}
