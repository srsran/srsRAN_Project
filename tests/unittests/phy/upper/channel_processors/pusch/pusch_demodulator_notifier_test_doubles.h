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
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator_notifier.h"

namespace srsran {

class pusch_demodulator_notifier_spy : public pusch_demodulator_notifier
{
public:
  void on_provisional_stats(const demodulation_stats& stats) override { provisional_stats_entries.emplace_back(stats); }

  void on_end_stats(const demodulation_stats& stats) override { end_stats_entries.emplace_back(stats); }

  const std::vector<demodulation_stats>& get_provisional_stats_entries() const { return provisional_stats_entries; }

  const std::vector<demodulation_stats>& get_end_stats_entries() const { return end_stats_entries; }

private:
  std::vector<demodulation_stats> provisional_stats_entries;
  std::vector<demodulation_stats> end_stats_entries;
};

} // namespace srsran
