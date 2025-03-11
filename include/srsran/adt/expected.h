/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "TartanLlama/expected.hpp"

namespace srsran {

struct default_error_t {};
struct default_success_t {};

template <typename E>
using unexpected = tl::unexpected<E>;

template <typename E>
unexpected<std::decay_t<E>> make_unexpected(E&& e)
{
  return unexpected<std::decay_t<E>>(std::forward<E>(e));
}

template <typename T = default_success_t, typename E = default_error_t>
using expected = tl::expected<T, E>;

namespace detail {
template <typename T>
struct is_expected : std::false_type {};
template <typename V, typename E>
struct is_expected<expected<V, E>> : std::true_type {};
} // namespace detail

template <typename E>
using error_type = expected<default_success_t, E>;

} // namespace srsran
