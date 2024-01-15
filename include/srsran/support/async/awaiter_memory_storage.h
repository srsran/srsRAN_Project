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

#include "srsran/support/srsran_assert.h"
#include <cstddef>
#include <memory>
#include <vector>

namespace srsran {

namespace detail {

/// Memory chunk to store object. It uses small object optimization
template <std::size_t SmallBufferSize, std::size_t SmallBufferAlignment = alignof(std::max_align_t)>
struct small_memory_buffer_t {
  ~small_memory_buffer_t() { srsran_sanity_check(empty(), "Removing small memory buffer without deleting object"); }

  template <typename Object>
  Object* get()
  {
    return reinterpret_cast<Object*>(mem_ptr);
  }

  template <typename Object, typename... Args>
  void emplace(Args&&... args)
  {
    static_assert(alignof(Object) <= SmallBufferAlignment, "Trying to store object with invalid alignment");
    srsran_sanity_check(empty(), "Overwrites are not supported");
    if (sizeof(Object) <= sizeof(inline_storage)) {
      mem_ptr = &inline_storage;
    } else {
      out_storage.resize(sizeof(Object));
      mem_ptr = out_storage.data();
    }
    new (mem_ptr) Object(std::forward<Args>(args)...);
  }

  /// Clear memory buffer
  template <typename Object>
  void clear()
  {
    get<Object>()->~Object();
    mem_ptr = nullptr;
  }

  /// Checks whether memory buffer has no object
  bool empty() const { return mem_ptr == nullptr; }

private:
  std::aligned_storage_t<SmallBufferSize, SmallBufferAlignment> inline_storage;
  std::vector<uint8_t>                                          out_storage;
  void*                                                         mem_ptr = nullptr;
};

/// Metafunction to get Awaiter Type from Awaitable
template <typename Awaitable>
using awaiter_t = decltype(std::declval<Awaitable>().get_awaiter());

/// Memory chunk to store Awaitable (value or reference) + Awaiter
template <std::size_t InlineBuffer>
struct awaiter_memory_storage_t {
  template <typename Awaitable>
  struct stored_awaiter {
    using awaitable_type = std::decay_t<Awaitable>;
    using awaiter_type   = awaiter_t<Awaitable>;

    explicit stored_awaiter(Awaitable a) : awaitable(std::forward<Awaitable>(a)), awaiter(awaitable.get_awaiter()) {}
    stored_awaiter(const stored_awaiter<Awaitable>&)            = delete;
    stored_awaiter(stored_awaiter<Awaitable>&&)                 = delete;
    stored_awaiter& operator=(const stored_awaiter<Awaitable>&) = delete;
    stored_awaiter& operator=(stored_awaiter<Awaitable>&&)      = delete;

    Awaitable    awaitable;
    awaiter_type awaiter;
  };

  ~awaiter_memory_storage_t() { srsran_assert(empty(), "Emptying non-destroyed object"); }

  /// Get stored awaiter object
  template <typename Awaitable>
  stored_awaiter<Awaitable>* get_storage()
  {
    return storage.template get<stored_awaiter<Awaitable>>();
  }

  /// Storage Awaitable and respective awaiter
  template <typename Awaitable>
  void emplace(Awaitable&& a)
  {
    storage.template emplace<stored_awaiter<Awaitable>>(std::forward<Awaitable>(a));
  }

  /// Clear memory buffer
  template <typename Awaitable>
  void clear()
  {
    storage.template clear<stored_awaiter<Awaitable>>();
  }

  /// Get stored awaiter reference
  template <typename Awaitable>
  typename stored_awaiter<Awaitable>::awaiter_type& get_awaiter()
  {
    return get_storage<Awaitable>()->awaiter;
  }

  /// Get stored awaitable reference
  template <typename Awaitable>
  typename stored_awaiter<Awaitable>::awaitable_type& get_awaitable()
  {
    return get_storage<Awaitable>()->awaitable;
  }

  /// Checks whether awaiter is stored in memory buffer
  bool empty() const { return storage.empty(); }

private:
  small_memory_buffer_t<InlineBuffer, alignof(std::max_align_t)> storage;
};

} // namespace detail

} // namespace srsran
