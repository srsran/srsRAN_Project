/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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