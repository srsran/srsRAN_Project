/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  processors(dl_processors.num_sectors)
{
  for (auto& proc : dl_processors.procs) {
    srsran_assert(!proc.procs.empty(), "Cannot store an empty processor pool");
    processors[proc.sector].insert(proc.scs, std::move(proc.procs));
  }
}

downlink_processor_controller& downlink_processor_pool_impl::get_processor_controller(slot_point slot,
                                                                                      unsigned   sector_id)
{
  srsran_assert(sector_id < processors.size(), "Invalid sector ({}) when requesting a downlink processor", sector_id);
  srsran_assert(slot.valid(), "Invalid slot ({}) when requesting a downlink processor", slot);

  return processors[sector_id].get_processor(slot);
}
