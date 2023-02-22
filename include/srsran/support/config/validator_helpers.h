/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

namespace srsran {

/// \brief Check if all elements of a range have a unique ID, which is provided via \c id_get.
/// \param[in] Range with begin() and end().
/// \param[in] id_get Callback with signature "id_type(const Range::value_type&)" that returns back an element
/// identifier.
/// \return True if all elements in the range have a unique identifier.
template <typename Range, typename IdentifierGetter>
bool has_unique_ids(const Range& r, const IdentifierGetter& id_get)
{
  for (auto it = r.begin(); it != r.end(); ++it) {
    auto it2 = it;
    ++it2;
    for (; it2 != r.end(); ++it2) {
      if (id_get(*it) == id_get(*it2)) {
        return false;
      }
    }
  }
  return true;
}

/// \brief Check if all elements of a range have a unique ID, where the ID can be accessed via the provided field
/// pointer.
/// \param[in] Range with begin() and end().
/// \param[in] id_field Member pointer used to get ID of elements of provided \c Range.
/// \return True if all elements in the range have a unique identifier.
template <typename Range, typename ValueType = typename Range::value_type, typename IdType>
bool has_unique_ids(const Range& r, IdType ValueType::*id_field)
{
  return has_unique_ids(r, [id_field](const auto& e) { return e.*id_field; });
}

} // namespace srsran