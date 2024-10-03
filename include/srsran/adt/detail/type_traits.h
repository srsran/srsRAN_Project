/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <array>
#include <type_traits>

namespace srsran {
namespace detail {

template <typename U>
struct is_std_array : std::false_type {
};
template <typename U, std::size_t N>
struct is_std_array<std::array<U, N>> : std::true_type {
};

} // namespace detail
} // namespace srsran
