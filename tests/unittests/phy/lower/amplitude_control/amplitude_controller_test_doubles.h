/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/phy/lower/amplitude_controller/amplitude_controller.h"
#include "srsran/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "srsran/srsvec/copy.h"
#include <random>

namespace srsran {

class amplitude_controller_spy : public amplitude_controller
{
public:
  struct entry_t {
    std::vector<cf_t> input;
    std::vector<cf_t> output;
  };

  amplitude_controller_metrics process(span<cf_t> output, span<const cf_t> input) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();

    entry.input.resize(input.size());
    srsvec::copy(entry.input, input);

    std::generate(output.begin(), output.end(), [this]() { return cf_t(output_dist(rgen), output_dist(rgen)); });

    entry.output.resize(output.size());
    srsvec::copy(entry.output, output);

    return amplitude_controller_metrics();
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> output_dist;
  std::vector<entry_t>                  entries;
};

PHY_SPY_FACTORY_TEMPLATE(amplitude_controller);

} // namespace srsran