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

#include "srsran/adt/span.h"
#include <mutex>
#include <vector>

namespace srsran {

/// This class stores incoming events in a thread-safe queue, and pops all enqueued events in a single batch, when
/// a new slot indication is notified.
/// The advantage of this class over a mutexed queue is the lower contention caused when popping elements from the
/// queue. This is particularly relevant because the events need to be processed during the scheduler slot handling.
/// \tparam Event type of event pushed/popped from the list.
template <typename Event>
class slot_event_list
{
public:
  void reserve(unsigned cap)
  {
    std::lock_guard<std::mutex> lock(mutex);
    pending_events.reserve(cap);
    current_events.reserve(cap);
  }

  void slot_indication()
  {
    current_events.clear();
    std::lock_guard<std::mutex> lock(mutex);
    pending_events.swap(current_events);
  }

  template <typename Ev>
  void push(Ev&& ev)
  {
    static_assert(std::is_convertible<Ev, Event>::value, "Invalid type");
    std::lock_guard<std::mutex> lock(mutex);
    pending_events.push_back(std::forward<Ev>(ev));
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    std::lock_guard<std::mutex> lock(mutex);
    pending_events.template emplace_back(std::forward<Args>(args)...);
  }

  span<Event> get_events() { return current_events; }

private:
  /// Stores all enqueued events that are going to be processed in the next slot_indication.
  std::vector<Event> pending_events;

  /// Contains the events being processed in the current slot.
  /// Note: the transfer of next_events to current_events is done via a std::swap, which for std::vector is very fast.
  std::vector<Event> current_events;

  std::mutex mutex;
};

} // namespace srsran
