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

#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include <vector>

namespace srsran {

class pusch_decoder_notifier_spy : public pusch_decoder_notifier
{
public:
  void on_sch_data(const pusch_decoder_result& result) override { entries.emplace_back(result); }

  const std::vector<pusch_decoder_result>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<pusch_decoder_result> entries;
};

} // namespace srsran
