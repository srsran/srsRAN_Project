/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_ASYNC_QUEUE_H
#define SRSGNB_ASYNC_QUEUE_H

#include "srsgnb/adt/circular_buffer.h"
#include "srsgnb/support/async/coroutine.h"
#include "srsgnb/support/async/manual_event.h"

namespace srsgnb {

template <typename T>
class async_queue
{
public:
  async_queue(size_t queue_size) : queue(queue_size) {}
  async_queue(const async_queue&) = delete;
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

  size_t size() const { return queue.size(); }

  struct awaiter_type {
    awaiter_type(async_queue* parent_) : parent(parent_) {}

    bool await_ready() { return not parent->queue.empty(); }
    void await_suspend(coro_handle<> ch)
    {
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
      front->suspended_handle.resume();
      front = front->next;
    }
  }

  awaiter_type*          front = nullptr;
  awaiter_type*          last  = nullptr;
  dyn_circular_buffer<T> queue;
};

} // namespace srsgnb

#endif // SRSGNB_ASYNC_QUEUE_H
