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

#include "srsran/phy/upper/channel_processors/pusch_processor.h"

namespace srsran {

class pusch_processor_dummy : public pusch_processor
{
public:
  pusch_processor_result
  process(span<uint8_t> data, rx_softbuffer& softbuffer, const resource_grid_reader& grid, const pdu_t& pdu) override
  {
    return {};
  }
};

} // namespace srsran
