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

#include "srsgnb/adt/optional.h"

namespace srsgnb {

namespace detail {

template <typename T, T...>
class tiny_optional_container;

template <typename T>
class tiny_optional_container<std::unique_ptr<T>>
{
  // SFINAE helpers.
  template <typename U>
  using is_self = std::is_same<tiny_optional_container<std::unique_ptr<T>>, std::remove_const_t<std::decay_t<U>>>;

public:
  constexpr tiny_optional_container() = default;
  template <typename U = T, std::enable_if_t<not is_self<U>::value, int> = 0>
  constexpr tiny_optional_container(U&& u) : val(std::forward<U>(u))
  {
  }
  void           reset() { val = nullptr; }
  constexpr bool has_value() const noexcept { return val != nullptr; }

protected:
  std::unique_ptr<T> val;
};

template <typename T, T AbsentValue>
class tiny_optional_container<T, AbsentValue>
{
  // SFINAE helpers.
  template <typename U>
  using is_self = std::is_same<tiny_optional_container<T, AbsentValue>, std::remove_const_t<std::decay_t<U>>>;

public:
  constexpr tiny_optional_container() = default;
  template <typename U = T, std::enable_if_t<not is_self<U>::value, int> = 0>
  constexpr tiny_optional_container(U&& u) : val(std::forward<U>(u))
  {
  }
  void           reset() { val = AbsentValue; }
  constexpr bool has_value() const noexcept { return val != AbsentValue; }

protected:
  T val = AbsentValue;
};

template <typename T, T... Args>
class base_tiny_optional : public tiny_optional_container<T, Args...>
{
  using base_type = tiny_optional_container<T, Args...>;

  // SFINAE helpers.
  template <typename U>
  using is_self = std::is_same<base_tiny_optional<T, Args...>, std::remove_const_t<std::decay_t<U>>>;

public:
  constexpr base_tiny_optional() = default;
  constexpr base_tiny_optional(nullopt_t /**/) : base_tiny_optional() {}
  template <typename U = T, std::enable_if_t<not is_self<U>::value, int> = 0>
  constexpr base_tiny_optional(U&& u) : base_type(std::forward<U>(u))
  {
  }

  using base_type::has_value;
  using base_type::reset;

  constexpr explicit operator bool() const noexcept { return has_value(); }

  constexpr T& value() & noexcept
  {
    srsgnb_assert(has_value(), "Invalid tiny_optional<T> access");
    return this->val;
  }
  constexpr T&& value() && noexcept
  {
    srsgnb_assert(has_value(), "Invalid tiny_optional<T> access");
    return std::move(this->val);
  }
  constexpr const T& value() const& noexcept
  {
    srsgnb_assert(has_value(), "Invalid tiny_optional<T> access");
    return this->val;
  }
  constexpr const T&& value() const&& noexcept
  {
    srsgnb_assert(has_value(), "Invalid optional<T> access");
    return std::move(this->val);
  }
  constexpr T*        operator->() noexcept { return &value(); }
  constexpr const T*  operator->() const noexcept { return &value(); }
  constexpr T&        operator*() & noexcept { return value(); }
  constexpr T&&       operator*() && noexcept { return value(); }
  constexpr const T&  operator*() const& noexcept { return value(); }
  constexpr const T&& operator*() const&& noexcept { return value(); }
};

} // namespace detail

template <typename T, T...>
class tiny_optional;

/// \brief Specialization when no AbsentValue is defined.
template <typename T>
class tiny_optional<T> : public optional<T>
{
public:
  using value_type = typename optional<T>::value_type;
  using optional<T>::optional;
};

/// \brief Specialization of tiny_optional<T> where a value==\c AbsentValue is defined as equivalent to the object
/// having no value set.
template <typename T, T AbsentValue>
class tiny_optional<T, AbsentValue> : public detail::base_tiny_optional<T, AbsentValue>
{
public:
  using value_type = T;
  using detail::base_tiny_optional<T, AbsentValue>::base_tiny_optional;
};

/// \brief Specialization of tiny_optional<T> when T is a unique_ptr. In this case, we use the value nullptr to
/// mark the absence of a value.
template <typename T>
class tiny_optional<std::unique_ptr<T>> : public detail::base_tiny_optional<std::unique_ptr<T>>
{
public:
  using value_type = std::unique_ptr<T>;
  using detail::base_tiny_optional<std::unique_ptr<T>>::base_tiny_optional;
};

} // namespace srsgnb