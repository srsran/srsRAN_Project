/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/async/detail/function_signature.h"
#include "srsgnb/support/detail/is_iterable.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <algorithm>

/// \brief In this file, we implement several algorithms used to add/mod/release asn.1 objects in lists
/// based on their ids. This operations are commonly used during the application of Diffs/Deltas when the UE has
/// to be reconfigured.

namespace srsgnb {

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
 * @param src_list original list of rrc fields
 * @param add_diff_list added/modified elements
 * @param rm_diff_list removed elements
 * @param target_list resulting list. (Can be same as src_list)
 */
template <typename AddModList, typename RemoveList, typename GetId>
void apply_addmodremlist_diff(const AddModList& src_list,
                              const AddModList& add_diff_list,
                              const RemoveList& rm_diff_list,
                              AddModList&       target_list,
                              const GetId&      id_func)
{
  if (add_diff_list.size() == 0 and rm_diff_list.size() == 0) {
    if (&target_list != &src_list) {
      target_list = src_list;
    }
    return;
  }
  auto id_cmp_op = make_id_comparator(id_func);
  srsgnb_sanity_check(std::is_sorted(src_list.begin(), src_list.end(), id_cmp_op), "Expected sorted list");
  srsgnb_sanity_check(std::is_sorted(add_diff_list.begin(), add_diff_list.end(), id_cmp_op), "Expected sorted list");
  srsgnb_sanity_check(std::is_sorted(rm_diff_list.begin(), rm_diff_list.end()), "Expected sorted list");

  AddModList tmp_lst;
  // apply remove list
  std::set_difference(src_list.begin(),
                      src_list.end(),
                      rm_diff_list.begin(),
                      rm_diff_list.end(),
                      std::back_inserter(tmp_lst),
                      id_cmp_op);

  // apply toaddmodlist
  target_list.clear();
  std::set_union(add_diff_list.begin(),
                 add_diff_list.end(),
                 tmp_lst.begin(),
                 tmp_lst.end(),
                 std::back_inserter(target_list),
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

/// \brief Invoke rem_func, add_func and mod_func for any detected difference between src_list and target_list.
template <typename List, typename RemFunctor, typename AddFunctor, typename ModFunctor, typename GetId>
std::enable_if_t<not is_iterable<AddFunctor>::value> calculate_addmodremlist_diff(const List&  src_list,
                                                                                  const List&  target_list,
                                                                                  RemFunctor   rem_func,
                                                                                  AddFunctor   add_func,
                                                                                  ModFunctor   mod_func,
                                                                                  const GetId& id_func)
{
  auto id_cmp_op = make_id_comparator(id_func);
  srsgnb_sanity_check(std::is_sorted(src_list.begin(), src_list.end(), id_cmp_op), "Expected sorted list");
  srsgnb_sanity_check(std::is_sorted(target_list.begin(), target_list.end(), id_cmp_op), "Expected sorted list");

  auto src_it    = src_list.begin();
  auto target_it = target_list.begin();

  bool src_left    = src_it != src_list.end();
  bool target_left = target_it != target_list.end();
  while (src_left or target_left) {
    if (not target_left or (src_left and id_cmp_op(*src_it, *target_it))) {
      rem_func(src_it++);
    } else if (not src_left or (target_left and id_cmp_op(*target_it, *src_it))) {
      // a new object has been added to target
      add_func(target_it++);
    } else {
      mod_func(src_it++, target_it++);
    }
    src_left    = src_it != src_list.end();
    target_left = target_it != target_list.end();
  }
}

/// \brief Generate toAddModList and toReleaseList based on differences between src_list and target_list.
template <typename List, typename toAddModList, typename RemoveList, typename ConvertElem, typename GetId>
std::enable_if_t<is_iterable<toAddModList>::value> calculate_addmodremlist_diff(const List&        src_list,
                                                                                const List&        target_list,
                                                                                toAddModList&      add_diff_list,
                                                                                RemoveList&        rem_diff_list,
                                                                                const ConvertElem& convert_func,
                                                                                const GetId&       id_func)
{
  if (&src_list == &target_list) {
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
  calculate_addmodremlist_diff(src_list, target_list, rem_func, add_func, mod_func, id_func);
}

template <typename toAddModList, typename RemoveList, typename GetId>
void calculate_addmodremlist_diff(const toAddModList& src_list,
                                  const toAddModList& target_list,
                                  toAddModList&       add_diff_list,
                                  RemoveList&         rem_diff_list,
                                  const GetId&        id_func)
{
  using elem_type = decltype(*src_list.begin());

  if (&src_list == &add_diff_list) {
    // use const src_list instead.
    toAddModList src_list2 = src_list;
    calculate_addmodremlist_diff(src_list2, target_list, add_diff_list, rem_diff_list, id_func);
    return;
  }

  if (&target_list == &add_diff_list) {
    // use const target_list
    toAddModList target_list2 = target_list;
    calculate_addmodremlist_diff(src_list, target_list2, add_diff_list, rem_diff_list, id_func);
    return;
  }

  calculate_addmodremlist_diff(
      src_list, target_list, add_diff_list, rem_diff_list, [](const elem_type& e) { return e; }, id_func);
}

} // namespace srsgnb
