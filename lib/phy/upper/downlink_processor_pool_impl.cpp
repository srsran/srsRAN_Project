/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "downlink_processor_pool_impl.h"
using namespace srsgnb;

downlink_processor_pool_impl::downlink_processor_pool_impl(downlink_processor_pool_config_impl dl_processors) :
  processors(dl_processors.num_sectors)
{
  for (auto& proc : dl_processors.procs) {
    srsran_assert(!proc.procs.empty(), "Cannot store an empty processor pool");
    processors[proc.sector].insert(proc.numerology, std::move(proc.procs));
  }
}

downlink_processor& downlink_processor_pool_impl::get_processor(const slot_point& slot, unsigned int sector)
{
  srsran_assert(sector < processors.size(), "Invalid sector ({}) when requesting a downlink processor", sector);
  srsran_assert(slot.valid(), "Invalid slot ({}) when requesting a downlink processor", slot);

  auto&    sector_proc = processors[sector];
  auto&    proc_pool   = sector_proc[slot.numerology()];
  unsigned index       = slot.system_slot() % proc_pool.size();
  return *proc_pool[index];
}
