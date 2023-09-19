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
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/phy/upper/channel_state_information.h"

namespace srsran {

class pusch_processor_result_notifier_spy : public pusch_processor_result_notifier
{
public:
  void on_uci(const pusch_processor_result_control& uci) override { uci_entries.emplace_back(uci); }

  void on_sch(const pusch_processor_result_data& sch) override { sch_entries.emplace_back(sch); }

  const std::vector<pusch_processor_result_control>& get_uci_entries() const { return uci_entries; }

  const std::vector<pusch_processor_result_data>& get_sch_entries() const { return sch_entries; }

  void clear()
  {
    uci_entries.clear();
    sch_entries.clear();
  }

private:
  std::vector<pusch_processor_result_control> uci_entries;
  std::vector<pusch_processor_result_data>    sch_entries;
};

} // namespace srsran