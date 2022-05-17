/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
