/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SLOT_EVENT_LIST_H
#define SRSGNB_SLOT_EVENT_LIST_H

#include "srsgnb/adt/span.h"
#include <mutex>
#include <vector>

namespace srsgnb {

template <typename Event>
class slot_event_list
{
public:
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

} // namespace srsgnb

#endif // SRSGNB_SLOT_EVENT_LIST_H
