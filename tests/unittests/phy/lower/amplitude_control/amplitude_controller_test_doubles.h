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

  amplitude_controller_spy() : random_data(1009)
  {
    std::mt19937                          rgen;
    std::uniform_real_distribution<float> sample_dist(-1, 1);
    std::generate(random_data.begin(), random_data.end(), [&sample_dist, &rgen]() {
      return cf_t(sample_dist(rgen), sample_dist(rgen));
    });
  }

  amplitude_controller_metrics process(span<cf_t> output, span<const cf_t> input) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();

    entry.input = std::vector<cf_t>(input.begin(), input.end());

    std::size_t remaining = output.size();
    span<cf_t>  random_data_view(random_data);

    while (remaining > 0) {
      // Number of samples to copy for this iteration.
      unsigned   nof_copied_samples = std::min(random_data.size() - random_data_index, remaining);
      span<cf_t> random_samples     = random_data_view.subspan(random_data_index, nof_copied_samples);
      span<cf_t> out_samples        = output.subspan(output.size() - remaining, nof_copied_samples);

      srsvec::copy(out_samples, random_samples);

      // Update remaining samples and buffer index.
      remaining -= nof_copied_samples;
      random_data_index = (random_data_index + nof_copied_samples) % random_data.size();
    }

    entry.output = std::vector<cf_t>(output.begin(), output.end());

    return amplitude_controller_metrics();
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  unsigned                              random_data_index = 0;
  std::vector<cf_t>                     random_data;
  std::uniform_real_distribution<float> output_dist;
  std::vector<entry_t>                  entries;
};

PHY_SPY_FACTORY_TEMPLATE(amplitude_controller);

} // namespace srsran
