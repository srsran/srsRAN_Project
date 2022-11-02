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

/// Tag to disambiguate optional ctor overloads. Introduced only in C++17.
struct in_place_t {};

/// Empty class type to represent empty optional objects.
struct nullopt_t {
  explicit constexpr nullopt_t(int /**/) {}
};

/// Initializer for empty optional objects.
constexpr nullopt_t nullopt{0};

namespace detail {

/// Storage of an optional internal value and empty/has_value flag.
template <typename T>
struct base_optional_storage {
  using storageT = std::remove_const_t<T>;

  // Storage for trivially destructible type.
  template <typename U, bool = std::is_trivially_destructible<U>::value>
  union storage_type {
    constexpr storage_type() noexcept : empty() {}
    template <typename... Args>
    explicit storage_type(in_place_t, Args&&... args) : val(std::forward<Args>(args)...)
    {
    }

    std::true_type empty;
    U              val;
  };

  // Storage for non-trivially destructible type.
  template <typename U>
  union storage_type<U, false> {
    constexpr storage_type() noexcept : empty() {}
    template <typename... Args>
    explicit storage_type(in_place_t, Args&&... args) : val(std::forward<Args>(args)...)
    {
    }

    // User-defined destructor in case T is not trivially destructible.
    ~storage_type() {}

    std::true_type empty;
    U              val;
  };

  constexpr base_optional_storage() noexcept = default;
  template <typename... Args>
  explicit base_optional_storage(in_place_t tag, Args&&... args) :
    payload(tag, std::forward<Args>(args)...), has_val(true)
  {
  }
  explicit base_optional_storage(bool has_val_, const storage_type<storageT>& payload) noexcept(
      std::is_nothrow_copy_constructible<T>::value) :
    has_val(has_val_)
  {
    if (has_val_) {
      construct_(payload.val);
    }
  }
  explicit base_optional_storage(bool has_val_, storage_type<storageT>&& payload) noexcept(
      std::is_nothrow_move_constructible<T>::value) :
    has_val(has_val_)
  {
    if (has_val_) {
      construct_(std::move(payload.val));
    }
  }
  base_optional_storage(const base_optional_storage<T>&)                = default;
  base_optional_storage(base_optional_storage<T>&&) noexcept            = default;
  base_optional_storage& operator=(const base_optional_storage<T>&)     = default;
  base_optional_storage& operator=(base_optional_storage<T>&&) noexcept = default;

  template <typename... Args>
  constexpr void construct_(Args&&... args) noexcept(std::is_nothrow_constructible<storageT, Args...>::value)
  {
    new (&payload.val) storageT(std::forward<Args>(args)...);
    has_val = true;
  }

  constexpr void destroy_() noexcept
  {
    has_val = false;
    payload.val.~storageT();
  }

  constexpr void reset_() noexcept
  {
    if (has_val) {
      destroy_();
    }
  }

  constexpr void copy_assign_(const base_optional_storage<T>& other)
  {
    if (this == &other) {
      return;
    }
    if (other.has_val and has_val) {
      this->payload.val = other.payload.val;
    } else {
      if (other.has_val) {
        this->construct_(other.payload.val);
      } else {
        this->reset_();
      }
    }
  }

  constexpr void move_assign_(base_optional_storage<T>&& other) noexcept(
      std::is_nothrow_move_constructible<T>::value and std::is_nothrow_move_assignable<T>::value)
  {
    if (other.has_val and has_val) {
      this->payload.val = std::move(other.payload.val);
    } else {
      if (other.has_val) {
        this->construct_(std::move(other.payload.val));
      } else {
        this->reset_();
      }
    }
  }

  storage_type<storageT> payload;
  bool                   has_val = false;
};

/// Specialization of optional<T> storage based on triviality traits of dtor/move/copy.
template <typename T,
          bool IsTriviallyDestructible = std::is_trivially_destructible<T>::value,
          bool isTriviallyCopyable =
              std::is_trivially_copy_constructible<T>::value and std::is_trivially_copy_assignable<T>::value,
          bool                                                   isTriviallyMovable =
              std::is_trivially_move_constructible<T>::value and std::is_trivially_move_assignable<T>::value>
class optional_storage;

// Specialization for trivial dtor/copy/move.
template <typename T>
class optional_storage<T, true, true, true> : public base_optional_storage<T>
{
public:
  using base_optional_storage<T>::base_optional_storage;
};

// Specialization for non-trivial copy.
template <typename T>
class optional_storage<T, true, false, true> : public base_optional_storage<T>
{
public:
  using base_optional_storage<T>::base_optional_storage;

  optional_storage()  = default;
  ~optional_storage() = default;
  optional_storage(const optional_storage& other) noexcept(std::is_nothrow_copy_constructible<T>::value) :
    base_optional_storage<T>(other.has_val, other.payload)
  {
  }
  optional_storage(optional_storage&&) noexcept            = default;
  optional_storage& operator=(optional_storage&&) noexcept = default;

  optional_storage& operator=(const optional_storage& other)
  {
    this->copy_assign_(other);
    return *this;
  }
};

// Specialization for non-trivial move.
template <typename T>
class optional_storage<T, true, true, false> : public base_optional_storage<T>
{
public:
  using base_optional_storage<T>::base_optional_storage;

