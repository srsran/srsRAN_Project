/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "downlink_processor_pool_impl.h"
#include "srsran/ran/slot_point.h"

using namespace srsran;

downlink_processor_pool_impl::downlink_processor_pool_impl(downlink_processor_pool_impl_config dl_processors)
{
  for (auto& proc : dl_processors.procs) {
    srsran_assert(!proc.procs.empty(), "Cannot store an empty processor pool");
    processors.insert(proc.scs, std::move(proc.procs));
  }
}

downlink_processor_controller& downlink_processor_pool_impl::get_processor_controller(slot_point slot)
{
  srsran_assert(slot.valid(), "Invalid slot ({}) when requesting a downlink processor", slot);
  return processors.get_processor(slot).get_controller();
}

void downlink_processor_pool_impl::stop()
{
  processors.stop();
}
