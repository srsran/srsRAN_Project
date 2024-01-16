/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
