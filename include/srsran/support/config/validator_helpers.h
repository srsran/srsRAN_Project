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

#include "srsran/support/config/validator_result.h"
#include <algorithm>
#include <optional>

namespace srsran {

/// Macro used to check a condition and propagate an error message if the validation fails.
#define VERIFY(cond, ...)                                                                                              \
  if (not(cond)) {                                                                                                     \
    return make_unexpected(fmt::format(__VA_ARGS__));                                                                  \
  }

/// Macro used to check a validation result and propagate the error in case of failure.
#define HANDLE_ERROR(cond)                                                                                             \
  {                                                                                                                    \
    auto ret = cond;                                                                                                   \
    if (not ret.has_value()) {                                                                                         \
      return ret;                                                                                                      \
    }                                                                                                                  \
  }

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

/// \brief Check if all elements of a range are unique.
template <typename Range>
bool are_all_unique(const Range& r)
{
  return has_unique_ids(
      r, [](const auto& e) -> const auto& { return e; });
}

/// \brief Find an ID in \c id_set_list of \c set that is not present in range \c r.
///
/// \param set List of sets of resources.
/// \param r List of resources.
/// \param id_set_list Member pointer to the list of resources belonging to a set.
/// \param idfield Member pointer to a resource ID.
/// \return An resource Id belonging to a set which was not found in the list of resources \c r. If all resource Ids
/// in the set are present in \c r, then an empty optional is returned.
template <typename RangeSet,
          typename Range,
          typename SetValueType = typename RangeSet::value_type,
          typename ValueType    = typename Range::value_type,
          typename IdType,
          typename IdList>
std::optional<IdType>
find_disconnected_id(const RangeSet& set, const Range& r, IdList SetValueType::*id_set_list, IdType ValueType::*idfield)
{
  for (const auto& es : set) {
    for (const auto& id : es.*id_set_list) {
      if (std::none_of(r.begin(), r.end(), [&](const auto& er) { return er.*idfield == id; })) {
        return id;
      }
    }
  }
  return {};
}

} // namespace srsran
