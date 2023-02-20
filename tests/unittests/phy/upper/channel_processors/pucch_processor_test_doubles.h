/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/pucch_processor.h"

namespace srsran {

class pucch_processor_dummy : public pucch_processor
{
public:
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format1_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    return {};
  }
};

} // namespace srsran
