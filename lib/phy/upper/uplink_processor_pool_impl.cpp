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

#include "uplink_processor_pool_impl.h"
#include "srsran/ran/slot_point.h"

using namespace srsran;

uplink_processor_pool_impl::uplink_processor_pool_impl(uplink_processor_pool_impl_config dl_processors) :
  processors(dl_processors.num_sectors)
{
  for (auto& proc : dl_processors.procs) {
    srsran_assert(!proc.procs.empty(), "Cannot store an empty processor pool");
    processors[proc.sector].insert(proc.scs, std::move(proc.procs));
  }
}

uplink_processor& uplink_processor_pool_impl::get_processor(slot_point slot, unsigned sector_id)
{
  srsran_assert(slot.valid(), "Invalid slot ({}) when requesting an uplink processor", slot);

  return processors.back().get_processor(slot);
}
