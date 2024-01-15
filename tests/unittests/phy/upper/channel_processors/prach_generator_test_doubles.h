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
#include "srsran/ran/prach/prach_preamble_information.h"
#include <random>

namespace srsran {

class prach_generator_spy : public prach_generator
{
public:
  struct generate_entry {
    configuration     config;
    std::vector<cf_t> result;
  };

  span<const cf_t> generate(const configuration& config) override
  {
    // Extract sequence length.
    unsigned sequence_length = is_long_preamble(config.format) ? prach_constants::LONG_SEQUENCE_LENGTH
                                                               : prach_constants::SHORT_SEQUENCE_LENGTH;

    // Create entry.
    entries.emplace_back();
    generate_entry& entry = entries.back();
    entry.config          = config;
    entry.result.resize(sequence_length);

    // Generates the random result.
    std::generate(entry.result.begin(), entry.result.end(), [&, n = 0]() mutable {
      return std::polar(1.0F, static_cast<float>((n++) * config.preamble_index) / static_cast<float>(sequence_length));
    });

    return entry.result;
  }

  const std::vector<generate_entry>& get_entries() const { return entries; }

  void clear_entries() { entries.clear(); }

private:
  std::vector<generate_entry> entries;
};

PHY_SPY_FACTORY_TEMPLATE(prach_generator);

} // namespace srsran