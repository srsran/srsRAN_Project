
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

  struct awaiter_type {
    awaiter_type(async_queue* parent_) : parent(parent_) {}

    bool await_ready() { return not parent->queue.empty(); }
    bool await_suspend(coro_handle<> ch)
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
      return true;
    }
    expected<T> await_resume()
    {
      if (parent->queue.empty()) {
        return {default_error_t{}};
      }
      expected<T> ret = std::move(parent->queue.top());
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
      front->suspended_handle.operator()();
      front = front->next;
    }
  }

  awaiter_type*          front = nullptr;
  awaiter_type*          last  = nullptr;
  dyn_circular_buffer<T> queue;
};

} // namespace srsgnb

#endif // SRSGNB_ASYNC_QUEUE_H
