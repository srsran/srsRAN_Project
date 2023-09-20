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

#pragma once

#include "srsran/adt/ring_buffer.h"
#include "srsran/support/async/coroutine.h"
#include "srsran/support/async/manual_event.h"

namespace srsran {

template <typename T>
class async_queue
{
public:
  async_queue(size_t queue_size) : queue(queue_size) {}
  async_queue(const async_queue&)            = delete;
  async_queue& operator=(const async_queue&) = delete;

  // Pushing interface
  bool try_push(const T& t)
  {
    if (queue.try_push(t)) {
      notify_one_awaiter();
      return true;
    }
    return false;
  }
  bool try_push(T&& t)
  {
    if (queue.try_push(std::move(t))) {
      notify_one_awaiter();
      return true;
    }
    return false;
  }

  void clear() { queue.clear(); }

  size_t size() const { return queue.size(); }

  struct awaiter_type {
    awaiter_type(async_queue* parent_) : parent(parent_) {}

    bool await_ready() { return not parent->queue.empty(); }
    void await_suspend(coro_handle<> ch)
    {
      srsran_assert(this->next == nullptr, "Trying to suspend already suspended coroutine");
      suspended_handle = ch;
      // enqueue awaiter
      if (parent->last == nullptr) {
        // queue of awaiters is empty.
        parent->last  = this;
        parent->front = this;
      } else {
        parent->last->next = this;
        parent->last       = this;
      }
    }
    T await_resume()
    {
      srsran_sanity_check(not parent->queue.empty(), "Callback being resumed but queue is still empty");
      T ret = std::move(parent->queue.top());
      parent->queue.pop();
      return ret;
    }

    async_queue*  parent;
    awaiter_type* next = nullptr;
    coro_handle<> suspended_handle;
  };

  awaiter_type get_awaiter() { return {this}; }

private:
  void notify_one_awaiter()
  {
    if (front != nullptr) {
      awaiter_type* to_resume = front;
      if (front == last) {
        front = nullptr;
        last  = nullptr;
      } else {
        front = front->next;
      }
      to_resume->suspended_handle.resume();
    }
  }

  awaiter_type*  front = nullptr;
  awaiter_type*  last  = nullptr;
  ring_buffer<T> queue;
};

} // namespace srsran
