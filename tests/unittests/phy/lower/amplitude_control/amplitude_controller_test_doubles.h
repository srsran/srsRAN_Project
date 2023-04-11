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