  optional_storage()                        = default;
  ~optional_storage()                       = default;
  optional_storage(const optional_storage&) = default;
  optional_storage(optional_storage&& other) noexcept(std::is_nothrow_move_constructible<T>::value) :
    base_optional_storage<T>(other.has_val, std::move(other.payload))
  {
  }
  optional_storage& operator=(const optional_storage&) = default;

  constexpr optional_storage& operator=(optional_storage&& other) noexcept(
      std::is_nothrow_move_constructible<T>::value and std::is_nothrow_move_assignable<T>::value)
  {
    this->move_assign_(std::move(other));
    return *this;
  }
};

// Specialization for non-trivial move and copy.
template <typename T>
class optional_storage<T, true, false, false> : public base_optional_storage<T>
{
public:
  using base_optional_storage<T>::base_optional_storage;

  optional_storage()  = default;
  ~optional_storage() = default;
  optional_storage(const optional_storage& other) noexcept(std::is_nothrow_copy_constructible<T>::value) :
    base_optional_storage<T>(other.has_val, other.payload)
  {
  }
  optional_storage(optional_storage&& other) noexcept(std::is_nothrow_move_constructible<T>::value) :
    base_optional_storage<T>(other.has_val, std::move(other.payload))
  {
  }

  constexpr optional_storage& operator=(const optional_storage& other)
  {
    this->copy_assign_(other);
    return *this;
  }

  constexpr optional_storage& operator=(optional_storage&& other) noexcept(
      std::is_nothrow_move_constructible<T>::value and std::is_nothrow_move_assignable<T>::value)
  {
    this->move_assign_(std::move(other));
    return *this;
  }
};

// Specialization for non-trivial dtor.
template <typename T, bool TrivialCopy, bool TrivialMove>
class optional_storage<T, false, TrivialCopy, TrivialMove> : public optional_storage<T, true, false, false>
{
public:
  using optional_storage<T, true, false, false>::optional_storage;
  optional_storage()                                                                                  = default;
  optional_storage(const optional_storage&)                                                           = default;
  optional_storage(optional_storage&&) noexcept(std::is_nothrow_move_constructible<T>::value)         = default;
  optional_storage& operator=(const optional_storage&)                                                = default;
  optional_storage& operator=(optional_storage&&) noexcept(std::is_nothrow_move_assignable<T>::value) = default;

  ~optional_storage() { this->reset_(); }
};

} // namespace detail

/// \brief Optional objects.
///
/// An instance of this class may either contain a value of type \c T (and <tt>has_value() == true</tt>) or be empty
/// (and <tt>has_value() == false</tt>).
///
/// The class handles the safe construction, destruction, move and copy of the contained object.
/// \tparam T Type of stored object.
template <typename T>
class optional
{
  // SFINAE helpers.
  template <typename U>
  using is_self = std::is_same<optional<T>, std::remove_const_t<std::decay_t<U>>>;

public:
  using value_type = T;

  constexpr optional() = default;
  constexpr optional(nullopt_t /**/) : optional() {}
  template <typename U = T, std::enable_if_t<not is_self<U>::value, int> = 0>
  constexpr optional(U&& u) : storage(in_place_t{}, std::forward<U>(u))
  {
  }

  /// Checks the state of the optional.
  /// return true if the optional is not empty. False otherwise.
  constexpr bool     has_value() const noexcept { return storage.has_val; }
  constexpr explicit operator bool() const noexcept { return has_value(); }

  constexpr T& value() & noexcept
  {
    srsgnb_assert(has_value(), "Invalid optional<T> access");
    return storage.payload.val;
  }
  constexpr T&& value() && noexcept
  {
    srsgnb_assert(has_value(), "Invalid optional<T> access");
    return std::move(storage.payload.val);
  }
  constexpr const T& value() const& noexcept
  {
    srsgnb_assert(has_value(), "Invalid optional<T> access");
    return storage.payload.val;
  }
  constexpr const T&& value() const&& noexcept
  {
    srsgnb_assert(has_value(), "Invalid optional<T> access");
    return std::move(storage.payload.val);
  }
  constexpr T*        operator->() noexcept { return &value(); }
  constexpr const T*  operator->() const noexcept { return &value(); }
  constexpr T&        operator*() & noexcept { return value(); }
  constexpr T&&       operator*() && noexcept { return value(); }
  constexpr const T&  operator*() const& noexcept { return value(); }
  constexpr const T&& operator*() const&& noexcept { return value(); }

  optional& operator=(nullopt_t /**/) noexcept
  {
    reset();
    return *this;
  }

  /// Constructs a new object of type T inside the optional<T>'s storage, and sets the optional state to not empty.
  template <typename... Args>
  constexpr T& emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
  {
    storage.reset_();
    storage.construct_(std::forward<Args>(args)...);
    return value();
  }

  /// Resets the internal value stored in optional<T> and sets the optional state to empty.
  constexpr void reset() noexcept { storage.reset_(); }

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
bool operator==(const T& lhs, const optional<T>& rhs) noexcept
{
  return rhs.has_value() and lhs == rhs.value();
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
bool operator!=(const T& lhs, const optional<T>& rhs) noexcept
{
  return not(lhs == rhs);
}

template <typename T>
bool operator<(const optional<T>& lhs, const optional<T>& rhs) noexcept
{
  return rhs.has_value() and ((lhs.has_value() and lhs.value() < rhs.value()) or (not lhs.has_value()));
}

} // namespace srsgnb
