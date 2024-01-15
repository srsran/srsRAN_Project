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

#include "../coroutine.h"
#include <utility>

namespace srsran {

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

} // namespace srsran
