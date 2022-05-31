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

  void push(const Event& ev)
  {
    std::lock_guard<std::mutex> lock(mutex);
    pending_events.push_back(ev);
  }

  void push(Event&& ev)
  {
    std::lock_guard<std::mutex> lock(mutex);
    pending_events.push_back(std::move(ev));
  }

  span<const Event> get_events() const { return current_events; }

private:
  std::vector<Event> pending_events;
  std::vector<Event> current_events;
  std::mutex         mutex;
};

} // namespace srsgnb

#endif // SRSGNB_SLOT_EVENT_LIST_H
