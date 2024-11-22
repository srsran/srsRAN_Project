/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_7_2_o_du_factory.h"
#include "helpers/ru_ofh_factories.h"

using namespace srsran;

std::unique_ptr<radio_unit>
split_7_2_o_du_factory::create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                          const flexible_o_du_ru_dependencies& ru_dependencies)
{
  return create_ofh_radio_unit(unit_config.ru_cfg.config, ru_config, ru_dependencies);
}

std::unique_ptr<radio_unit>
multicell_split_7_2_du_factory::create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                  const flexible_o_du_ru_dependencies& ru_dependencies)
{
  return create_ofh_radio_unit(unit_config.ru_cfg.config, ru_config, ru_dependencies);
}
