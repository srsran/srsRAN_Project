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

/// \file
/// \brief Implementation of tiny_optional<T> class as a more memory-efficient alternative to optional<T>.

#include "srsran/adt/optional.h"

namespace srsran {

/// \brief Metafunction used to derive the specialization of tiny_optional<T> based on "T" and "T...".
template <typename T, T...>
struct tiny_optional_traits;

/// \brief Specialization for tiny_optional<T, AbsentValue>, where an instance of T equal to AbsentValue corresponds
/// to an empty optional.
template <typename T, T AbsentValue>
struct tiny_optional_traits<T, AbsentValue> {
  constexpr static T empty_value() noexcept { return AbsentValue; }
};

/// \brief Specialization for tiny_optional for unique_ptr<T>, where nullptr corresponds to an empty optional.
template <typename T>
struct tiny_optional_traits<std::unique_ptr<T>> {
  constexpr static std::unique_ptr<T> empty_value() noexcept { return nullptr; }
};

namespace detail {

/// \brief Base class for tiny_optional<T> when tiny_optional_traits<T, T...> is defined.
template <typename T, T... Args>
class base_tiny_optional
{
  using traits = tiny_optional_traits<T, Args...>;

  // SFINAE helpers.
  template <typename U>
  using is_self = std::is_same<base_tiny_optional<T, Args...>, std::remove_const_t<std::decay_t<U>>>;

public:
  constexpr base_tiny_optional() = default;
  constexpr base_tiny_optional(nullopt_t /**/) : base_tiny_optional() {}
  template <typename U = T, std::enable_if_t<not is_self<U>::value, int> = 0>
  constexpr base_tiny_optional(U&& u) : val(std::forward<U>(u))
  {
  }

  /// \brief Checks whether tiny_optional has value.
  constexpr bool     has_value() const noexcept { return val != traits::empty_value(); }
  constexpr explicit operator bool() const noexcept { return has_value(); }

  /// \brief Resets the tiny_optional to empty state.
  void reset() { val = traits::empty_value(); }

  constexpr T& value() & noexcept
  {
    srsran_assert(has_value(), "Invalid tiny_optional<T> access");
    return this->val;
  }
  constexpr T&& value() && noexcept
  {
    srsran_assert(has_value(), "Invalid tiny_optional<T> access");
    return std::move(this->val);
  }
  constexpr const T& value() const& noexcept
  {
    srsran_assert(has_value(), "Invalid tiny_optional<T> access");
    return this->val;
  }
  constexpr const T&& value() const&& noexcept
  {
    srsran_assert(has_value(), "Invalid optional<T> access");
    return std::move(this->val);
  }
  constexpr T*        operator->() noexcept { return &value(); }
  constexpr const T*  operator->() const noexcept { return &value(); }
  constexpr T&        operator*() & noexcept { return value(); }
  constexpr T&&       operator*() && noexcept { return value(); }
  constexpr const T&  operator*() const& noexcept { return value(); }
  constexpr const T&& operator*() const&& noexcept { return value(); }

  /// Constructs a new object of type T inside the optional<T>'s storage, and sets the optional state to not empty.
  template <typename... Args2>
  constexpr T& emplace(Args2&&... args) noexcept(std::is_nothrow_constructible<T, Args2...>::value)
  {
    this->val = T(std::forward<Args2>(args)...);
    return value();
  }

  constexpr bool operator==(const base_tiny_optional& rhs) const
  {
    return has_value() == rhs.has_value() and (has_value() ? val == rhs.val : true);
  }

  constexpr bool operator!=(const base_tiny_optional& rhs) const { return !(rhs == *this); }

private:
  T val = traits::empty_value();
};

/// Metafunction to check whether a certain T is complete.
template <typename T, size_t = sizeof(T)>
std::true_type  is_complete_impl(T*);
std::false_type is_complete_impl(...);
template <typename T>
using is_complete_type = decltype(is_complete_impl(std::declval<T*>()));

} // namespace detail

/// \brief Tiny optional class. When a specialization of tiny_optional_traits<T, Flags...> is provided, tiny_optional
/// will have detail::base_tiny_optional<T, Flags...> as its base class. Otherwise, optional<T> is used as base class.
template <typename T, T... Flags>
class tiny_optional : public std::conditional_t<detail::is_complete_type<tiny_optional_traits<T, Flags...>>::value,
                                                detail::base_tiny_optional<T, Flags...>,
                                                optional<T>>
{
  using base_type = typename std::conditional_t<detail::is_complete_type<tiny_optional_traits<T, Flags...>>::value,
                                                detail::base_tiny_optional<T, Flags...>,
                                                optional<T>>;

public:
  using value_type = T;
  using base_type::base_type;
};

} // namespace srsran
