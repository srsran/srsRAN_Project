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

#include "pdxch_processor_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

void pdxch_processor_impl::connect(pdxch_processor_notifier& notifier_)
{
  notifier = &notifier_;
}

pdxch_processor_request_handler& pdxch_processor_impl::get_request_handler()
{
  return *this;
}

pdxch_processor_baseband& pdxch_processor_impl::get_baseband()
{
  return *this;
}

lower_phy_center_freq_controller& pdxch_processor_impl::get_center_freq_control()
{
  return *this;
}

void pdxch_processor_impl::handle_request(const shared_resource_grid& grid, const resource_grid_context& context)
{
  // Ignore request if the processor has stopped.
  if (stopped) {
    return;
  }

  srsran_assert(notifier != nullptr, "Notifier has not been connected.");

  // Ignore grid if it is empty.
  if (grid.get_reader().is_empty()) {
    return;
  }

  // Obtain baseband buffer.
  baseband_gateway_buffer_ptr buffer = bb_buffers.get();
  if (!buffer) {
    logger.error(context.slot.sfn(), context.slot.slot_index(), "Insufficient number of buffers.");
    return;
  }

  // Try modulating the request. It can be the modulator is busy with a previous transmission.
  bool success = get_modulator(context.slot).handle_request(std::move(buffer), grid.copy(), context);

  // If there was a request with a resource grid, then notify a late event with the context of the discarded request.
  if (!success) {
    logger.error(context.slot.sfn(), context.slot.slot_index(), "The modulator is busy.");
    l1_dl_tracer << instant_trace_event{"modulator_busy", instant_trace_event::cpu_scope::thread};
  }
}

void pdxch_processor_impl::on_modulation_completion(pdxch_processor_baseband::slot_result result,
                                                    resource_grid_context                 context)
{
  // Write modulation result in the request buffer.
  auto request = requests.exchange({context.slot, std::move(result)});

  // Check if the previous entry in the request buffer is not empty.
  if (request.resource.buffer) {
    resource_grid_context late_context;
    late_context.slot   = request.slot;
    late_context.sector = context.sector;
    notifier->on_pdxch_request_late(late_context);
    l1_dl_tracer << instant_trace_event{"on_pdxch_request_late", instant_trace_event::cpu_scope::thread};
  }
}

bool pdxch_processor_impl::set_carrier_center_frequency(double carrier_center_frequency_Hz)
{
  common_ofdm_modulator->set_center_frequency(carrier_center_frequency_Hz);
  return true;
}

pdxch_processor_baseband::slot_result pdxch_processor_impl::process_slot(slot_context context)
{
  // Exchange an empty request with the current slot with a stored request.
  auto request = requests.exchange({.slot = context.slot, .resource = {}});

  // If the request buffer is invalid, the request is empty.
  if (!request.resource.buffer) {
    return {};
  }

  // If the slot of the request does not match the current slot, then notify a late event.
  if (context.slot != request.slot) {
    resource_grid_context late_context;
    late_context.slot   = request.slot;
    late_context.sector = context.sector;
    notifier->on_pdxch_request_late(late_context);
    return {};
  }

  // Return the result.
  return std::move(request.resource);
}
