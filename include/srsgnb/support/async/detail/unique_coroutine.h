/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../coroutine.h"
#include <utility>

namespace srsgnb {

/// Handle to coroutine object that ensures safe move ctor, move assignment and destruction.
template <typename Promise>
class unique_coroutine
{
public:
  using promise_type = Promise;

  unique_coroutine() = default;
  explicit unique_coroutine(coro_handle<promise_type> cb) : handle(cb) {}
  unique_coroutine(const unique_coroutine&)            = delete;
  unique_coroutine& operator=(const unique_coroutine&) = delete;
  unique_coroutine(unique_coroutine&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
  unique_coroutine& operator=(unique_coroutine&& other) noexcept
  {
    if (not handle.empty()) {
      handle.destroy();
    }
    handle = std::exchange(other.handle, nullptr);
    return *this;
  }
  ~unique_coroutine()
  {
    if (not handle.empty()) {
      handle.destroy();
    }
  }

  coro_handle<promise_type>& operator*() { return handle; }

  bool empty() const { return handle.empty(); }

  void resume() { handle.resume(); }

  promise_type&       promise() { return handle.promise(); }
  const promise_type& promise() const { return handle.promise(); }

private:
  coro_handle<promise_type> handle;
};

} // namespace srsgnb
