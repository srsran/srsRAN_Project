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
#include "tx_softbuffer_test_doubles.h"
#include "srsran/phy/upper/tx_buffer_pool.h"

namespace srsran {

class tx_softbuffer_pool_spy : public tx_buffer_pool
{
  struct reserve_softbuffer_entry {
    slot_point           slot;
    tx_buffer_identifier id;
    unsigned             nof_codeblocks;
  };

public:
  unique_tx_buffer
  reserve_buffer(const slot_point& slot, const tx_buffer_identifier& id, unsigned nof_codeblocks) override
  {
    reserve_softbuffer_entries.emplace_back();
    reserve_softbuffer_entry& entry = reserve_softbuffer_entries.back();
    entry.slot                      = slot;
    entry.id                        = id;
    entry.nof_codeblocks            = nof_codeblocks;
    return unique_tx_buffer(softbuffer_spy);
  }

  unique_tx_buffer reserve_buffer(const slot_point& slot, unsigned int nof_codeblocks) override
  {
    reserve_softbuffer_entries.emplace_back();
    reserve_softbuffer_entry& entry = reserve_softbuffer_entries.back();
    entry.slot                      = slot;
    entry.id                        = {};
    entry.nof_codeblocks            = nof_codeblocks;
    return unique_tx_buffer(softbuffer_spy);
  }

  void run_slot(const slot_point& slot) override { run_slot_entries.emplace_back(slot); }

  const std::vector<reserve_softbuffer_entry>& get_reserve_softbuffer_entries() const
  {
    return reserve_softbuffer_entries;
  }

  const std::vector<slot_point>& get_run_slot_entries() const { return run_slot_entries; }

private:
  std::vector<reserve_softbuffer_entry> reserve_softbuffer_entries;
  std::vector<slot_point>               run_slot_entries;
  tx_buffer_spy                         softbuffer_spy;
};

} // namespace srsran