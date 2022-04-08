
#ifndef SRSGNB_EVENT_IMPL_H
#define SRSGNB_EVENT_IMPL_H

#include "../coroutine.h"

namespace srsgnb {

namespace detail {

/// Iterates over linked list of awaiters and calls resume for each awaiter stored coroutine handle
template <typename AwaiterType>
void flush_awaiter_list(AwaiterType* current)
{
  while (current != nullptr) {
    auto* next = current->next;
    current->suspended_handle.resume();
    current = next;
  }
}

} // namespace detail

} // namespace srsgnb

#endif // SRSGNB_EVENT_IMPL_H
