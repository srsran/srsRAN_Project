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

#include "../../phy_test_utils.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/pucch_detector.h"
#include <random>

namespace srsran {

class pucch_detector_spy : public pucch_detector
{
public:
  struct entry_format1 {
    const resource_grid_reader* grid;
    const channel_estimate*     estimate;
    format1_configuration       config;
    pucch_uci_message           msg;
  };

  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&  grid,
                                                                 const format0_configuration& config) override
  {
    return std::pair<pucch_uci_message, channel_state_information>();
  }

  pucch_detection_result detect(const resource_grid_reader&  grid,
                                const channel_estimate&      estimates,
                                const format1_configuration& config) override
  {
    entries_format1.emplace_back();
    entry_format1& entry        = entries_format1.back();
    entry.grid                  = &grid;
    entry.estimate              = &estimates;
    entry.config                = config;
    entry.msg                   = pucch_uci_message({0, config.nof_harq_ack, 0, 0});
    span<uint8_t> harq_ack_bits = entry.msg.get_harq_ack_bits();
    std::generate(harq_ack_bits.begin(), harq_ack_bits.end(), [this]() { return rgen() & 1; });
    return {entry.msg, 1};
  }

  const std::vector<entry_format1>& get_entries_format1() const { return entries_format1; }

  void clear() { entries_format1.clear(); }

private:
  std::mt19937               rgen;
  std::vector<entry_format1> entries_format1;
};

PHY_SPY_FACTORY_TEMPLATE(pucch_detector);

} // namespace srsran
