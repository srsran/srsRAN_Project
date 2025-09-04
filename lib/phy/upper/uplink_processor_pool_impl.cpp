/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uplink_processor_pool_impl.h"
#include "srsran/ran/slot_point.h"

using namespace srsran;

uplink_processor_pool_impl::uplink_processor_pool_impl(uplink_processor_pool_impl_config ul_processors) :
  default_processor(std::move(ul_processors.default_processor))
{
  srsran_assert(default_processor, "Invalid default processor");
  for (auto& proc : ul_processors.procs) {
    srsran_assert(!proc.procs.empty(), "Cannot store an empty processor pool");
    processors.insert(proc.scs, std::move(proc.procs));
  }
}

void uplink_processor_pool_impl::stop()
{
  processors.stop();
}

uplink_slot_processor& uplink_processor_pool_impl::get_slot_processor(slot_point slot)
{
  srsran_assert(slot.valid(), "Invalid slot ({}) when requesting an uplink processor", slot);
  uplink_processor* processor = assigned_processors[slot.system_slot()].load();
  if (processor) {
    return processor->get_slot_processor(slot);
  }
  return default_processor->get_slot_processor(slot);
}

unique_uplink_pdu_slot_repository uplink_processor_pool_impl::get_pdu_slot_repository(slot_point slot)
{
  srsran_assert(slot.valid(), "Invalid slot ({}) when requesting an uplink processor", slot);

  // Get processor.
  uplink_processor& processor = processors.get_processor(slot);

  // Assign the processor to the circular array, where it will later be retrieved by the receiver symbol handler.
  assigned_processors[slot.system_slot()] = &processor;

  return processor.get_pdu_slot_repository(slot);
}
