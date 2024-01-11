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
#include "tx_buffer_test_doubles.h"
#include "srsran/phy/upper/tx_buffer_pool.h"

namespace srsran {

class tx_buffer_pool_spy : public tx_buffer_pool
{
  struct reserve_entry {
    slot_point            slot;
    trx_buffer_identifier id;
    unsigned              nof_codeblocks;
  };

public:
  unique_tx_buffer reserve(const slot_point& slot, trx_buffer_identifier id, unsigned nof_codeblocks) override
  {
    reserve_entries.emplace_back();
    reserve_entry& entry = reserve_entries.back();
    entry.slot           = slot;
    entry.id             = id;
    entry.nof_codeblocks = nof_codeblocks;
    return unique_tx_buffer(rm_buffer_spy);
  }

  unique_tx_buffer reserve(const slot_point& slot, unsigned nof_codeblocks) override
  {
    reserve_entries.emplace_back();
    reserve_entry& entry = reserve_entries.back();
    entry.slot           = slot;
    entry.id             = trx_buffer_identifier::invalid();
    entry.nof_codeblocks = nof_codeblocks;
    return unique_tx_buffer(rm_buffer_spy);
  }

  void run_slot(const slot_point& slot) override { run_slot_entries.emplace_back(slot); }

  const std::vector<reserve_entry>& get_reserve_entries() const { return reserve_entries; }

  const std::vector<slot_point>& get_run_slot_entries() const { return run_slot_entries; }

private:
  std::vector<reserve_entry> reserve_entries;
  std::vector<slot_point>    run_slot_entries;
  tx_buffer_spy              rm_buffer_spy;
};

} // namespace srsran