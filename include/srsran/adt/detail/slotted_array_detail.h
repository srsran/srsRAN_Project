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

#include "srsran/adt/span.h"

namespace srsran {

namespace detail {

template <typename IdType>
struct cast_to_size_operator {
  constexpr std::size_t get_index(const IdType& t) const { return static_cast<std::size_t>(t); }
  constexpr IdType      get_id(std::size_t idx) const { return static_cast<IdType>(idx); }
};

} // namespace detail

} // namespace srsran
