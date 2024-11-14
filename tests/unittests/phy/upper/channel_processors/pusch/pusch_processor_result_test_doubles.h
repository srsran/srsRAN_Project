/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
