/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_OPTIONAL_H
#define SRSGNB_OPTIONAL_H

#include "detail/type_storage.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

/// Wrapper class that represents an optional field of type T. In case has_value()==true, the object of type T has
/// been constructed in the embedded buffer. The class handles the object safe construction, destruction, move, copy
/// \tparam T type of stored object
template <typename T>
class optional
{
public:
  using value_type = T;

  optional() noexcept : has_val_(false), empty() {}
  optional(const T& t) noexcept(std::is_nothrow_copy_constructible<T>::value) : has_val_(true) { storage.emplace(t); }
  optional(T&& t) noexcept(std::is_nothrow_move_constructible<T>::value) : has_val_(true)
  {
    storage.emplace(std::move(t));
  }
  optional(const optional<T>& other) noexcept(std::is_nothrow_copy_constructible<T>::value) :
    has_val_(other.has_value())
  {
    if (other.has_value()) {
      storage.copy_ctor(other.storage);
    }
  }
  optional(optional<T>&& other) noexcept(std::is_nothrow_move_constructible<T>::value) : has_val_(other.has_value())
  {
    if (other.has_value()) {
      storage.template emplace(std::move(other.storage.get()));
    }
  }
  optional& operator=(const optional<T>& other) noexcept(std::is_nothrow_copy_assignable<T>::value)
  {
    if (this == &other) {
      return *this;
    }
    copy_if_present_helper(storage, other.storage, has_value(), other.has_value());
    has_val_ = other.has_value();
    return *this;
  }
  optional& operator=(optional<T>&& other) noexcept(std::is_nothrow_move_assignable<T>::value)
  {
    move_if_present_helper(storage, other.storage, has_value(), other.has_value());
    has_val_ = other.has_value();
    return *this;
  }
  ~optional()
  {
    if (has_value()) {
      storage.destroy();
    }
  }

  bool     has_value() const noexcept { return has_val_; }
  explicit operator bool() const noexcept { return has_value(); }

  T*       operator->() noexcept { return &value(); }
  const T* operator->() const noexcept { return &value(); }
  T&       operator*() noexcept { return value(); }
  const T& operator*() const noexcept { return value(); }
  T&       value() noexcept
  {
    srsran_assert(has_val_, "Invalid optional<T> access");
    return storage.get();
  }
  const T& value() const noexcept
  {
    srsran_assert(has_val_, "Invalid optional<T> access");
    return storage.get();
  }

  template <typename... Args>
  void emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value)
  {
    static_assert(std::is_constructible<T, Args&&...>::value, "Ctor T(Args...) does not exist.");
    if (has_value()) {
      storage.destroy();
    }
    storage.emplace(std::forward<Args>(args)...);
    has_val_ = true;
  }

  void reset() noexcept
  {
    if (has_value()) {
      storage.destroy();
      has_val_ = false;
    }
  }

private:
  bool has_val_;
  union {
    char                    empty;
    detail::type_storage<T> storage;
  };
};

template <typename T>
bool operator==(const optional<T>& lhs, const optional<T>& rhs) noexcept
{
  return lhs.has_value() == rhs.has_value() and (not lhs.has_value() or lhs.value() == rhs.value());
}

template <typename T>
bool operator==(const optional<T>& lhs, const T& rhs) noexcept
{
  return lhs.has_value() and lhs.value() == rhs;
}

template <typename T>
bool operator!=(const optional<T>& lhs, const optional<T>& rhs) noexcept
{
  return not(lhs == rhs);
}

template <typename T>
bool operator!=(const optional<T>& lhs, const T& rhs) noexcept
{
  return not(lhs == rhs);
}

template <typename T>
bool operator<(const optional<T>& lhs, const optional<T>& rhs) noexcept
{
  return rhs.has_value() and ((lhs.has_value() and lhs.value() < rhs.value()) or (not lhs.has_value()));
}

} // namespace srsgnb

#endif // SRSGNB_OPTIONAL_H
