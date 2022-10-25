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

#include "srsgnb/support/srsgnb_assert.h"
#include <type_traits>

namespace srsgnb {

namespace detail {

/// Tag to disambiguate optional ctor overloads.
struct in_place_t {};

template <typename T, bool = std::is_trivially_destructible<T>::value>
class base_optional_storage;

/// Specialization of optional storage for non-trivially destructible types.
template <typename T>
class base_optional_storage<T, false>
{
public:
  constexpr base_optional_storage() noexcept : dummy(), has_val(false) {}
  template <typename... Args>
  explicit base_optional_storage(in_place_t, Args&&... args) : val(std::forward<Args>(args)...), has_val(true)
  {
  }
  base_optional_storage(const base_optional_storage& other) noexcept(std::is_nothrow_copy_constructible<T>::value) :
    has_val(other.has_val)
  {
    if (has_val) {
      new (&val) T(other.val);
    }
  }
  base_optional_storage(base_optional_storage&& other) noexcept(std::is_nothrow_move_constructible<T>::value) :
    has_val(other.has_val)
  {
    if (has_val) {
      new (&val) T(std::move(other.val));
    }
  }
  ~base_optional_storage() { reset(); }

  template <typename... Args>
  void set(Args&&... u) noexcept(std::is_nothrow_constructible<T, Args&&...>::value)
  {
    static_assert(std::is_constructible<T, Args&&...>::value, "Ctor T(Args&&...) does not exist.");
    if (has_val) {
      val = T(std::forward<Args>(u)...);
    } else {
      new (&val) T(std::forward<Args>(u)...);
      has_val = true;
    }
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    static_assert(std::is_constructible<T, Args&&...>::value, "Ctor T(Args&&...) does not exist.");
    if (has_val) {
      val.~T();
    } else {
      has_val = true;
    }
    new (&val) T(std::forward<Args>(args)...);
  }

  void reset()
  {
    if (has_val) {
      val.~T();
      has_val = false;
    }
  }

  union {
    std::true_type dummy;
    T              val;
  };
  bool has_val;
};

/// Specialization of optional storage for trivially destructible types.
template <typename T>
class base_optional_storage<T, true>
{
public:
  constexpr base_optional_storage() noexcept : dummy(), has_val(false) {}
  template <typename... Args>
  explicit base_optional_storage(in_place_t, Args&&... args) : val(std::forward<Args>(args)...), has_val(true)
  {
  }

  template <typename... Args>
  void set(Args&&... args)
  {
    static_assert(std::is_constructible<T, Args&&...>::value, "Ctor T(Args...) does not exist.");
    if (has_val) {
      val = T(std::forward<Args>(args)...);
    } else {
      new (&val) T(std::forward<Args>(args)...);
      has_val = true;
    }
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    static_assert(std::is_constructible<T, Args&&...>::value, "Ctor T(Args&&...) does not exist.");
    if (has_val) {
      val.~T();
    } else {
      has_val = true;
    }
    new (&val) T(std::forward<Args>(args)...);
  }

  void reset() { has_val = false; }

  union {
    std::true_type dummy;
    T              val;
  };
  bool has_val;
};

template <typename T,
          bool = std::is_trivially_copy_constructible<T>::value and std::is_trivially_copy_assignable<T>::value and
              std::is_trivially_move_constructible<T>::value and std::is_trivially_move_assignable<T>::value>
class optional_storage;

template <typename T>
class optional_storage<T, true> : public base_optional_storage<T>
{
public:
  using base_optional_storage<T>::base_optional_storage;
};

template <typename T>
class optional_storage<T, false> : public base_optional_storage<T>
{
public:
  using base_optional_storage<T>::base_optional_storage;

  optional_storage(const optional_storage&) noexcept(std::is_nothrow_copy_constructible<T>::value) = default;
  optional_storage(optional_storage&&) noexcept(std::is_nothrow_move_constructible<T>::value)      = default;

  optional_storage& operator=(const optional_storage<T>& other) noexcept(std::is_nothrow_copy_assignable<T>::value)
  {
    if (this == &other) {
      return *this;
    }
    if (other.has_val) {
      this->set(other.val);
    } else {
      this->reset();
    }
    return *this;
  }
  optional_storage& operator=(optional_storage<T>&& other) noexcept(std::is_nothrow_move_assignable<T>::value)
  {
    if (other.has_val) {
      this->set(std::move(other.val));
    } else {
      this->reset();
    }
    return *this;
  }
};

} // namespace detail

/// Wrapper class that represents an optional field of type T. In case has_value()==true, the object of type T has
/// been constructed in the embedded buffer. The class handles the object safe construction, destruction, move, copy
/// \tparam T type of stored object
template <typename T>
class optional
{
  // SFINAE helpers.
  template <typename U>
  using is_self = std::is_same<optional<T>, std::remove_const_t<std::decay_t<U>>>;

public:
  using value_type = T;

  optional() = default;
  template <typename U = T, std::enable_if_t<not is_self<U>::value, int> = 0>
  optional(U&& u) : storage(detail::in_place_t{}, std::forward<U>(u))
  {
  }

  bool     has_value() const noexcept { return storage.has_val; }
  explicit operator bool() const noexcept { return has_value(); }

  T& value() noexcept
  {
    srsgnb_assert(has_value(), "Invalid optional<T> access");
    return storage.val;
  }
  const T& value() const noexcept
  {
    srsgnb_assert(has_value(), "Invalid optional<T> access");
    return storage.val;
  }
  T*       operator->() noexcept { return &value(); }
  const T* operator->() const noexcept { return &value(); }
  T&       operator*() noexcept { return value(); }
  const T& operator*() const noexcept { return value(); }

  template <typename... Args>
  void emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value)
  {
    storage.emplace(std::forward<Args>(args)...);
  }

  void reset() noexcept { storage.reset(); }

private:
  detail::optional_storage<T> storage;
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
