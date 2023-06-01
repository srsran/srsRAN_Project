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

/// \file
/// \brief asn.1 utilities.
///
/// In this file, we implement several algorithms used to add/mod/release asn.1 objects in lists
/// based on their ids. This operations are commonly used during the application of Diffs/Deltas when the UE has
/// to be reconfigured.

#pragma once

#include "srsran/support/async/detail/function_signature.h"
#include "srsran/support/detail/is_iterable.h"
#include "srsran/support/srsran_assert.h"
#include <algorithm>

namespace srsran {

/// Creates an functor that applies operator< comparison between objects based on their id.
template <typename GetId>
struct obj_id_comparator {
  using id_type = detail::function_return_t<decltype(&std::decay_t<GetId>::operator())>;

  obj_id_comparator(GetId&& c_) : func(std::forward<GetId>(c_)) {}

  template <typename T, typename U>
  bool operator()(const T& lhs, const U& rhs) const
  {
    return get_id(lhs) < get_id(rhs);
  }

private:
  template <typename T>
  id_type get_id(const T& t) const
  {
    return func(t);
  }
  id_type get_id(const id_type& t) const { return t; }

  GetId func;
};

template <typename GetId>
obj_id_comparator<GetId> make_id_comparator(GetId&& g)
{
  return {std::forward<GetId>(g)};
}

/**
 * Apply toAddModList/toRemoveList changes
 * @param prev_list original list of rrc fields
 * @param add_diff_list added/modified elements
 * @param rm_diff_list removed elements
 * @param next_list resulting list. (Can be same as prev_list)
 */
template <typename AddModList, typename RemoveList, typename GetId>
void apply_addmodremlist_diff(const AddModList& prev_list,
                              const AddModList& add_diff_list,
                              const RemoveList& rm_diff_list,
                              AddModList&       next_list,
                              const GetId&      id_func)
{
  if (add_diff_list.size() == 0 and rm_diff_list.size() == 0) {
    if (&next_list != &prev_list) {
      next_list = prev_list;
    }
    return;
  }
  auto id_cmp_op = make_id_comparator(id_func);
  srsran_sanity_check(std::is_sorted(prev_list.begin(), prev_list.end(), id_cmp_op), "Expected sorted list");
  srsran_sanity_check(std::is_sorted(add_diff_list.begin(), add_diff_list.end(), id_cmp_op), "Expected sorted list");
  srsran_sanity_check(std::is_sorted(rm_diff_list.begin(), rm_diff_list.end()), "Expected sorted list");

  AddModList tmp_lst;
  // apply remove list
  std::set_difference(prev_list.begin(),
                      prev_list.end(),
                      rm_diff_list.begin(),
                      rm_diff_list.end(),
                      std::back_inserter(tmp_lst),
                      id_cmp_op);

  // apply toaddmodlist
  next_list.clear();
  std::set_union(add_diff_list.begin(),
                 add_diff_list.end(),
                 tmp_lst.begin(),
                 tmp_lst.end(),
                 std::back_inserter(next_list),
                 id_cmp_op);
}

template <typename AddModList, typename RemoveList, typename GetId>
void apply_addmodremlist_diff(AddModList&       src_and_dest_list,
                              const AddModList& add_diff_list,
                              const RemoveList& rm_diff_list,
                              const GetId&      id_func)
{
  apply_addmodremlist_diff(src_and_dest_list, add_diff_list, rm_diff_list, src_and_dest_list, id_func);
}

/// \brief Invokes rem_func, add_func and mod_func for any detected differences between prev_list and next_list.
/// \param rem_func Callable with signature void(const obj_type&), where obj_type is the value_type of \c List,
/// which gets called for every object whose id exists in \c prev_list but does not exist in \c next_list.
/// \param add_func Callable with signature void(const obj_type&), where obj_type is the value_type of \c List,
/// which gets called for every object whose id exists in \c next_list but does not exist in \c prev_list.
/// \param mod_func Callable with signature void(const obj_type&), where obj_type is the value_type of \c List,
/// which gets called for every object whose id exists in both \c next_list and \c prev_list, but its parameters are
/// different between the two lists.
/// \tparam List List of objects, where each object has an ID.
/// \tparam GetId Function to obtain ID of an object.
/// \param[in] prev_list Previous list of objects.
/// \param[in] next_list Next list of objects.
/// \param[in] id_func function to extract the ID from an object with type equal to the value_type of \c List.
template <typename List, typename RemFunctor, typename AddFunctor, typename ModFunctor, typename GetId>
std::enable_if_t<not is_iterable<AddFunctor>::value> calculate_addmodremlist_diff(const RemFunctor& rem_func,
                                                                                  const AddFunctor& add_func,
                                                                                  const ModFunctor& mod_func,
                                                                                  const List&       prev_list,
                                                                                  const List&       next_list,
                                                                                  const GetId&      id_func)
{
  auto id_cmp_op = make_id_comparator(id_func);
  srsran_sanity_check(std::is_sorted(prev_list.begin(), prev_list.end(), id_cmp_op), "Expected sorted list");
  srsran_sanity_check(std::is_sorted(next_list.begin(), next_list.end(), id_cmp_op), "Expected sorted list");

  auto src_it    = prev_list.begin();
  auto target_it = next_list.begin();

  bool src_left    = src_it != prev_list.end();
  bool target_left = target_it != next_list.end();
  while (src_left or target_left) {
    if (not target_left or (src_left and id_cmp_op(*src_it, *target_it))) {
      rem_func(src_it++);
    } else if (not src_left or (target_left and id_cmp_op(*target_it, *src_it))) {
      // a new object has been added to target
      add_func(target_it++);
    } else {
      mod_func(src_it++, target_it++);
    }
    src_left    = src_it != prev_list.end();
    target_left = target_it != next_list.end();
  }
}

/// \brief Generate toAddModList and toReleaseList based on differences between prev_list and next_list.
/// \tparam toAddModList List of objects added or modified.
/// \tparam RemoveList List of IDs for objects removed.
/// \tparam List List of objects, where each object has an ID.
/// \tparam ConvertElem Function to convert element of \c List to element of \c toAddModList.
/// \tparam GetId Function to obtain ID of an object.
/// \param[out] add_diff_list Difference in terms of added/modified objects between \c prev_list and \c next_list.
/// \param[out] rem_diff_list Difference in terms of removed objects between  \c prev_list and \c next_list.
/// \param[in] prev_list Previous list of objects.
/// \param[in] next_list Next list of objects.
/// \param[in] convert_func Callable used to convert an object with type \c List::value_type into an object with type
/// \c toAddModList::value_type.
/// \param[in] id_func function to extract the ID from an object with type equal to the value_type of \c List.
template <typename List, typename toAddModList, typename RemoveList, typename ConvertElem, typename GetId>
std::enable_if_t<is_iterable<toAddModList>::value> calculate_addmodremlist_diff(toAddModList&      add_diff_list,
                                                                                RemoveList&        rem_diff_list,
                                                                                const List&        prev_list,
                                                                                const List&        next_list,
                                                                                const ConvertElem& convert_func,
                                                                                const GetId&       id_func)
{
  if (&prev_list == &next_list) {
    // No difference because src and target are the same list. Early return.
    add_diff_list.clear();
    rem_diff_list.clear();
    return;
  }

  add_diff_list.clear();
  rem_diff_list.clear();

  using it_t    = typename List::const_iterator;
  auto rem_func = [&rem_diff_list, &id_func](it_t rem_it) { rem_diff_list.push_back(id_func(*rem_it)); };
  auto add_func = [&add_diff_list, &convert_func](it_t add_it) { add_diff_list.push_back(convert_func(*add_it)); };
  auto mod_func = [&add_diff_list, &convert_func](it_t src_it, it_t target_it) {
    if (not(*src_it == *target_it)) {
      add_diff_list.push_back(convert_func(*target_it));
    }
  };
  calculate_addmodremlist_diff(rem_func, add_func, mod_func, prev_list, next_list, id_func);
}

/// \brief Generate toAddModList and toReleaseList based on differences between prev_list and next_list.
/// \tparam toAddModList List of objects added or modified.
/// \tparam RemoveList List of IDs for objects removed.
/// \tparam GetId Function to obtain ID of an object.
/// \param[out] add_diff_list Difference in terms of added/modified objects between \c prev_list and \c next_list.
/// \param[out] rem_diff_list Difference in terms of removed objects between  \c prev_list and \c next_list.
/// \param[in] prev_list Previous list of objects.
/// \param[in] next_list Next list of objects.
/// \param[in] id_func function to extract the ID from an object with type equal to the value_type of \c List.
template <typename toAddModList, typename RemoveList, typename GetId>
void calculate_addmodremlist_diff(toAddModList&       add_diff_list,
                                  RemoveList&         rem_diff_list,
                                  const toAddModList& prev_list,
                                  const toAddModList& next_list,
                                  const GetId&        id_func)
{
  using elem_type = decltype(*prev_list.begin());

  if (&prev_list == &add_diff_list) {
    // use const prev_list instead.
    toAddModList prev_list2 = prev_list;
    calculate_addmodremlist_diff(add_diff_list, rem_diff_list, prev_list2, next_list, id_func);
    return;
  }

  if (&next_list == &add_diff_list) {
    // use const next_list
    toAddModList next_list2 = next_list;
    calculate_addmodremlist_diff(add_diff_list, rem_diff_list, prev_list, next_list2, id_func);
    return;
  }

  calculate_addmodremlist_diff(
      add_diff_list, rem_diff_list, prev_list, next_list, [](const elem_type& e) { return e; }, id_func);
}

/// \brief Sets up the setup/release ASN.1 object based on the difference between prev and next.
///
/// \param setup_rel Setup/release ASN.1 object.
/// \param prev Previous optional value.
/// \param next New optional value.
/// \tparam ConvertElem Function to convert element of \c OptionalElem to element of \c Asn1Type.
/// \return Whether changes were made to Setup/Release ASN.1 object.
template <typename Asn1Type, typename OptionalElem, typename ConvertElem>
bool calculate_setup_release(asn1::setup_release_c<Asn1Type>& setup_rel,
                             const optional<OptionalElem>&    prev,
                             const optional<OptionalElem>&    next,
                             const ConvertElem&               convert_func)
{
  if (next.has_value()) {
    if (not prev.has_value() or *prev != *next) {
      setup_rel.set_setup() = convert_func(*next);
    }
  } else if (prev.has_value()) {
    setup_rel.set_release();
  } else {
    setup_rel.set(asn1::setup_release_opts::nulltype);
  }
  return setup_rel.type().value != asn1::setup_release_opts::nulltype;
}

} // namespace srsran
