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
