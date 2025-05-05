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

#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator_notifier.h"

namespace srsran {

class pusch_demodulator_notifier_spy : public pusch_demodulator_notifier
{
public:
  void on_provisional_stats(unsigned i_symbol, const demodulation_stats& stats) override
  {
    provisional_stats_entries.emplace_back(stats);
  }

  void on_end_stats(const demodulation_stats& stats) override { end_stats_entries.emplace_back(stats); }

  const std::vector<demodulation_stats>& get_provisional_stats_entries() const { return provisional_stats_entries; }

  const std::vector<demodulation_stats>& get_end_stats_entries() const { return end_stats_entries; }

private:
  std::vector<demodulation_stats> provisional_stats_entries;
  std::vector<demodulation_stats> end_stats_entries;
};

} // namespace srsran
