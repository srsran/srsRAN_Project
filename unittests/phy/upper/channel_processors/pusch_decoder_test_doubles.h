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

#include "../../phy_test_utils.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"

namespace srsgnb {

class pusch_decoder_spy : public pusch_decoder
{
public:
  void decode(span<uint8_t>                    transport_block,
              pusch_decoder_result&            stats,
              rx_softbuffer*                   soft_codeword,
              span<const log_likelihood_ratio> llrs,
              const configuration&             cfg) override
  {
    // TBD.
  }
};

PHY_SPY_FACTORY_TEMPLATE(pusch_decoder);

} // namespace srsgnb