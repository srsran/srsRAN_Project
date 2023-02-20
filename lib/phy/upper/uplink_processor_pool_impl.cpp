/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uplink_processor_pool_impl.h"
#include "srsgnb/ran/slot_point.h"

using namespace srsran;

uplink_processor_pool_impl::uplink_processor_pool_impl(uplink_processor_pool_impl_config dl_processors) :
  processors(dl_processors.num_sectors)
{
  for (auto& proc : dl_processors.procs) {
    srsgnb_assert(!proc.procs.empty(), "Cannot store an empty processor pool.");
    processors[proc.sector].insert(proc.scs, std::move(proc.procs));
  }
}

uplink_processor& uplink_processor_pool_impl::get_processor(slot_point slot, unsigned sector_id)
{
  srsgnb_assert(sector_id < processors.size(), "Invalid sector ({}) when requesting an uplink processor.", sector_id);
  srsgnb_assert(slot.valid(), "Invalid slot ({}) when requesting an uplink processor.", slot);

  return processors[sector_id].get_processor(slot);
}
