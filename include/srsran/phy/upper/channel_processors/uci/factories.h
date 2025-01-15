/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
#include <memory>

namespace srsran {

class uci_decoder_factory
{
public:
  virtual ~uci_decoder_factory()                = default;
  virtual std::unique_ptr<uci_decoder> create() = 0;
};

std::shared_ptr<uci_decoder_factory>
create_uci_decoder_factory_generic(std::shared_ptr<short_block_detector_factory> decoder_factory,
                                   std::shared_ptr<polar_factory>                polar_factory,
                                   std::shared_ptr<crc_calculator_factory>       crc_calc_factory);

} // namespace srsran
