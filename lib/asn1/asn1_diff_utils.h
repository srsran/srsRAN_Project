
#pragma once

#include "srsgnb/support/async/detail/function_signature.h"
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

} // namespace srsgnb
