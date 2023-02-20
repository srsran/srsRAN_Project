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
#include "srsran/ran/slot_point.h"

using namespace srsran;

downlink_processor_pool_impl::downlink_processor_pool_impl(downlink_processor_pool_impl_config dl_processors) :
  processors(dl_processors.num_sectors), null_proc(std::move(dl_processors.null_proc))
{
  for (auto& proc : dl_processors.procs) {
    srsgnb_assert(!proc.procs.empty(), "Cannot store an empty processor pool.");
    processors[proc.sector].insert(proc.scs, std::move(proc.procs));
  }
  srsgnb_assert(null_proc, "Invalid null processor.");
}

downlink_processor& downlink_processor_pool_impl::get_processor(slot_point slot, unsigned sector_id)
{
  srsgnb_assert(sector_id < processors.size(), "Invalid sector ({}) when requesting a downlink processor.", sector_id);
  srsgnb_assert(slot.valid(), "Invalid slot ({}) when requesting a downlink processor.", slot);

  downlink_processor& proc = processors[sector_id].get_processor(slot);
  if (proc.is_reserved()) {
    return *null_proc;
  }
  return proc;
}
