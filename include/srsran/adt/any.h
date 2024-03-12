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

#define ANY_IMPL_NO_EXCEPTIONS
#include "any/any.hpp"

namespace srsran {

using bad_any_cast = linb::bad_any_cast;

using any = linb::any;

template <typename ValueType>
inline ValueType any_cast(const any& operand)
{
  return linb::any_cast<ValueType>(operand);
}

template <typename ValueType>
inline ValueType any_cast(any& operand)
{
  return linb::any_cast<ValueType>(operand);
}

template <typename ValueType>
inline ValueType any_cast(any&& operand)
{
  return linb::any_cast<ValueType>(std::move(operand));
}

template <typename ValueType>
inline const ValueType* any_cast(const any* operand) noexcept
{
  return linb::any_cast<ValueType>(operand);
}

template <typename ValueType>
inline ValueType* any_cast(any* operand) noexcept
{
  return linb::any_cast<ValueType>(operand);
}

} // namespace srsran
