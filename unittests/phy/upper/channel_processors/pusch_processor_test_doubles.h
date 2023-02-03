/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"

namespace srsgnb {

class pusch_processor_dummy : public pusch_processor
{
public:
  pusch_processor_result process(span<uint8_t>               data,
                                 unique_rx_softbuffer        softbuffer,
                                 const resource_grid_reader& grid,
                                 const pdu_t&                pdu) override
  {
    return {};
  }
};

} // namespace srsgnb
