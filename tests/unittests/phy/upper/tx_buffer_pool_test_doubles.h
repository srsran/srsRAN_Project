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